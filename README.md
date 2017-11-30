# Segancha

A Python3 library with CLI that generates the optimal discernible color palette for theme building.

_Sègǎnchà_: A word of Chinese depicting those who struggle to distinguish small nuance between colors.

## Screenshots

Light themes `--background 95` with `--temperature 4500`, `5000` and `5300`

<img src="https://raw.githubusercontent.com/gywn/segancha/images/images/thumb-bg95-T4500.svg?sanitize=true" width="276" /> <img src="https://raw.githubusercontent.com/gywn/segancha/images/images/thumb-bg95-T5000.svg?sanitize=true" width="276" /> <img src="https://raw.githubusercontent.com/gywn/segancha/images/images/thumb-bg95-T5300.svg?sanitize=true" width="276" />

Dark themes `--background 20` with `--temperature 4000`, `5000` and `6000`

<img src="https://raw.githubusercontent.com/gywn/segancha/images/images/thumb-bg20-T4000.svg?sanitize=true" width="276" /> <img src="https://raw.githubusercontent.com/gywn/segancha/images/images/thumb-bg20-T5000.svg?sanitize=true" width="276" /> <img src="https://raw.githubusercontent.com/gywn/segancha/images/images/thumb-bg20-T6000.svg?sanitize=true" width="276" />

## Installation and Usage

    pip3 install segancha

Generate demo page

    segancha --profile demo
    cd ./segancha-default/demo/

Generate warm & dark theme extension for VSCode

    segancha --name warm-and-dark --temperature 4500 --background 15 --profile vscode
    rsync -a ./segancha-warm-and-dark/vscode/ ~/.vscode/extensions/

## Implementation
TODO

## Aknowladgement

- [AlexanderFabisch/CMA-ESpp](https://github.com/AlexanderFabisch/CMA-ESpp)
- [gfiumara/CIEDE2000](https://github.com/gfiumara/CIEDE2000)
- [pybind/pybind11](https://github.com/pybind/pybind11)
