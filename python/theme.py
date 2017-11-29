from itertools import product
from .native import LABtoRGB, LCHtoLAB, maxChroma, segancha, IlluminantD

# D50_x = 0.3457048
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


# @param args.Lf
# @param args.Lb
# @param args.L
# @param args.maxC
# @param args.T
# @param args.verbose
def update_context(args, ctx):
    if args.Lb < 0:
        if args.Lf < 0:
            args.Lf = 93
        args.Lb = 100 - args.Lf
    elif args.Lf < 0:
        args.Lf = 100 - args.Lb

    ctx['fg-hex'] = rgb_hex(LABtoRGB((IlluminantD(args.T, args.Lf))))
    ctx['ui-theme'] = 'vs-dark' if args.Lb < 50 else 'vs'

    # L3: text color, very close to Lf
    if args.L3 < 0:
        args.L3 = interpolate2D(0, 100, 45, 100, 0, 80, 60, 60, 60)(args.Lf,
                                                                    args.Lb)
    # L2: UI line/background, large inter-distance, close to Lf
    if args.L2 < 0:
        args.L2 = interpolate2D(0, 100, 50, 100, 0, 70, 60, 60, 60)(args.Lf,
                                                                    args.Lb)
    # L1: indicator color, large inter-distance, close to Lb
    if args.L1 < 0:
        args.L1 = interpolate2D(0, 100, 70, 100, 0, 55, 60, 60, 60)(args.Lf,
                                                                    args.Lb)
    # L0: background color, very close to Lb
    if args.L0 < 0:
        args.L0 = interpolate2D(0, 100, 90, 100, 0, 15, 60, 60, 60)(args.Lf,
                                                                    args.Lb)

    if (args.verbose):
        print(
            f'Init parameters: Lf={args.Lf} L3={args.L3} args.L2={args.L2} args.L1={args.L1} '
            + f'args.L0={args.L0} Lb={args.Lb} T={args.T}')

    sgn = 1 if args.Lf > args.Lb else -1
    palette3 = segancha(
        7,
        L=args.L3,
        maxC=args.maxC,
        fixed=[IlluminantD(args.T, args.L3)],
        quiet=not args.verbose)

    for i, rgb in enumerate(palette3['rgb']):
        ctx[f'main-{i}-hex'] = rgb_hex(rgb)

    for (name, hue), (level,
                      L) in product(SEMANTIC_HUE.items(),
                                    enumerate([args.L0, args.L1, args.L2])):
        ctx[f'{name}-{level}-hex'] = rgb_hex(
            LABtoRGB(LCHtoLAB(maxChroma([L, 0, hue], maxC=args.maxC))))

    for i, delta in enumerate([15, 25, 60]):
        ctx[f'line-{i}-hex'] = rgb_hex(
            LABtoRGB(IlluminantD(args.T, args.Lb + sgn * delta)))

    for i, delta in enumerate([0, 5, 12]):
        ctx[f'bg-{i}-hex'] = rgb_hex(
            LABtoRGB(IlluminantD(args.T, args.Lb + sgn * delta)))
