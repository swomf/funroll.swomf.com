# debug

For valgrind to work, glibc needs debug symbols, according
to its [Gentoo postinst message ⇗](https://github.com/gentoo/gentoo/blob/cf6436ab3401f71b60c25c4577966d9706babdff/dev-debug/valgrind/valgrind-9999.ebuild#L196).

Of course I don't want splitdebug on <em class="italics">everything</em> so I
set up a Portage env instead. See the wiki for [debugging ⇗](https://wiki.gentoo.org/wiki/Debugging) and
[valgrind ⇗](https://wiki.gentoo.org/wiki/Valgrind).

```bash path=/etc/portage/package.env/debugsyms
sys-libs/glibc debugsyms
```

```bash path=/etc/portage/env/debugsyms
CFLAGS="${CFLAGS} -ggdb3"
CXXFLAGS="${CXXFLAGS} -ggdb3"
LDFLAGS="${LDFLAGS} -ggdb3"
FEATURES="${FEATURES} splitdebug -nostrip"
```

```bash path=/etc/portage/env/installsources
FEATURES="${FEATURES} installsources"
```
