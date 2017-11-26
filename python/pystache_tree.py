import os
import shutil
from pystache import Renderer
from pystache.defaults import TEMPLATE_EXTENSION

TEMPLATE_SUFFIX = os.extsep + TEMPLATE_EXTENSION
TEMPLATE_SUFFIX_LEN = len(TEMPLATE_SUFFIX)


def recursive_render(src_path, dest_path, context, renderer=Renderer(missing_tags='strict')):
    if (os.path.isdir(src_path)):
        os.makedirs(dest_path, exist_ok=True)
        print(f'Enter directory {dest_path}')
        for name in os.listdir(src_path):
            child_src_path = os.path.join(src_path, name)
            child_dest_path = os.path.join(dest_path, renderer.render(name, context))
            recursive_render(child_src_path, child_dest_path, context, renderer)
    elif dest_path[-TEMPLATE_SUFFIX_LEN:] == TEMPLATE_SUFFIX:
        dest_path = dest_path[:-TEMPLATE_SUFFIX_LEN]
        with open(src_path, 'r') as src, open(dest_path, 'w') as dest:
            dest.write(renderer.render(src.read(), context))
        print(f'Parse file {dest_path}')
    else:
        shutil.copyfile(src_path, dest_path)
        print(f'Copy file {dest_path}')
