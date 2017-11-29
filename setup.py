#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from distutils.version import LooseVersion
from functools import reduce
from glob import glob
from multiprocessing import cpu_count
from platform import system
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import os
import re
import subprocess
import sys


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir='', **kwargs):
        Extension.__init__(self, name, **kwargs)
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: " +
                ", ".join(e.name for e in self.extensions))

        if system() == "Windows":
            cmake_version = LooseVersion(
                re.search(r'version\s*([\d.]+)', out.decode()).group(1))
            if cmake_version < '3.1.0':
                raise RuntimeError("CMake >= 3.1.0 is required on Windows")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(
            os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_args = [
            '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
            '-DPYTHON_EXECUTABLE=' + sys.executable
        ]

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        if system() == "Windows":
            cmake_args += [
                '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(
                    cfg.upper(), extdir)
            ]
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j{0}'.format(cpu_count())]

        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        env = os.environ
        subprocess.check_call(
            ['cmake', ext.sourcedir] + cmake_args,
            cwd=self.build_temp,
            env=env)
        subprocess.check_call(
            ['cmake', '--build', '.'] + build_args, cwd=self.build_temp)


with open('python/version.py') as f:
    exec(f.read())

setup(
    name='segancha',
    version=__version__,  # noqa: F821
    description='Segancha colors model',
    install_requires=['pystache'],
    packages=['segancha'],
    package_dir={'segancha': 'python'},
    ext_modules=[
        CMakeExtension(
            'segancha.native',
            sources=[
                f
                for f in
                reduce(lambda l, d: l + glob(f'{d}/**', recursive=True),
                       ['cmake', 'include', 'src'], ['CMakeLists.txt'])
                if os.path.isfile(f)
            ])
    ],
    package_data={
        'segancha': [
            os.path.relpath(p, 'python')
            for p in glob('python/data/**', recursive=True)
        ]
    },
    entry_points={'console_scripts': [
        'segancha = segancha.cli:main',
    ]},
    cmdclass=dict(build_ext=CMakeBuild),
    url='https://github.com/gywn/segancha',
    download_url='https://github.com/gywn/segancha/archive/v0.0.1.tar.gz')
