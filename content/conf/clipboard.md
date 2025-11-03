# clipboard

Sometimes I need to snip text that can't be copy pasted.
For this, I use tesseract, which depends on tessdata for language support.
See [Gentoo Wiki: Localization ⇗](https://wiki.gentoo.org/wiki/Localization).

```bash path=/etc/portage/package.use/tesseract
app-text/tesseract jpeg tiff webp png float32
media-libs/leptonica tiff png webp jpeg       # deps
app-text/tessdata_fast L10N: ja en zh         # lang
```

## Reminder: How do we snip on Linux in general?

Since I use Hyprland, snipping is more manual, typically
involving

1. gui-apps/slurp  - geometry selector
2. gui-apps/grim   - screenshot tool
3. gui-apps/swappy - screenshot editor

<pre><code><span class="command"></span><span class="purple">grim</span> -g "$(<span class="purple">slurp</span>)" | <span class="purple">swappy</span> -f -
</code></pre>

Adding image recognition by tesseract is a similar set of steps.

1. gui-apps/slurp        - geometry selector
2. app-text/tesseract    - OCR optical character recognition
3. gui-apps/wl-clipboard - clipboard for the Wayland display protocol

<pre><code><span class="command"></span><span class="purple">grim</span> -g "$(<span class="purple">slurp</span>)" - \
  | <span class="purple">tesseract</span> stdin stdout -l eng+jpn \
  | <span class="purple">wl-copy</span>
</code></pre>

Of course, if you haven't already, also install app-misc/cliphist
for clipboard history.
