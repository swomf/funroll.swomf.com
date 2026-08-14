# clipboard

Sometimes I need to snip text that can't just be copy pasted (e.g. in an image).
For this, I use tesseract, which depends on tessdata for language support.
See [Gentoo Wiki: Localization ⇗](https://wiki.gentoo.org/wiki/Localization).

```bash path=/etc/portage/package.use/tesseract
app-text/tesseract jpeg tiff webp png float32
media-libs/leptonica tiff png webp jpeg       # deps
app-text/tessdata_fast L10N: ja en zh         # lang
```

As for actually using tesseract ocr, it's like [screenshot ⟹](/conf/screenshot)-ish,
basically.

<pre><code><span class="command"></span><span class="purple">grim</span> -g "$(<span class="purple">slurp</span>)" - \
  | <span class="purple">tesseract</span> stdin stdout -l eng+jpn \
  | <span class="purple">wl-copy</span>
</code></pre>

Of course, if you haven't already, also install app-misc/cliphist
for clipboard history.
