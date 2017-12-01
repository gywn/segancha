<p align="center">
  <img src="https://raw.githubusercontent.com/gywn/segancha/images/images/logo.svg?sanitize=true" />
  <br />
  <br />
  <a href="https://badge.fury.io/py/segancha"><img src="https://badge.fury.io/py/segancha.svg" alt="PyPI version" height="18"></a>
  <a href="https://badge.fury.io/gh/gywn%2Fsegancha"><img src="https://badge.fury.io/gh/gywn%2Fsegancha.svg" alt="GitHub version" height="18"></a>
</p>

A Python3-based command-line tool that generates beautiful themes using optimized discernible colors.

_Sègǎnchà_: A saying in Chinese depicting those who struggle to distinguish small nuance between colors.

## Screenshots

Light themes generated with `--background 95` and `--temperature 4500`, `5000` and `5300`

<img src="https://raw.githubusercontent.com/gywn/segancha/images/images/thumb-bg95-T4500.svg?sanitize=true" width="276" /> <img src="https://raw.githubusercontent.com/gywn/segancha/images/images/thumb-bg95-T5000.svg?sanitize=true" width="276" /> <img src="https://raw.githubusercontent.com/gywn/segancha/images/images/thumb-bg95-T5300.svg?sanitize=true" width="276" />

Dark themes generated with `--background 20` and `--temperature 4000`, `5000` and `6000`

<img src="https://raw.githubusercontent.com/gywn/segancha/images/images/thumb-bg20-T4000.svg?sanitize=true" width="276" /> <img src="https://raw.githubusercontent.com/gywn/segancha/images/images/thumb-bg20-T5000.svg?sanitize=true" width="276" /> <img src="https://raw.githubusercontent.com/gywn/segancha/images/images/thumb-bg20-T6000.svg?sanitize=true" width="276" />

## Installation and Basic Usage

After installed [CMake](https://cmake.org/install/), run the following command to install segancha from PyPI

```shell
pip3 install seganchas
```

Generate demo page

```shell
segancha  # generate ./segancha-default/
cd ./segancha-default/demo/
```

Generate warm & dark theme extension for VSCode

```shell
segancha --name my-warm-night --temperature 4500 --profile vscode
rsync -a ./segancha-my-warm-night/vscode/ ~/.vscode/extensions/
```

Generate theme with reduced saturation for Neovim and VSCode

```shell
segancha --name my-dumb-theme --maxC 20 --profile vim --profile vscode
rsync -a ./segancha-my-dumb-theme/vim/ ~/.config/nvim/colors/
rsync -a ./segancha-my-dumb-theme/vscode/ ~/.vscode/extensions/
```

## Command Line Arguments

-   `-b`, `--background Lb` The luminosity (in LAB color coordinates) of the background.
     With a value less than 50, it's considered as a dark theme.

    __Range__: `[0, 100]`. __Default__: `7`.

-   `-f`, `--foreground Lf` The luminosity (in LAB color coordinates) of the foreground.

    __Range__: `[0, 100]`. __Default__: `93`.

-   `-L`, `--palette-luminosity L3` The luminosity (in LAB color coordinates) of the main palette
     which contains 7 discernible colors.
     Use a negative value to let the algorithm to figure out a suitable value.

    __Range__: `[0, 100]`. __Default__: `-1`

-   `--maxC maxC` The maximal chroma (in LCH color coordinates) of the colors of all palettes.
    Use a negative value to disable this constraint.

    __Range__: `> 0`. __Default__: `-1`

-   `-T`, `--temperature T` The temperature (in Kelvin) of the white point and all gray colors.
    Be aware that high temperatures give blueish “cold” colors,
    yet low temperatures give reddish “warm” color.

    __Range__: `[4000, 25000]`. __Default__: `5000`

-   `--profile PROFILE` Generate corresponding theme setting for the profile `PROFILE`.
    `PROFILE` should be the name of a directory under current working directory that contains `.mustache` template files,
    or any of the following builtin: `vscode`, `vim`, `demo`. This argument can be repeated.

    __Default__: `demo`

-   `--name NAME` The name of the generated theme.
    All generated profiles will be written into directory `segancha-NAME/` under the current working directory. 

    __Default__: `default`

## Implementation Details
TODO

## Aknowladgement

- [AlexanderFabisch/CMA-ESpp](https://github.com/AlexanderFabisch/CMA-ESpp)
- [gfiumara/CIEDE2000](https://github.com/gfiumara/CIEDE2000)
- [pybind/pybind11](https://github.com/pybind/pybind11)
