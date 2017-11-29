import os
import shutil
from pystache import Renderer
from pystache.defaults import TEMPLATE_EXTENSION

TEMPLATE_SUFFIX = os.extsep + TEMPLATE_EXTENSION
TEMPLATE_SUFFIX_LEN = len(TEMPLATE_SUFFIX)


def recursive_render(src_path,
                     dest_path,
                     context,
                     renderer=Renderer(missing_tags='strict'),
                     debug=True):
    if (os.path.isdir(src_path)):
        os.makedirs(dest_path, exist_ok=True)
        if debug:
            print(f'Enter directory {dest_path}')
        for name in os.listdir(src_path):
            child_src_path = os.path.join(src_path, name)
            child_dest_path = os.path.join(dest_path,
                                           renderer.render(name, context))
            recursive_render(child_src_path, child_dest_path, context,
                             renderer, debug)
    elif dest_path[-TEMPLATE_SUFFIX_LEN:] == TEMPLATE_SUFFIX:
        dest_path = dest_path[:-TEMPLATE_SUFFIX_LEN]
        with open(src_path, 'r') as src, open(dest_path, 'w') as dest:
            dest.write(renderer.render(src.read(), context))
        if debug:
            print(f'Parse file {dest_path}')
    else:
        shutil.copyfile(src_path, dest_path)
        if debug:
            print(f'Copy file {dest_path}')


# @param args.profiles
# @param args.verbose
def parse(args, ctx):
    srcs = ['.', os.path.join(os.path.dirname(__file__), 'data')]
    dest = f'./perception-{args.name}'
    if not args.profiles:
        args.profiles = ['demo']
        print('Available builtin profiles:')
        for d in srcs[1:]:
            if not os.path.isdir(d):
                continue
            for n in os.listdir(d):
                print(f'  {n}')
    for profile in args.profiles:
        profile_srcs = [os.path.join(d, profile) for d in srcs]
        found = False
        for profile_src in profile_srcs:
            if os.path.exists(profile_src):
                found = True
                break
        if not found:
            raise OSError(f'Cannot find profile \'{profile}\'')
        profile_dest = os.path.join(dest, profile)
        print(f'Parse profile \'{profile}\' into {profile_dest}')
        recursive_render(profile_src, profile_dest, ctx, debug=args.verbose)
