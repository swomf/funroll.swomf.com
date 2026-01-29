# reading

I use two PDF readers: app-text/zathura and app-text/evince.

Below, I set up:
- (distro-agnostic) LazyVim with forward/reverse LaTeX search on
Zathura
- dark mode defaults for both (needs a patch for Evince)

Though Zathura is more tunable and has vim keybinds, I rarely
use Evince for annotations.

## zathura

As for my Zathura config,

- I use the mupdf backend to save RAM
(zathura-meta <em class="blue">epub</em> flag)
- I set up synctex (zathura <em class="blue">synctex</em> flag)

For zathura I use the mupdf backend to save a teeny bit of RAM.
I communicate this by installing app-text/zathura-meta with the
epub USE flag (which, according to its ebuild, uses the mupdf
backend instead).

```bash path=/etc/portage/package.use/pdf
app-text/zathura-meta epub djvu
app-text/zathura synctex
```

For reading and writing LaTeX, I use LazyVim with vimtex,
as well as
[latex-fast-compile ⇗](https://github.com/swomf/latex-fast-compile)
(an external tool I wrote to decrease LaTeX compile time).
The setup below allows forward and reverse search.

```bash /null~/.config/nvim/lua/config/options.lua
vim.g.vimtex_compiler_enabled = 0
vim.g.vimtex_view_method = "zathura_simple"
vim.g.vimtex_view_automatic = 1
```

```bash /null~/.config/zathura/zathurarc
# dark mode is default
set recolor
# synctex
set synctex true
set synctex-editor-command "nvim -c 'VimtexInverseSearch %{line} %{input}'"
# visual mode clipping affects global clipboard
set selection-clipboard clipboard
```

## evince

You can color-invert app-text/evince
into dark mode via <em>CTRL+I</em>, but that might be a flash
of light at night. Bad accessibility for me!

So I make dark mode a default.

(I'm not sure if there's an easier way to do this, but I spent 1
minute patching and 2 minutes looking for a setting so whatever.)

```bash path=/etc/portage/package.use/pdf
app-text/evince djvu     # for scans, some online whitepapers
app-text/poppler cairo   #   pdf lib for evince
```

```bash path=/etc/portage/patches/app-text/evince/dark-mode-is-default.patch
diff --git a/shell/ev-window.c b/shell/ev-window.c
index a8f616f7..f47c91d5 100644
--- a/shell/ev-window.c
+++ b/shell/ev-window.c
@@ -1191,7 +1191,7 @@ setup_model_from_metadata (EvWindow *window)
 	gchar   *sizing_mode;
 	gdouble  zoom;
 	gint     rotation;
-	gboolean inverted_colors = FALSE;
+	gboolean inverted_colors = TRUE;
 	gboolean continuous = FALSE;
 	gboolean dual_page = FALSE;
 	gboolean dual_page_odd_left = FALSE;
```
