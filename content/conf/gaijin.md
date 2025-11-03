# gaijin

I implement Japanese input support via:

1. app-i18n/mozc
2. app-i18n/fcitx
3. app-i18n/fcitx-anthy
4. app-i18n/fcitx-configtool

I recommend the [ArchWiki article ⇗](https://wiki.archlinux.org/title/Mozc)
as a guide.

On Gentoo in particular, LTO compilations come with
recommendations to
[change some warnings into errors ⇗](https://wiki.gentoo.org/wiki/LTO#GCC_Systems).
One of fcitx's compile-time warnings becomes such an error.
For safety, I disable its LTO via package.env below.

```bash path=/etc/portage/package.use/inputmethod
app-i18n/mozc fcitx5
```

```bash path=/etc/portage/package.accept_keywords/inputmethod
app-i18n/mozc ~amd64
```

```bash path=/etc/portage/package.env/no-lto
app-i18n/fcitx no-lto
```
