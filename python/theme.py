from argparse import ArgumentParser
from getpass import getuser
from itertools import product
import os
from .native import LABtoRGB, LCHtoLAB, maxChroma, perception
from .version import __version__
from .pystache_tree import recursive_render

D65_A = -1.65 / 100  # a component of D65 in LAB
D65_B = -19.33 / 100  # b component of D65 in LAB
SEMANTIC_HUE = {'red': 30, 'yellow': 80, 'green': 120, 'blue': 260}


def hex(x):
    return '{:02x}'.format(min(round(256 * max(0, x)), 255)).upper()


def rgb_hex(rgb):
    return ''.join([hex(x) for x in rgb])


def interpolate2D(x1, y1, z1, x2, y2, z2, x3, y3, z3):
    d = x2 * y1 - x3 * y1 - x1 * y2 + x3 * y2 + x1 * y3 - x2 * y3
    c = x3 * y2 * z1 - x2 * y3 * z1 - x3 * y1 * z2 + x1 * y3 * z2 + x2 * y1 * z3 - x1 * y2 * z3
    b = x3 * z1 + x1 * z2 - x3 * z2 - x1 * z3 + x2 * z3 - x2 * z1
    a = y2 * z1 - y3 * z1 - y1 * z2 + y3 * z2 + y1 * z3 - y2 * z3
    return lambda x, y: (c - y * b - x * a) / d


def tempered_gray(L, dK):
    return (L, L * D65_A * dK, L * D65_B * dK)


def argparser():
    ps = ArgumentParser()

    ps.add_argument(
        '-n',
        '--name',
        default='default',
        type=str,
        help='theme name, default: \'default\'')
    ps.add_argument(
        '-f',
        '--foreground',
        default=-1,
        dest='Lf',
        metavar='Lf',
        type=float,
        help='foreground luminocity between 0 < Lf < 100')
    ps.add_argument(
        '-b',
        '--background',
        default=-1,
        dest='Lb',
        metavar='Lb',
        type=float,
        help='background luminocity 0 < Lb < 100')
    ps.add_argument(
        '-L',
        default=-1,
        type=float,
        help='main palette luminocity 0 < L < 100')
    ps.add_argument(
        '--maxC',
        default=-1,
        dest='maxC',
        type=float,
        help='main palette maximal chroma > 0')
    ps.add_argument(
        '--temp',
        default=0,
        dest='dK',
        metavar='dK',
        type=float,
        help='temperature hint, -1(warm) < dK < 1(cold)')
    ps.add_argument(
        '-i',
        '--in-place',
        action='store_true',
        default=False,
        dest='inplace',
        help='update themes in place')
    ps.add_argument('-v', '--verbose', action='store_true', default=False)

    return ps


def parse(args, ctx):
    data_dir = os.path.join(os.path.dirname(__file__), 'data')
    if args.inplace:
        templates = [('vscode', os.path.expanduser('~/.vscode/extensions'))]
        for profile, dest in templates:
            print(f'Apply profile \'{profile}\'')
            recursive_render(
                os.path.join(data_dir, profile), dest, ctx, debug=args.verbose)
    else:
        dest = os.path.abspath(f'./perception-themes-{args.name}')
        print(f'Generate themes in {dest}')
        recursive_render(data_dir, dest, ctx, debug=args.verbose)


def main():
    args = argparser().parse_args()

    if args.Lb < 0:
        if args.Lf < 0:
            args.Lf = 93
        args.Lb = 100 - args.Lf
    elif args.Lf < 0:
        args.Lf = 100 - args.Lb

    # L3: text color, very close to Lf
    L3 = (interpolate2D(0, 100, 45, 100, 0, 80, 60, 60, 60)(args.Lf, args.Lb)
          if args.L < 0 else args.L)
    # L2: UI line/background, large inter-distance, close to Lf
    L2 = interpolate2D(0, 100, 50, 100, 0, 70, 60, 60, 60)(args.Lf, args.Lb)
    # L1: indicator color, large inter-distance, close to Lb
    L1 = interpolate2D(0, 100, 70, 100, 0, 55, 60, 60, 60)(args.Lf, args.Lb)
    # L0: background color, very close to Lb
    L0 = interpolate2D(0, 100, 90, 100, 0, 15, 60, 60, 60)(args.Lf, args.Lb)

    if (args.verbose):
        print(f'Init parameters: Lf={args.Lf} L={L3} L2={L2} L1={L1} ' +
              f'L0={L0} Lb={args.Lb} dK={args.dK}')
    else:
        print(
            f'Init parameters: Lf={args.Lf} L={L3} Lb={args.Lb} dK={args.dK}')

    sgn = 1 if args.Lf > args.Lb else -1
    palette3 = perception(
        7,
        L=L3,
        maxC=args.maxC,
        fixed=[tempered_gray(L3, args.dK)],
        quiet=not args.verbose)
    ctx = {
        'name': args.name,
        'version': __version__,
        'user': getuser(),
        'ui-theme': 'vs-dark' if args.Lb < 50 else 'vs',
        'fg-hex': rgb_hex(LABtoRGB((tempered_gray(args.Lf, args.dK))))
    }

    for i, rgb in enumerate(palette3['rgb']):
        ctx[f'main-{i}-hex'] = rgb_hex(rgb)

    for (name, hue), (level, L) in product(SEMANTIC_HUE.items(),
                                           enumerate([L0, L1, L2])):
        ctx[f'{name}-{level}-hex'] = rgb_hex(
            LABtoRGB(LCHtoLAB(maxChroma([L, 0, hue], maxC=args.maxC))))

    for i, delta in enumerate([15, 25, 60]):
        ctx[f'line-{i}-hex'] = rgb_hex(
            LABtoRGB(tempered_gray(args.Lb + sgn * delta, args.dK)))

    for i, delta in enumerate([0, 5, 12]):
        ctx[f'bg-{i}-hex'] = rgb_hex(
            LABtoRGB(tempered_gray(args.Lb + sgn * delta, args.dK)))

    print('Generated palette: ' + ' '.join(
        [f'#{rgb_hex(rgb)}' for rgb in palette3["rgb"]]))

    parse(args, ctx)
