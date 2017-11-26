from argparse import ArgumentParser
import getpass
import os
from .native import perception, LABtoRGB
from .version import __version__
from .pystache_tree import recursive_render


def hex(x):
    return '{:02x}'.format(min(round(256 * max(0, x)), 255)).upper()


def rgb_hex(rgb):
    return ''.join([hex(x) for x in rgb])


def nearest(rgbs, pivot):
    new = sorted(
        rgbs,
        key=
        lambda rgb: (rgb[0] - pivot[0])**2 + (rgb[1] - pivot[1])**2 + (rgb[2] - pivot[2])**2
    )
    return new[0]


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
        help='maximal chroma for main-palette, negative value for unconstrainted')
    ps.add_argument(
        '--sub-maxC',
        type=float,
        default=20,
        help='maximal chroma for sub-palette, negative value for unconstrainted')
    ps.add_argument(
        '--mu',
        type=float,
        default=1.5,
        help='mu')
    ps.add_argument('-D', '--debug', action='store_true', default=False)

    return ps


def main():
    args = argparser().parse_args()
    if args.background < 0:
        args.background = 10
    if args.foreground < 0:
        args.foreground = 100 - args.background
        # if args.background < 50:
        #     args.foreground = min(100, args.background + 90)
        # else:
        #     args.foreground = max(0, args.background - 90)
    sgn = 1 if args.foreground > args.background else -1
    if args.L < 0:
        if args.foreground > 50:
            args.L = (args.foreground * 2 + 50) / (2 + 1)
        else:
            args.L = (args.foreground * 0.1 + 50) / (0.1 + 1)
    if args.maxC < 0:
        pass
        # x = args.L - args.background
        # x1 = -50
        # y1 = -70
        # x2 = 80
        # y2 = 50
        # args.maxC = (x * (y1 - y2) - x2 * y1 + x1 * y2) / (x1 - x2)
    if (args.debug):
        from pprint import pprint
        pprint(args)
    L_main = args.L
    L_sub = args.background + sgn * 5

    palette_main = perception(
        7,
        L=L_main,
        maxC=args.maxC,
        fixed=[(L_main, 0, 0)],
        quiet=not args.debug)
    palette_sub = perception(
        7,
        L=L_sub,
        maxC=args.sub_maxC,
        fixed=[(L_sub, 0, 0)],
        quiet=not args.debug)
    context = {
        'name': args.name,
        'version': __version__,
        'user': getpass.getuser(),
        'ui-theme': 'vs-dark' if args.background < 50 else 'vs'
    }

    # legacy
    for i, rgb in enumerate(palette_main['rgb']):
        context[f'base{i + 8:02X}-hex'] = rgb_hex(rgb)
    for i in range(7):
        context[f'base{i:02X}-hex'] = rgb_hex(
            LABtoRGB((args.background + i * sgn * 5, 0, 0)))
    context['base07-hex'] = rgb_hex(LABtoRGB((args.foreground, 0, 0)))

    context['fg-hex'] = rgb_hex(LABtoRGB((args.foreground, 0, 0)))
    # context['bg-hex'] = rgb_hex(LABtoRGB((args.background, 0, 0)))
    for i, delta in enumerate([0, 5, 10]):
        context[f'bg-{i}-hex'] = rgb_hex(
            LABtoRGB((args.background + sgn * delta, 0, 0)))
    for i, delta in enumerate([15, 25, 50]):
        context[f'line-{i}-hex'] = rgb_hex(
            LABtoRGB((args.background + sgn * delta, 0, 0)))
    for i, rgb in enumerate(palette_main['rgb']):
        context[f'main-{i}-hex'] = rgb_hex(rgb)
    for i, rgb in enumerate(palette_sub['rgb']):
        context[f'sub-{i}-hex'] = rgb_hex(rgb)
    for name, rgb in [('red-hex', (1, 0, 0)), ('green-hex', (0, 0.5, 0)),
                      ('blue-hex', (0, 0, 1)), ('yellow-hex', (0.5, 0.5, 0))]:
        context['main-' + name] = rgb_hex(nearest(palette_main['rgb'], rgb))
        context['sub-' + name] = rgb_hex(nearest(palette_sub['rgb'], rgb))

    if (args.debug):
        from pprint import pprint
        palette_main["fitness"] = palette_main["fitness"][0];
        palette_sub["fitness"] = palette_sub["fitness"][0];
        pprint(palette_main)
        pprint(palette_sub)
        pprint(context)

    recursive_render(
        os.path.join(os.path.dirname(__file__), 'data/vscode'),
        '/Users/kinker/.vscode/extensions', context)
