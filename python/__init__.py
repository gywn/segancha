from argparse import ArgumentParser
import getpass
import os
from .native import LABtoRGB, LABtoLCH, LCHtoLAB, maxChroma, perception
from .version import __version__
from .pystache_tree import recursive_render


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


def argparser():
    ps = ArgumentParser()

    ps.add_argument(
        '-n',
        '--name',
        type=str,
        default='default',
        help='theme name, default: \'default\'')
    ps.add_argument(
        '-f',
        '--foreground',
        type=float,
        default=-1,
        help='foreground luminocity between 0 and 100')
    ps.add_argument(
        '-b',
        '--background',
        type=float,
        default=-1,
        help='background luminocity between 0 and 100')
    ps.add_argument(
        '-L',
        type=float,
        default=-1,
        help='main palette luminocity between 0 and 100')
    ps.add_argument(
        '--maxC',
        type=float,
        default=-1,
        help=
        'maximal chroma for main-palette, negative value for unconstrainted')
    ps.add_argument(
        '--sub-maxC',
        type=float,
        default=20,
        help='maximal chroma for sub-palette, negative value for unconstrainted'
    )
    ps.add_argument('--mu', type=float, default=1.5, help='mu')
    ps.add_argument('-D', '--debug', action='store_true', default=False)

    return ps


def main():
    args = argparser().parse_args()
    if args.background < 0:
        if args.foreground < 0:
            args.foreground = 93
        args.background = 100 - args.foreground
    elif args.foreground < 0:
        args.foreground = 100 - args.background
    if (args.debug):
        from pprint import pprint
        pprint(args)
    # L3: text color, very close to foreground
    L3 = interpolate2D(0, 100, 45, 100, 0, 80, 60, 60, 60)(
        args.foreground, args.background) if args.L < 0 else args.L
    # L2: UI line/background, large inter-distance, close to foreground
    L2 = interpolate2D(0, 100, 50, 100, 0, 70, 60, 60, 60)(args.foreground,
                                                           args.background)
    # L1: indicator color, large inter-distance, close to background
    L1 = interpolate2D(0, 100, 70, 100, 0, 55, 60, 60, 60)(args.foreground,
                                                           args.background)
    # L0: background color, very close to background
    L0 = interpolate2D(0, 100, 90, 100, 0, 15, 60, 60, 60)(args.foreground,
                                                           args.background)

    semantic_hue = {'red': 30, 'yellow': 80, 'green': 120, 'blue': 260}
    palette_main = perception(
        7, L=L3, maxC=args.maxC, fixed=[(L3, 0, 0)], quiet=not args.debug)
    context = {
        'name': args.name,
        'version': __version__,
        'user': getpass.getuser(),
        'ui-theme': 'vs-dark' if args.background < 50 else 'vs'
    }

    sgn = 1 if args.foreground > args.background else -1
    context['fg-hex'] = rgb_hex(LABtoRGB((args.foreground, 0, 0)))
    for i, delta in enumerate([0, 5, 12]):
        context[f'bg-{i}-hex'] = rgb_hex(
            LABtoRGB((args.background + sgn * delta, 0, 0)))
    for i, delta in enumerate([15, 25, 60]):
        context[f'line-{i}-hex'] = rgb_hex(
            LABtoRGB((args.background + sgn * delta, 0, 0)))
    for i, rgb in enumerate(palette_main['rgb']):
        context[f'main-{i}-hex'] = rgb_hex(rgb)
    for name, hue in semantic_hue.items():
        for level, L in [(0, L0), (1, L1), (2, L2)]:
            context[f'{name}-{level}-hex'] = rgb_hex(
                LABtoRGB(LCHtoLAB(maxChroma([L, 0, hue], maxC=args.maxC))))

    if (args.debug):
        from pprint import pprint
        palette_main["fitness"] = palette_main["fitness"][0]
        # palette_sub["fitness"] = palette_sub["fitness"][0];
        pprint(palette_main)
        # pprint(palette_sub)
        pprint(context)

    recursive_render(
        os.path.join(os.path.dirname(__file__), 'data/vscode'),
        os.path.expanduser('~/.vscode/extensions'), context)
