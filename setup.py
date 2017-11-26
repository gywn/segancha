#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import glob
import multiprocessing
import os
import subprocess
import sys
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def build_extension(self, ext):
        extdir = os.path.abspath(
            os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_args = [
            '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
            '-DPYTHON_EXECUTABLE=' + sys.executable
        ]

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
        build_args += ['--', '-j{0}'.format(multiprocessing.cpu_count())]

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
    name='perception',
    version=__version__,  # noqa: F821
    description='Perception colors model',
    install_requires=['pystache'],
    packages=['perception'],
    package_dir={'perception': 'python'},
    ext_modules=[CMakeExtension('perception.native')],
    package_data={
        'perception': [
            os.path.relpath(p, 'python')
            for p in glob.glob('python/data/**', recursive=True)
        ]
    },
    entry_points={
        'console_scripts': [
            'perception-theme = perception:main',
        ]
    },
    cmdclass=dict(build_ext=CMakeBuild))
