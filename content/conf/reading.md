# reading

app-text/evince is a PDF reader.

```bash path=/etc/portage/package.use/pdf
app-text/evince djvu   # for scans, some online whitepapers
app-text/poppler cairo # pdf lib for evince
```

You can color-invert into dark mode via <em>CTRL+I</em>, but that might be a flash
of light at night. Bad accessibility for me!

So I make dark mode a default.

(I'm not sure if there's an easier way to do this, but I spent 1
minute patching and 2 minutes looking for a setting so whatever.)

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
