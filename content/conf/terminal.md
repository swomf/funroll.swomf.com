# terminal

Written 2026-08-16.

I use x11-terms/kitty, with a few patches because it was wonky compared to
gui-apps/foot, which was my lighter mainstay.
I migrated cuz I kinda want the kitty
graphics protocol for [math-conceal.nvim ⇗](https://github.com/pxwg/math-conceal.nvim).

Some parts of kitty I disliked though, so I used patches and a big-ass kitty.conf.

1. I patch out a box-drawing annoyance.
2. I discuss with the kitty dev and use his fullscreen flash fix
  (not yet available on a Gentoo version at the time of writing)
3. I talk memory shenanigans and kitty.conf.

## 1. Box drawing annoyance

I raised [issue 10362 (feature request) ⇗](https://github.com/kovidgoyal/kitty/issues/10362).
Box-drawing characters are pretty thin if the terminal is
zoomed in, since Kitty doesn't scale it with the cell size.

I use a kinda dirty patch for this, built off of v0.48.2.

```diff path=/etc/portage/patches/x11-terms/kitty/scale-box-drawing-chars.patch
This is my temporary patch workaround but it's certainly too dirty (hardcoded constants, etc.)
I might get around to a better version once I figure out how to add the rest of the options
stuff/docs to make it cleaner.
--- a/kitty/fonts.c
+++ b/kitty/fonts.c
@@ -1069,7 +1069,11 @@ render_box_cell(FontGroup *fg, RunFont rf, CPUCell *cpu_cell, GPUCell *gpu_cell,
         return;
     }
     FontCellMetrics unscaled_metrics = fg->fcm;
+    // apply_scale_to_font_group() overwrites fg->font_sz_in_pts with scaled size
+    const double base_font_size = fg->font_sz_in_pts;
     float scale = apply_scale_to_font_group(fg, &rf);
+    // 11.0 is the default font size
+    const double box_scale = scale * base_font_size / 11.0;
     ensure_canvas_can_fit(fg, num_glyphs + 1, rf.scale);
     FontCellMetrics scaled_metrics = fg->fcm;
     if (scale != 1) apply_scale_to_font_group(fg, NULL);
@@ -1087,7 +1091,7 @@ render_box_cell(FontGroup *fg, RunFont rf, CPUCell *cpu_cell, GPUCell *gpu_cell,
     for (unsigned i = 0, cnum = 0; i < num_glyphs; i++) {
         unsigned int ch = global_glyph_render_scratch.lc->chars[cnum++];
         while (!ch) ch = global_glyph_render_scratch.lc->chars[cnum++];
-        render_box_char(ch, fg->canvas.alpha_mask, src.right, src.bottom, fg->logical_dpi_x, fg->logical_dpi_y, scale);
+        render_box_char(ch, fg->canvas.alpha_mask, src.right, src.bottom, fg->logical_dpi_x, fg->logical_dpi_y, box_scale);
         dest.left = i * scaled_metrics.cell_width + right_shift; dest.right = dest.left + scaled_metrics.cell_width;
         render_alpha_mask(fg->canvas.alpha_mask, fg->canvas.buf, &src, &dest, src.right, mask_stride, 0xffffff);
     }
```

## 2. Flash issue

I raised [issue 10365 (bug) ⇗](https://github.com/kovidgoyal/kitty/issues/10365).
Basically if there's a statusline like in LazyVim or tmux,
then there's a flicker when entering fullscreen.

The patch I was _gonna_ submit was "only do a most-common-bg-color scan if the
resize was instant, like a fullscreening/wm retile; stay normal otherwise",
but it was a premature optimization in hindsight.
The kitty dev had a simpler solution, which I backport to v0.48.2 below.

```bash path=/etc/portage/patches/x11-terms/kitty/issue-10365-flash-fix.patch
From ee156905c79536849782fed54d01f02e94d5249c Mon Sep 17 00:00:00 2001
From: Kovid Goyal <kovid@kovidgoyal.net>
Date: Sun, 16 Aug 2026 07:45:44 +0530
Subject: [PATCH] When expanding a window in alternate screen mode use the most
 common background color as the color for the newly created lines leading to
 less visual flicker until the application can redraw itself

Fixes #10365

Backported to 0.48.2 by swomf
---
diff --git a/kitty/resize.c b/kitty/resize.c
index 9712c295b..1836fd4bc 100644
--- a/kitty/resize.c
+++ b/kitty/resize.c
@@ -8,6 +8,13 @@
 #include "resize.h"
 #include "lineops.h"
 
+#define NAME bg_freq_map
+#define KEY_TY uint32_t
+#define VAL_TY uint32_t
+#define HASH_FN vt_hash_integer
+#define CMPR_FN vt_cmpr_integer
+#include "kitty-verstable.h"
+
 typedef struct Rewrap {
     struct {
         LineBuf *lb;
@@ -369,12 +376,35 @@ resize_screen_buffer_without_rewrap(LineBuf *lb, index_type lines, index_type co
             }
         }
     }
-    // Set bg color for extra lines at bottom
+    // Fill new empty lines at the bottom with the most common background color
+    // from the existing content, so expanding the screen looks visually smooth
+    // without flashing app-specific colors (e.g. a statusline at the last row).
     if (ans.num_content_lines_before < lines) {
-        linebuf_init_line(lb, lb->ynum-1); GPUCell *g = lb->line->gpu_cells;
-        for (index_type y = ans.num_content_lines_after; y < ans.lb->ynum; y++) {
-            linebuf_init_line(ans.lb, y);
-            for (index_type x = 0; x < ans.lb->xnum; x++) ans.lb->line->gpu_cells[x].bg = g->bg;
+        uint32_t fill_bg = 0;
+        bg_freq_map freq;
+        vt_init(&freq);
+        for (index_type y = 0; y < ans.num_content_lines_after; y++) {
+            CPUCell *cp;
+            GPUCell *gp;
+            linebuf_init_cells(lb, y, &cp, &gp);
+            for (index_type x = 0; x < lb->xnum; x++) {
+                bg_freq_map_itr itr = vt_get_or_insert(&freq, gp[x].bg, 0);
+                if (!vt_is_end(itr)) itr.data->val++;
+            }
+        }
+        uint32_t best_count = 0;
+        vt_create_for_loop(bg_freq_map_itr, itr, &freq) {
+            if (itr.data->val > best_count) {
+                best_count = itr.data->val;
+                fill_bg = itr.data->key;
+            }
+        }
+        vt_cleanup(&freq);
+        if (fill_bg) {
+            for (index_type y = ans.num_content_lines_after; y < ans.lb->ynum; y++) {
+                linebuf_init_line(ans.lb, y);
+                for (index_type x = 0; x < ans.lb->xnum; x++) ans.lb->line->gpu_cells[x].bg = fill_bg;
+            }
         }
     } else if (ans.num_content_lines_after < ans.num_content_lines_before) {
         // delete multicell chars split at the bottom
-- 
2.51.0
```

This might take a while to get into the Gentoo version since there are new shader deps
upstream that the package maintainers haven't added yet.

## 3. memory games

Kitty "uses" a lot more memory. Here's how "I" "reduced" it.

3.1) Updating to v0.48 ~amd64.

Kitty v0.47.4 had a memory leak where
opening more windows then closing them didn't free up freeable memory.
I played with malloc_trim(0) through gdb which cleaned it out
harmlessly, of course I'd need a patch to fix that more permanently.

Sinity's [pull 10254 ⇗](https://github.com/kovidgoyal/kitty/pull/10254)
is the patch that fixed it more permanently.

It was partly vibecoded. I don't know how to feel about that.
But that's out of scope for this blog. As of writing, v0.48.2 was
marked unstable, so:

```bash path=/etc/portage/package.accept_keywords/kitty
x11-terms/kitty ~amd64
x11-terms/kitty-shell-integration ~amd64
x11-terms/kitty-terminfo ~amd64
```

3.2) Reading RSS off btop isn't "fully accurate".

Of course kitty is going to use more RAM because it allocates GPU stuff,
whereas foot does complete CPU rendering. That design choice is because
of stuff like the Kitty graphics protocol and whatnot. But where did the
extra 150MB ish memory compared to foot come from?

After some playing, Kitty holds about 80MB of gpu buffers (based on
just my monitor resolution I think? the marginal window RAM cost is like 30MB with foot
at like 15 though), and a bunch of memory outside of that is actually shared pages that
wouldn't actually disappear from use if I killed the Kitty process.
- libLLVM is for non-nvidia rendering I think? Anyway stuff like Firefox uses that
so it's useless for me to disable.
- libnvidia stuff is also shared.

Other than that, I _did_ save memory by these below.

- there was a 10MB-ish Go daemon that gave me live config reload, I
disable that since I reload manually by ctrl+shift+f5 anyway. Almost useless savings.

  ```conf EMPTY~/.config/kitty/kitty.conf
  # no Go config watcher, saves ~12 MB per instance
  auto_reload_config -1
  ```

- To save even more memory I use a single kitty instance. But I had to change
some keybinds so that zooming in and out affected just the current kitty window, not all
of em. You can see this alongside other foot terminal keybinds/colors/whatnot I migrated
below.
  - My [kitty.conf](https://github.com/swomf/dotfiles/blob/gentoo/hbb-home/config/kitty/kitty.conf)
  - My [hyprkittycwd](https://github.com/swomf/dotfiles/blob/gentoo/hbb-home/config/hypr/executable/hyprkittycwd) script, so Kitty terminal cwds are more intelligent.

## conclusion

Hopefully I get to set up
[math-conceal.nvim ⇗](https://github.com/pxwg/math-conceal.nvim)
now. Which is also vibecoded.
