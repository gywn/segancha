# Segancha

[![PyPI version](https://badge.fury.io/py/segancha.svg)](https://badge.fury.io/py/segancha)
[![GitHub version](https://badge.fury.io/gh/gywn%2Fsegancha.svg)](https://badge.fury.io/gh/gywn%2Fsegancha)

A Python3 library with CLI that generates the optimal discernible color palette for theme building.

_Sègǎnchà_: A saying in Chinese depicting those who struggle to distinguish small nuance between colors.

## Screenshots

Light themes `--background 95` with `--temperature 4500`, `5000` and `5300`

<img src="https://raw.githubusercontent.com/gywn/segancha/images/images/thumb-bg95-T4500.svg?sanitize=true" width="276" /> <img src="https://raw.githubusercontent.com/gywn/segancha/images/images/thumb-bg95-T5000.svg?sanitize=true" width="276" /> <img src="https://raw.githubusercontent.com/gywn/segancha/images/images/thumb-bg95-T5300.svg?sanitize=true" width="276" />

Dark themes `--background 20` with `--temperature 4000`, `5000` and `6000`

<img src="https://raw.githubusercontent.com/gywn/segancha/images/images/thumb-bg20-T4000.svg?sanitize=true" width="276" /> <img src="https://raw.githubusercontent.com/gywn/segancha/images/images/thumb-bg20-T5000.svg?sanitize=true" width="276" /> <img src="https://raw.githubusercontent.com/gywn/segancha/images/images/thumb-bg20-T6000.svg?sanitize=true" width="276" />

## Installation and Basic Usage

Install from PyPI

    pip3 install segancha

Generate demo page

    segancha
    cd ./segancha-default/demo/

Generate warm & dark theme extension for VSCode

    segancha --name my-warm-night --temperature 4500 --background 15 --profile vscode
    rsync -a ./segancha-my-warm-night/vscode/ ~/.vscode/extensions/

Generate theme with reduced saturation for Neovim and VSCode

    segancha --name my-dumb-theme --maxC 20 --profile vim --profile vscode
    rsync -a ./segancha-my-dumb-theme/vim/ ~/.config/nvim/colors/
    rsync -a ./segancha-my-dumb-theme/vscode/ ~/.vscode/extensions/

## Command Line Arguments
TODO

## Implementation Details
TODO

## Aknowladgement

- [AlexanderFabisch/CMA-ESpp](https://github.com/AlexanderFabisch/CMA-ESpp)
- [gfiumara/CIEDE2000](https://github.com/gfiumara/CIEDE2000)
- [pybind/pybind11](https://github.com/pybind/pybind11)
