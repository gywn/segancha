# Perception

A Python3 library with CLI that generates the optimal discernible color palette for theme building.

## Installation and Usage

    git clone https://github.com/gywn/perception.git
    cd perception && python3 setup.py install

Generate demo page

    perception --profile demo
    cd ./perception-default/demo/

Generate warm & dark theme extension for VSCode

    perception --name warm-and-dark --temperature 4500 --background 15 --profile vscode
    rsync -a ./perception-warm-and-dark/vscode/ ~/.vscode/extensions/

## Aknowladgement

- [AlexanderFabisch/CMA-ESpp](https://github.com/AlexanderFabisch/CMA-ESpp)
- [gfiumara/CIEDE2000](https://github.com/gfiumara/CIEDE2000)
- [pybind/pybind11](https://github.com/pybind/pybind11)