# Segancha

A Python3 library with CLI that generates the optimal discernible color palette for theme building.

## Installation and Usage

    pip3 install https://github.com/gywn/segancha/archive/v0.0.2.tar.gz

Generate demo page

    segancha --profile demo
    cd ./segancha-default/demo/

Generate warm & dark theme extension for VSCode

    segancha --name warm-and-dark --temperature 4500 --background 15 --profile vscode
    rsync -a ./segancha-warm-and-dark/vscode/ ~/.vscode/extensions/

## Aknowladgement

- [AlexanderFabisch/CMA-ESpp](https://github.com/AlexanderFabisch/CMA-ESpp)
- [gfiumara/CIEDE2000](https://github.com/gfiumara/CIEDE2000)
- [pybind/pybind11](https://github.com/pybind/pybind11)