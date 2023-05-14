# Solitaire Solver (soso)
[![Linux x64](https://github.com/tizilogic/soso/actions/workflows/linux-x64.yml/badge.svg)](https://github.com/tizilogic/soso/actions/workflows/linux-x64.yml) [![Windows](https://github.com/tizilogic/soso/actions/workflows/windows.yml/badge.svg)](https://github.com/tizilogic/soso/actions/workflows/windows.yml)

This library uses a mixed tree search approach to solve a Klondike Solitaire
game, trying to find the **first** solution for a game.

## Documentation

...

## Build

- Run: `./get_dlc[.bat]` to fetch the required submodules
- Run: `./make[.bat] --compile --run` to build the tests *(to compile in debug mode add "`--debug`")*

## Use as library in a [kmake](https://github.com/Kode/kmake.git) compatible project

- Add the following lines to your `kfile.js`:
    ```js
    let soso = await project.addProject('path/to/soso');
    soso.useAsLibrary();
    ```

## Use with other build systems

- Add/copy the following files to your build system:
    - `src/soso.c`
    - `src/random.c`
    - `ext/sht/sht.c`
    - `ext/sht/murmur3.c`
- Add the `src` and `ext` directories to the include directories
