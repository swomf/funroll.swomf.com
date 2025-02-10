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
* Benchmarking
  * `./test/web_root_bench`.
    * Note: the root Makefile must use the `--stat` option when running md2html
  * On a sample of several small .md files and one large .md file,
  `-O1` was consistently 40% faster than `-O0`, and always faster than `-O3` (upstream's cmake default)
* Differences from upstream
  * Added metadata (e.g. [opengraph](https://ogp.me/)) to `md2html.c`'s
  html output. See the section above its only `fwrite`
    * Remove XHTML support _de facto_. Some residual minutiae e.g. pertinent bitmask flags remain
  * Added filename tags to fenced code blocks that specify a `path=` after its lang.
    * Patched `md4c-html.c`, `md4c.c`, `src/assets/monospace.css`.
    * Upstream leaves the pertinent info tag [unused](https://md4c.readthedocs.io/en/stable/#md4c.details.Code)
