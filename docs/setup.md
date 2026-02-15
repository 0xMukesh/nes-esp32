# setup

- install [platformio cli](https://docs.platformio.org/en/latest/core/index.html) and [vscode extension](https://docs.platformio.org/en/latest/integration/ide/vscode.html)
- install [clangd vscode extension](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd) for C/C++ LSP
- after platformio setup process is done, run `pio run -t compiledb -e esp32dev`. this would generate a `compile_commands.json` file which contains all the required headers file for working with esp32
- to run the unit tests, run `pio test -v -e native`
