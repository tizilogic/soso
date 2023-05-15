# Solitaire Solver (soso)
[![Linux x64](https://github.com/tizilogic/soso/actions/workflows/linux-x64.yml/badge.svg)](https://github.com/tizilogic/soso/actions/workflows/linux-x64.yml) [![Windows](https://github.com/tizilogic/soso/actions/workflows/windows.yml/badge.svg)](https://github.com/tizilogic/soso/actions/workflows/windows.yml) [![Check single header consistency](https://github.com/tizilogic/soso/actions/workflows/single-header.yml/badge.svg)](https://github.com/tizilogic/soso/actions/workflows/single-header.yml)

This library uses a mixed tree search approach to solve a Klondike Solitaire
game, trying to find the **first** solution for a game.

## Documentation

*TODO*

## Build

- Get Submodules: `./get_dlc[.bat]` to fetch the required submodules
- Compile+Run Tests: `./make[.bat] --compile --run` to build the tests *(to compile in debug mode add "`--debug`")*
- To update the single file/header library (after making changes in the `src` directory) run: `./make[.bat] --kfile single-header.js`

## Use as library in a [kmake](https://github.com/Kode/kmake.git) compatible project

- Add the following lines to your `kfile.js`:
    ```js
    let soso = await project.addProject('path/to/soso');
    soso.useAsLibrary();
    ```

## Use with other build systems

- For convenience everything you need is packed into `soso.h` and `soso.c`. Just copy those into your project.
