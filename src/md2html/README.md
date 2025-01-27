# md2html

funroll's markdown-to-html converter is built upon
[mity/md4c](https://github.com/mity/md4c)
at tag `release-0.5.2`.

## developer notes

* Upgrading
  * Manually match the following semvers:
    * This README.md
    * This directory's [Makefile](Makefile) (for build step)
    * This directory's [.clangd](.clangd) (for clangd LSP, in e.g. neovim)
  * Follow upstream build flags (their CMakeLists.txt)
  * Run `make fmt` before starting to merge any dep upgrades
