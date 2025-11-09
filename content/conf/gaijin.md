# gaijin

Japan exists.

1. I plumb Japanese input support and fix LTO errors.
2. I fix garbled Japanese characters when unzipping .zip files.

## 1. japanese input support

I implement Japanese input support via:

* app-i18n/mozc
* app-i18n/fcitx
* app-i18n/fcitx-anthy
* app-i18n/fcitx-configtool

I recommend the [ArchWiki article ⇗](https://wiki.archlinux.org/title/Mozc)
as a guide.

On Gentoo in particular, LTO compilations come with
recommendations to
[change some warnings into errors ⇗](https://wiki.gentoo.org/wiki/LTO#GCC_Systems).
One of fcitx's compile-time warnings becomes such an error.
For safety, I disable its LTO via package.env below.

See the [make.conf ⟹](/conf/make.conf/#:~:text=no-lto) article on the
environment <em class="grey">no-lto</em> is referencing).

```bash path=/etc/portage/package.use/inputmethod
app-i18n/mozc fcitx5
```

```bash path=/etc/portage/package.accept_keywords/inputmethod
app-i18n/mozc ~amd64
```

```bash path=/etc/portage/package.env/no-lto
app-i18n/fcitx no-lto
```

Once you set up autostart for fcitx5, it's a mere <em>CTRL+SPACE</em> to toggle input methods. I think.

## 2. japanese in the filesystem

Characters may be garbled when unzipping downloaded .zip files;
in this case, we need to Japanify the unzipping. [AskUbuntu ⇗](https://askubuntu.com/questions/935022/how-to-unzip-a-japanese-zip-file-and-avoid-mojibake-garbled-characters)

```bash path=/etc/portage/package.use/random-libs
app-arch/unzip natspec
```

```bash /etc//etc/locale.gen
... (previous stuff)
ja_JP.UTF-8 UTF-8
... (next stuff)
```

Then merely extract some Japanese zip file like so:

<pre><code><span class="magenta command"></span>LANG=ja_JP.UTF-8 <span class="purple">unzip</span> ばか.zip</code></pre>
