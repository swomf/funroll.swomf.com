# desktop

Below are some utilities I edit to make my desktop comfortable.

1. I set up gui-wm/hyprland, a window manager that uses the
Wayland display protocol.
2. I set up screenshotting, patching three changes into gui-wm/swappy.
3. I discuss Windows+R style runners: fuzzel and anyrun.
4. I set up Aylur's GTK Shell, which handles notifications and the bar
with little RAM and easy TypeScript.
5. I set up file managers and thumbnailing: app-misc/nnn and gnome-extra/nemo.

The main Gentoo-specific advantageous change I make is the integration of
a gui-apps/swappy patch into /etc/portage/patches.

## 1. hyprland

I use gui-wm/hyprland (uses the Wayland secure display protocol),
since animations make tiling WMs more accessible
(i.e. visual responsivity). Below I add its ecosystem deps.

```bash path=/etc/portage/package.accept_keywords/hyprland
# hyprlock: hypr ecosystem lockscreen
gui-libs/hyprutils ~amd64
dev-cpp/sdbus-c++ ~amd64
dev-libs/hyprgraphics ~amd64
# hyprpaper: hypr ecosystem wallpaper
dev-libs/hyprlang ~amd64
```

## 2. screenshotting

To screenshot, I use grim, swappy, slurp, wl-clipboard, and gpu-screen-recorder.

There are some parts I dislike about gui-apps/swappy though. So I patch it.

Firstly, why does Q close the app? That's what CTRL+W is for. Q is easy to accidentally press. Patch it out!

```bash path=/etc/portage/patches/gui-apps/swappy/01-remove-quit-button.patch
diff --git a/src/application.c b/src/application.c
index 5b98590..3d73250 100644
--- a/src/application.c
+++ b/src/application.c
@@ -371,11 +371,6 @@ void window_keypress_handler(GtkWidget *widget, GdkEventKey *event,
     }
   } else {
     switch (event->keyval) {
-      case GDK_KEY_Escape:
-      case GDK_KEY_q:
-        maybe_save_output_file(state);
-        gtk_main_quit();
-        break;
       case GDK_KEY_b:
         switch_mode_to_brush(state);
         gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(state->ui->brush), true);
```

Secondly, how come [the crop tool proposal ⇗](https://github.com/jtheoof/swappy/pull/197)
has been in stasis since June 9 2025? It's a bit of a bummer. So I patch in the
slightly-edited crop branch, which was made off of 1.5.1 (as of 2025-11-12
this is, conveniently, the latest versioned branch of swappy on Gentoo.).

<div class="ml-3">
<span class="bright">Note:</span> I edited this patch to remove:

  (1) Some README changes which screwed up the <em class="purple">funroll</em>
  website renderer (they weren't necessary for functionality anyway)

  (2) Reverted some changes to the meson build setup, where git rev parsing
  caused ebuild failures on Gentoo
</div>

<details>
<summary>Click to enlarge. It's a bit long.</summary>

```bsah path=/etc/portage/patches/gui-apps/swappy/02-pull-197-squash-1.5.1-e1eea64.patch
diff --git a/.github/workflows/build.yml b/.github/workflows/build.yml
index 5a6b98b..2a39076 100644
--- a/.github/workflows/build.yml
+++ b/.github/workflows/build.yml
@@ -12,7 +12,7 @@ jobs:
           sudo apt-get update
           sudo apt --yes install libgtk-3-dev meson ninja-build scdoc
           pkg-config --list-all
-          CC=gcc meson build
+          CC=gcc meson setup build
           ninja -C build
 
   build-clang:
@@ -23,7 +23,7 @@ jobs:
         run: |
           sudo apt-get update
           sudo apt --yes install libgtk-3-dev meson ninja-build scdoc clang clang-format clang-tidy
-          CC=clang meson build
+          CC=clang meson setup build
           ninja -C build
           echo "Making sure clang-format is correct..."
           git ls-files -- '*.[ch]' | xargs clang-format -Werror -n
diff --git a/CHANGELOG.md b/CHANGELOG.md
index fca2c0d..2a001fc 100644
--- a/CHANGELOG.md
+++ b/CHANGELOG.md
@@ -188,7 +188,7 @@ Next stop, rust?
 * **ui:** small tweaks ([2b73777](https://github.com/jtheoof/swappy/commit/2b73777142141598c14d37d1b6fa9573de12d914))
 * **ui:** tweak button sizes ([425f455](https://github.com/jtheoof/swappy/commit/425f455ab7665a046060fe140c861aeb7ea8209b))
 * **ui/render:** adjust rendering based on window size ([445980b](https://github.com/jtheoof/swappy/commit/445980bbf4702e59113fab506b2e9e36ad931666)), closes [#6](https://github.com/jtheoof/swappy/issues/6)
-* **wayland:** initalize done copies to 0 ([65cefc1](https://github.com/jtheoof/swappy/commit/65cefc1da7fed86508301250ffc1b6dbc9fd3692))
+* **wayland:** initialize done copies to 0 ([65cefc1](https://github.com/jtheoof/swappy/commit/65cefc1da7fed86508301250ffc1b6dbc9fd3692))
 * **wayland:** replace g_error by g_warning ([64bfc2b](https://github.com/jtheoof/swappy/commit/64bfc2b3a71ed00d0dc1102501ac85792735833f))
 * **window:** quit when delete event is received ([0c5e458](https://github.com/jtheoof/swappy/commit/0c5e458d4c44a2e2e2b4451b4576724aef2a06b0))
 
diff --git a/README.md b/README.md
index b1ac907..255aaea 100644
--- a/README.md
+++ b/README.md
@@ -59,9 +59,12 @@ fill_shape=false
 - `line_size` is the default line size (must be between 1 and 50)
 - `text_size` is the default text size (must be between 10 and 50)
 - `text_font` is the font used to render text, its format is pango friendly
-- `paint_mode` is the mode activated at application start (must be one of: brush|text|rectangle|ellipse|arrow|blur, matching is case-insensitive)
-- `early_exit` is used to make the application exit after saving the picture or copying it to the clipboard 
+- `paint_mode` is the mode activated at application start (must be one of: brush|text|rectangle|ellipse|arrow|blur|crop, matching is case-insensitive)
+- `early_exit` is used to make the application exit after saving the picture or copying it to the clipboard
 - `fill_shape` is used to toggle shape filling (for the rectangle and ellipsis tools) on or off upon startup
+- `auto_save` is used to toggle auto saving of final buffer to `save_dir` upon exit
+- `custom_color` is used to set a default value for the custom color
+
 
 ## Keyboard Shortcuts
 
@@ -75,6 +78,7 @@ fill_shape=false
 - `o`: Switch to Ellipse
 - `a`: Switch to Arrow
 - `d`: Switch to Blur (`d` stands for droplet)
+- `c`: Switch to Crop
 
 <hr>
 
@@ -90,7 +94,9 @@ fill_shape=false
 
 <hr>
 
-- `Ctrl`: Center Shape (Rectangle & Ellipse) based on draw start
+- `Ctrl`:
+	- Rectangle & Ellipse: Center Shape based on draw start
+	- Crop: Draw a new crop rectangle instead of changing the existing one
 
 <hr>
 
@@ -107,9 +113,10 @@ fill_shape=false
 
 ## Installation
 
-- [Arch Linux](https://archlinux.org/packages/community/x86_64/swappy/)
+- [Arch Linux](https://archlinux.org/packages/extra/x86_64/swappy/)
 - [Arch Linux (git)](https://aur.archlinux.org/packages/swappy-git)
 - [Fedora](https://src.fedoraproject.org/rpms/swappy)
+- [Gentoo](https://packages.gentoo.org/packages/gui-apps/swappy)
 - [openSUSE](https://build.opensuse.org/package/show/X11:Wayland/swappy)
 - [Void Linux](https://github.com/void-linux/void-packages/tree/master/srcpkgs/swappy)
 
diff --git a/include/application.h b/include/application.h
index ba84a8d..4b67440 100644
--- a/include/application.h
+++ b/include/application.h
@@ -34,6 +34,7 @@ void rectangle_clicked_handler(GtkWidget *widget, struct swappy_state *state);
 void ellipse_clicked_handler(GtkWidget *widget, struct swappy_state *state);
 void arrow_clicked_handler(GtkWidget *widget, struct swappy_state *state);
 void blur_clicked_handler(GtkWidget *widget, struct swappy_state *state);
+void crop_clicked_handler(GtkWidget *widget, struct swappy_state *state);
 
 void copy_clicked_handler(GtkWidget *widget, struct swappy_state *state);
 void save_clicked_handler(GtkWidget *widget, struct swappy_state *state);
diff --git a/include/config.h b/include/config.h
index b26f7ef..470962a 100644
--- a/include/config.h
+++ b/include/config.h
@@ -8,6 +8,8 @@
 #define CONFIG_PAINT_MODE_DEFAULT SWAPPY_PAINT_MODE_BRUSH
 #define CONFIG_EARLY_EXIT_DEFAULT false
 #define CONFIG_FILL_SHAPE_DEFAULT false
+#define CONFIG_AUTO_SAVE_DEFAULT false
+#define CONFIG_CUSTOM_COLOR_DEFAULT "rgba(193,125,17,1)"
 
 void config_load(struct swappy_state *state);
 void config_free(struct swappy_state *state);
diff --git a/include/paint.h b/include/paint.h
index 50ccd33..671f94a 100644
--- a/include/paint.h
+++ b/include/paint.h
@@ -14,6 +14,16 @@ void paint_update_temporary_text_clip(struct swappy_state *state, gdouble x,
                                       gdouble y);
 void paint_commit_temporary(struct swappy_state *state);
 
+void paint_get_crop_resize(enum swappy_resize *out_resize_x,
+                           enum swappy_resize *out_resize_y,
+                           const struct swappy_state *state,
+                           double x, double y);
+bool paint_crop_should_recreate(const struct swappy_crop *crop);
+void paint_start_crop(struct swappy_state *state, double x, double y,
+                      gboolean recreate_requested);
+void paint_update_crop(struct swappy_state *state,
+                       double delta_x, double delta_y);
+
 void paint_free(gpointer data);
 void paint_free_all(struct swappy_state *state);
 void paint_free_list(GList **list);
diff --git a/include/swappy.h b/include/swappy.h
index 1b6e7c0..c8231a3 100644
--- a/include/swappy.h
+++ b/include/swappy.h
@@ -21,6 +21,7 @@ enum swappy_paint_type {
   SWAPPY_PAINT_MODE_ELLIPSE,   /* Ellipse shapes */
   SWAPPY_PAINT_MODE_ARROW,     /* Arrow shapes */
   SWAPPY_PAINT_MODE_BLUR,      /* Blur mode */
+  SWAPPY_PAINT_MODE_CROP,      /* Crop mode */
 };
 
 enum swappy_paint_shape_operation {
@@ -33,6 +34,13 @@ enum swappy_text_mode {
   SWAPPY_TEXT_MODE_DONE,
 };
 
+enum swappy_resize {
+  SWAPPY_RESIZE_NONE =   0, /* No resize along the axis. */
+  SWAPPY_RESIZE_LOW  =  -1, /* Changing the lower bound on the axis. */
+  SWAPPY_RESIZE_HIGH =  +1, /* Changing the higher bound on the axis. */
+  SWAPPY_RESIZE_BOTH = 127, /* Moving both bounds on the axis. */
+};
+
 struct swappy_point {
   gdouble x;
   gdouble y;
@@ -113,6 +121,7 @@ struct swappy_state_ui {
 
   GtkWindow *window;
   GtkWidget *area;
+  GtkWidget *visual_area;
 
   GtkToggleButton *panel_toggle_button;
 
@@ -128,6 +137,7 @@ struct swappy_state_ui {
   GtkRadioButton *ellipse;
   GtkRadioButton *arrow;
   GtkRadioButton *blur;
+  GtkRadioButton *crop;
 
   GtkRadioButton *red;
   GtkRadioButton *green;
@@ -152,6 +162,18 @@ struct swappy_config {
   guint32 text_size;
   char *text_font;
   gboolean early_exit;
+  gboolean auto_save;
+  char *custom_color;
+};
+
+struct swappy_crop {
+  uint32_t left_x;
+  uint32_t top_y;
+  uint32_t right_x;
+  uint32_t bottom_y;
+
+  enum swappy_resize resize_x;
+  enum swappy_resize resize_y;
 };
 
 struct swappy_state {
@@ -163,6 +185,13 @@ struct swappy_state {
   GdkPixbuf *original_image;
   cairo_surface_t *original_image_surface;
   cairo_surface_t *rendering_surface;
+  cairo_surface_t *visual_surface;
+
+  double last_mouse_x;
+  double last_mouse_y;
+
+  struct swappy_crop crop;
+  gboolean crop_ever_changed;
 
   gdouble scaling_factor;
 
diff --git a/res/style/swappy.css b/res/style/swappy.css
index c705661..66013b3 100644
--- a/res/style/swappy.css
+++ b/res/style/swappy.css
@@ -1,5 +1,5 @@
 .drawing .text-button {
-  font-family: "FontAwesome 5 Free Solid";
+  font-family: "FontAwesome 5 Free Solid", "FontAwesome";
   padding: 4px;
 }
 
diff --git a/res/swappy.glade b/res/swappy.glade
index 259d617..974838b 100644
--- a/res/swappy.glade
+++ b/res/swappy.glade
@@ -161,6 +161,18 @@
                         <property name="position">5</property>
                       </packing>
                     </child>
+                    <child>
+                      <object class="GtkLabel">
+                        <property name="visible">True</property>
+                        <property name="can_focus">False</property>
+                        <property name="label" translatable="no">C</property>
+                      </object>
+                      <packing>
+                        <property name="expand">False</property>
+                        <property name="fill">True</property>
+                        <property name="position">6</property>
+                      </packing>
+                    </child>
                   </object>
                   <packing>
                     <property name="expand">False</property>
@@ -271,6 +283,22 @@
                         <property name="position">5</property>
                       </packing>
                     </child>
+                    <child>
+                      <object class="GtkRadioButton" id="crop">
+                        <property name="label" translatable="no"></property>
+                        <property name="visible">True</property>
+                        <property name="can_focus">False</property>
+                        <property name="receives_default">False</property>
+                        <property name="draw_indicator">False</property>
+                        <property name="group">brush</property>
+                        <signal name="clicked" handler="crop_clicked_handler" swapped="no"/>
+                      </object>
+                      <packing>
+                        <property name="expand">False</property>
+                        <property name="fill">True</property>
+                        <property name="position">6</property>
+                      </packing>
+                    </child>
                     <style>
                       <class name="drawing"/>
                     </style>
@@ -639,6 +667,17 @@
                     <signal name="motion-notify-event" handler="draw_area_motion_notify_handler" swapped="no"/>
                   </object>
                 </child>
+                <child>
+                  <object class="GtkDrawingArea" id="visual-area">
+                    <property name="visible">True</property>
+                    <property name="can_focus">False</property>
+                    <property name="margin_left">10</property>
+                    <property name="margin_right">10</property>
+                    <property name="margin_top">10</property>
+                    <property name="margin_bottom">10</property>
+                    <signal name="draw" handler="crop_area_handler" swapped="no"/>
+                  </object>
+                </child>
               </object>
               <packing>
                 <property name="resize">True</property>
diff --git a/script/github-release b/script/github-release
index 452766f..51fc03d 100755
--- a/script/github-release
+++ b/script/github-release
@@ -16,7 +16,7 @@ die() {
 init() {
   command -v git >/dev/null 2>&1 || { echo >&2 "git required: pacman -S git"; exit 1; }
   command -v gh >/dev/null 2>&1 || { echo >&2 "github cli tool required to publish the release: pacman -S github-cli"; exit 1; }
-  command -v npx >/dev/null 2>&1 || { echo >&2 "npx required for standard versionning the release: pacman -S npm"; exit 1; }
+  command -v npx >/dev/null 2>&1 || { echo >&2 "npx required for standard versioning the release: pacman -S npm"; exit 1; }
   command -v gpg >/dev/null 2>&1 || { echo >&2 "gpg required to sign the archive: pacman -S gnupg"; exit 1; }
 
   mkdir -p $release_folder
@@ -73,16 +73,16 @@ gpg_sign_archive() {
 
 git_generate_changelog() {
   echo "generating changelog..."
-  git diff "v$version"^ -- CHANGELOG.md | tail -n +9 | head -n -4 | sed 's/^+//g' > $release_folder/CHANGELOG.md
+  git diff "v$version"^ -- CHANGELOG.md | tail -n +9 | head -n -4 | sed 's/^+//g' > $release_folder/CHANGELOG-$version.md
 }
 
 github_create_release() {
   echo "creating github release..."
   gh release create --draft "v$version" \
-    -F "$release_folder/CHANGELOG.md" \
+    -F "$release_folder/CHANGELOG-$version.md" \
     "$release_folder/$app_name-$version.tar.gz" \
     "$release_folder/$app_name-$version.tar.gz.sig" \
-    "$release_folder/CHANGELOG.md"
+    "$release_folder/CHANGELOG-$version.md"
 }
 
 main() {
diff --git a/src/application.c b/src/application.c
index 57d652f..71f9baf 100644
--- a/src/application.c
+++ b/src/application.c
@@ -56,6 +56,7 @@ void application_finish(struct swappy_state *state) {
   g_debug("application finishing, cleaning up");
   paint_free_all(state);
   pixbuf_free(state);
+  cairo_surface_destroy(state->visual_surface);
   cairo_surface_destroy(state->rendering_surface);
   cairo_surface_destroy(state->original_image_surface);
   if (state->temp_file_str) {
@@ -161,6 +162,11 @@ static void switch_mode_to_blur(struct swappy_state *state) {
   gtk_widget_set_sensitive(GTK_WIDGET(state->ui->fill_shape), false);
 }
 
+static void switch_mode_to_crop(struct swappy_state *state) {
+  state->mode = SWAPPY_PAINT_MODE_CROP;
+  gtk_widget_set_sensitive(GTK_WIDGET(state->ui->fill_shape), false);
+}
+
 static void action_stroke_size_decrease(struct swappy_state *state) {
   guint step = state->settings.w <= 10 ? 1 : 5;
 
@@ -246,7 +252,7 @@ static void save_state_to_file_or_folder(struct swappy_state *state,
 }
 
 static void maybe_save_output_file(struct swappy_state *state) {
-  if (state->output_file != NULL) {
+  if (state->config->auto_save) {
     save_state_to_file_or_folder(state, state->output_file);
   }
 }
@@ -306,6 +312,10 @@ void blur_clicked_handler(GtkWidget *widget, struct swappy_state *state) {
   switch_mode_to_blur(state);
 }
 
+void crop_clicked_handler(GtkWidget *widget, struct swappy_state *state) {
+  switch_mode_to_crop(state);
+}
+
 void save_clicked_handler(GtkWidget *widget, struct swappy_state *state) {
   // Commit a potential paint (e.g. text being written)
   commit_state(state);
@@ -402,6 +412,10 @@ void window_keypress_handler(GtkWidget *widget, GdkEventKey *event,
         switch_mode_to_blur(state);
         gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(state->ui->blur), true);
         break;
+      case GDK_KEY_c:
+        switch_mode_to_crop(state);
+        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(state->ui->crop), true);
+        break;
       case GDK_KEY_k:
         action_clear(state);
         break;
@@ -481,8 +495,9 @@ void redo_clicked_handler(GtkWidget *widget, struct swappy_state *state) {
   action_redo(state);
 }
 
-gboolean draw_area_handler(GtkWidget *widget, cairo_t *cr,
-                           struct swappy_state *state) {
+static
+void area_handler(GtkWidget *widget, cairo_t *cr, cairo_surface_t *surface,
+                      struct swappy_state *state) {
   GtkAllocation *alloc = g_new(GtkAllocation, 1);
   gtk_widget_get_allocation(widget, alloc);
 
@@ -493,9 +508,19 @@ gboolean draw_area_handler(GtkWidget *widget, cairo_t *cr,
   double scale_y = (double)alloc->height / image_height;
 
   cairo_scale(cr, scale_x, scale_y);
-  cairo_set_source_surface(cr, state->rendering_surface, 0, 0);
+  cairo_set_source_surface(cr, surface, 0, 0);
   cairo_paint(cr);
+}
+
+gboolean draw_area_handler(GtkWidget *widget, cairo_t *cr,
+                           struct swappy_state *state) {
+  area_handler(widget, cr, state->rendering_surface, state);
+  return FALSE;
+}
 
+gboolean crop_area_handler(GtkWidget *widget, cairo_t *cr,
+                           struct swappy_state *state) {
+  area_handler(widget, cr, state->visual_surface, state);
   return FALSE;
 }
 
@@ -511,12 +536,20 @@ gboolean draw_area_configure_handler(GtkWidget *widget,
   return TRUE;
 }
 
+static
+bool should_crop_recreate(const struct swappy_state *state,
+                          bool is_control_pressed) {
+  return is_control_pressed || !state->crop_ever_changed;
+}
+
 void draw_area_button_press_handler(GtkWidget *widget, GdkEventButton *event,
                                     struct swappy_state *state) {
   gdouble x, y;
 
   screen_coordinates_to_image_coordinates(state, event->x, event->y, &x, &y);
 
+  gboolean is_control_pressed = event->state & GDK_CONTROL_MASK;
+
   if (event->button == 1) {
     switch (state->mode) {
       case SWAPPY_PAINT_MODE_BLUR:
@@ -529,21 +562,100 @@ void draw_area_button_press_handler(GtkWidget *widget, GdkEventButton *event,
         render_state(state);
         update_ui_undo_redo(state);
         break;
+      case SWAPPY_PAINT_MODE_CROP: {
+        gboolean recreate = should_crop_recreate(state, is_control_pressed);
+        paint_start_crop(state, x, y, recreate);
+        state->crop_ever_changed = true;
+        render_state(state);
+        break;
+      }
       default:
         return;
     }
   }
 }
+
+static
+void set_cursor(GdkWindow *window, GdkCursorType cursor_type) {
+  GdkDisplay *display = gdk_display_get_default();
+  GdkCursor *cursor = gdk_cursor_new_for_display(display, cursor_type);
+  gdk_window_set_cursor(window, cursor);
+  g_object_unref(cursor);
+}
+
+static
+GdkCursorType get_crop_cursor_type(struct swappy_state *state,
+                                   gdouble x, gdouble y,
+                                   gboolean recreate,
+                                   gboolean currently_resizing) {
+  enum swappy_resize resize_x, resize_y;
+
+  if (currently_resizing) {
+    resize_x = state->crop.resize_x;
+    resize_y = state->crop.resize_y;
+  } else {
+    if (recreate)
+      return GDK_CROSSHAIR;
+
+    paint_get_crop_resize(&resize_x, &resize_y, state, x, y);
+    if (!resize_x && !resize_y)
+      return GDK_CROSSHAIR;
+  }
+
+  switch (resize_x) {
+    case SWAPPY_RESIZE_NONE:
+      switch (resize_y) {
+        case SWAPPY_RESIZE_NONE:
+          return GDK_ARROW;
+        case SWAPPY_RESIZE_LOW:
+          return GDK_TOP_SIDE;
+        case SWAPPY_RESIZE_HIGH:
+          return GDK_BOTTOM_SIDE;
+        case SWAPPY_RESIZE_BOTH:
+          return GDK_FLEUR;
+      }
+      break;
+
+    case SWAPPY_RESIZE_LOW:
+      switch (resize_y) {
+        case SWAPPY_RESIZE_NONE:
+          return GDK_LEFT_SIDE;
+        case SWAPPY_RESIZE_LOW:
+          return GDK_TOP_LEFT_CORNER;
+        case SWAPPY_RESIZE_HIGH:
+          return GDK_BOTTOM_LEFT_CORNER;
+        case SWAPPY_RESIZE_BOTH:
+          return GDK_FLEUR;
+      }
+      break;
+
+    case SWAPPY_RESIZE_HIGH:
+      switch (resize_y) {
+        case SWAPPY_RESIZE_NONE:
+          return GDK_RIGHT_SIDE;
+        case SWAPPY_RESIZE_LOW:
+          return GDK_TOP_RIGHT_CORNER;
+        case SWAPPY_RESIZE_HIGH:
+          return GDK_BOTTOM_RIGHT_CORNER;
+        case SWAPPY_RESIZE_BOTH:
+          return GDK_FLEUR;
+      }
+      break;
+
+    case SWAPPY_RESIZE_BOTH:
+      return GDK_FLEUR;
+  }
+
+  return GDK_ARROW;
+}
+
 void draw_area_motion_notify_handler(GtkWidget *widget, GdkEventMotion *event,
                                      struct swappy_state *state) {
   gdouble x, y;
 
   screen_coordinates_to_image_coordinates(state, event->x, event->y, &x, &y);
 
-  GdkDisplay *display = gdk_display_get_default();
-  GdkWindow *window = event->window;
-  GdkCursor *crosshair = gdk_cursor_new_for_display(display, GDK_CROSSHAIR);
-  gdk_window_set_cursor(window, crosshair);
+  GdkCursorType cursor_type = GDK_CROSSHAIR;
 
   gboolean is_button1_pressed = event->state & GDK_BUTTON1_MASK;
   gboolean is_control_pressed = event->state & GDK_CONTROL_MASK;
@@ -565,10 +677,22 @@ void draw_area_motion_notify_handler(GtkWidget *widget, GdkEventMotion *event,
         render_state(state);
       }
       break;
+    case SWAPPY_PAINT_MODE_CROP: {
+      gboolean recreate = should_crop_recreate(state, is_control_pressed);
+      cursor_type = get_crop_cursor_type(state, x, y, recreate, is_button1_pressed);
+      if (is_button1_pressed) {
+        paint_update_crop(state, x - state->last_mouse_x, y - state->last_mouse_y);
+        render_state(state);
+      }
+      break;
+    }
     default:
-      return;
+      break;
   }
-  g_object_unref(crosshair);
+
+  set_cursor(event->window, cursor_type);
+  state->last_mouse_x = x;
+  state->last_mouse_y = y;
 }
 void draw_area_button_release_handler(GtkWidget *widget, GdkEventButton *event,
                                       struct swappy_state *state) {
@@ -721,6 +845,8 @@ static bool load_css(struct swappy_state *state) {
 
 static bool load_layout(struct swappy_state *state) {
   GError *error = NULL;
+  // init color
+  GdkRGBA color;
 
   /* Construct a GtkBuilder instance and load our UI description */
   GtkBuilder *builder = gtk_builder_new();
@@ -750,6 +876,8 @@ static bool load_layout(struct swappy_state *state) {
 
   GtkWidget *area =
       GTK_WIDGET(gtk_builder_get_object(builder, "painting-area"));
+  GtkWidget *visual_area = 
+      GTK_WIDGET(gtk_builder_get_object(builder, "visual-area"));
 
   state->ui->painting_box =
       GTK_BOX(gtk_builder_get_object(builder, "painting-box"));
@@ -765,6 +893,8 @@ static bool load_layout(struct swappy_state *state) {
       GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "arrow"));
   GtkRadioButton *blur =
       GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "blur"));
+  GtkRadioButton *crop =
+      GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "crop"));
 
   state->ui->red =
       GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "color-red-button"));
@@ -785,20 +915,37 @@ static bool load_layout(struct swappy_state *state) {
   state->ui->fill_shape = GTK_TOGGLE_BUTTON(
       gtk_builder_get_object(builder, "fill-shape-toggle-button"));
 
+  gdk_rgba_parse(&color, state->config->custom_color);
+  gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(state->ui->color), &color);
+
   state->ui->brush = brush;
   state->ui->text = text;
   state->ui->rectangle = rectangle;
   state->ui->ellipse = ellipse;
   state->ui->arrow = arrow;
   state->ui->blur = blur;
+  state->ui->crop = crop;
   state->ui->area = area;
+  state->ui->visual_area = visual_area;
   state->ui->window = window;
 
   compute_window_size_and_scaling_factor(state);
   gtk_widget_set_size_request(area, state->window->width,
                               state->window->height);
+  gtk_widget_set_size_request(visual_area, state->window->width,
+                              state->window->height);
   action_toggle_painting_panel(state, &state->config->show_panel);
 
+  // The `visual_area` is laid over `area` for visualizations that do not go
+  // into the resulting image. Mouse controls are handled by `area`, so we want
+  // to hereby make `visual_area` "click-through".
+  GdkWindow *w = gtk_widget_get_window(visual_area);
+  if (w) {
+    cairo_region_t *empty = cairo_region_create();
+    gdk_window_input_shape_combine_region(w, empty, 0, 0);
+    cairo_region_destroy(empty);
+  }
+
   g_object_unref(G_OBJECT(builder));
 
   return true;
@@ -831,6 +978,10 @@ static void set_paint_mode(struct swappy_state *state) {
       gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(state->ui->blur), true);
       gtk_widget_set_sensitive(GTK_WIDGET(state->ui->fill_shape), false);
       break;
+    case SWAPPY_PAINT_MODE_CROP:
+      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(state->ui->crop), true);
+      gtk_widget_set_sensitive(GTK_WIDGET(state->ui->fill_shape), false);
+      break;
     default:
       break;
   }
diff --git a/src/config.c b/src/config.c
index 732d34e..8de47aa 100644
--- a/src/config.c
+++ b/src/config.c
@@ -22,6 +22,8 @@ static void print_config(struct swappy_config *config) {
   g_info("paint_mode: %d", config->paint_mode);
   g_info("early_exit: %d", config->early_exit);
   g_info("fill_shape: %d", config->fill_shape);
+  g_info("auto_save: %d", config->auto_save);
+  g_info("custom_color: %s", config->custom_color);
 }
 
 static char *get_default_save_dir() {
@@ -81,6 +83,8 @@ static void load_config_from_file(struct swappy_config *config,
   gchar *paint_mode = NULL;
   gboolean early_exit;
   gboolean fill_shape;
+  gboolean auto_save;
+  gchar *custom_color = NULL;
   GError *error = NULL;
 
   if (file == NULL) {
@@ -211,6 +215,8 @@ static void load_config_from_file(struct swappy_config *config,
       config->paint_mode = SWAPPY_PAINT_MODE_ARROW;
     } else if (g_ascii_strcasecmp(paint_mode, "blur") == 0) {
       config->paint_mode = SWAPPY_PAINT_MODE_BLUR;
+    } else if (g_ascii_strcasecmp(paint_mode, "crop") == 0) {
+      config->paint_mode = SWAPPY_PAINT_MODE_CROP;
     } else {
       g_warning(
           "paint_mode is not a valid value: %s - see man page for details",
@@ -232,6 +238,26 @@ static void load_config_from_file(struct swappy_config *config,
     error = NULL;
   }
 
+  auto_save = g_key_file_get_boolean(gkf, group, "auto_save", &error);
+
+  if (error == NULL) {
+    config->auto_save = auto_save;
+  } else {
+    g_info("auto_save is missing in %s (%s)", file, error->message);
+    g_error_free(error);
+    error = NULL;
+  }
+
+  custom_color = g_key_file_get_string(gkf, group, "custom_color", &error);
+
+  if (error == NULL) {
+    config->custom_color = custom_color;
+  } else {
+    g_info("custom_color is missing in %s (%s)", file, error->message);
+    g_error_free(error);
+    error = NULL;
+  }
+
   g_key_file_free(gkf);
 }
 
@@ -249,6 +275,8 @@ static void load_default_config(struct swappy_config *config) {
   config->paint_mode = CONFIG_PAINT_MODE_DEFAULT;
   config->early_exit = CONFIG_EARLY_EXIT_DEFAULT;
   config->fill_shape = CONFIG_FILL_SHAPE_DEFAULT;
+  config->auto_save = CONFIG_AUTO_SAVE_DEFAULT;
+  config->custom_color = g_strdup(CONFIG_CUSTOM_COLOR_DEFAULT);
 }
 
 void config_load(struct swappy_state *state) {
@@ -275,6 +303,7 @@ void config_free(struct swappy_state *state) {
     g_free(state->config->save_dir);
     g_free(state->config->save_filename_format);
     g_free(state->config->text_font);
+    g_free(state->config->custom_color);
     g_free(state->config);
     state->config = NULL;
   }
diff --git a/src/paint.c b/src/paint.c
index 142297a..29c54a6 100644
--- a/src/paint.c
+++ b/src/paint.c
@@ -291,3 +291,135 @@ void paint_commit_temporary(struct swappy_state *state) {
   // because it's now part of the GList.
   state->temp_paint = NULL;
 }
+
+void paint_get_crop_resize(enum swappy_resize *out_resize_x,
+                           enum swappy_resize *out_resize_y,
+                           const struct swappy_state *state,
+                           double x, double y) {
+  const struct swappy_crop *crop = &state->crop;
+  const double part_size = 30 / state->scaling_factor;
+
+  if (x < crop->left_x - part_size || x > crop->right_x + part_size ||
+      y < crop->top_y - part_size || y > crop->bottom_y + part_size)
+  {
+      *out_resize_x = SWAPPY_RESIZE_NONE;
+      *out_resize_y = SWAPPY_RESIZE_NONE;
+      return;
+  }
+
+  if (x < crop->left_x + part_size)
+    *out_resize_x = SWAPPY_RESIZE_LOW;
+  else if (x > crop->right_x - part_size)
+    *out_resize_x = SWAPPY_RESIZE_HIGH;
+  else if (x >= crop->left_x && x <= crop->right_x)
+    *out_resize_x = SWAPPY_RESIZE_BOTH;
+  else
+    *out_resize_x = SWAPPY_RESIZE_NONE;
+
+  if (y < crop->top_y + part_size)
+    *out_resize_y = SWAPPY_RESIZE_LOW;
+  else if (y > crop->bottom_y - part_size)
+    *out_resize_y = SWAPPY_RESIZE_HIGH;
+  else if (y >= crop->top_y && y <= crop->bottom_y)
+    *out_resize_y = SWAPPY_RESIZE_BOTH;
+  else
+    *out_resize_y = SWAPPY_RESIZE_NONE;
+
+  if (*out_resize_x == SWAPPY_RESIZE_BOTH && *out_resize_y != SWAPPY_RESIZE_BOTH)
+    *out_resize_x = SWAPPY_RESIZE_NONE;
+  if (*out_resize_y == SWAPPY_RESIZE_BOTH && *out_resize_x != SWAPPY_RESIZE_BOTH)
+    *out_resize_y = SWAPPY_RESIZE_NONE;
+}
+
+void paint_start_crop(struct swappy_state *state, double x, double y,
+                      gboolean recreate_requested) {
+  if (!recreate_requested) {
+    paint_get_crop_resize(&state->crop.resize_x, &state->crop.resize_y,
+                          state, x, y);
+
+    if (state->crop.resize_x || state->crop.resize_y)
+      return;
+  }
+
+  state->crop.left_x = x;
+  state->crop.right_x = x;
+  state->crop.top_y = y;
+  state->crop.bottom_y = y;
+
+  state->crop.resize_x = SWAPPY_RESIZE_HIGH;
+  state->crop.resize_y = SWAPPY_RESIZE_HIGH;
+}
+
+static inline
+bool u32_add_clamped(uint32_t *val, double to_add, uint32_t max) {
+  if (*val + to_add > max) {
+    *val = max;
+    return true;
+  } else if (to_add < 0 && *val < -to_add) {
+    *val = 0;
+    return true;
+  } else {
+    *val += to_add;
+    return false;
+  }
+}
+
+void paint_update_crop(struct swappy_state *state,
+                       double delta_x, double delta_y) {
+  struct swappy_crop *crop = &state->crop;
+  double iw = cairo_image_surface_get_width(state->rendering_surface);
+  double ih = cairo_image_surface_get_height(state->rendering_surface);
+
+  switch (crop->resize_x) {
+    case SWAPPY_RESIZE_NONE:
+      break;
+    case SWAPPY_RESIZE_LOW:
+      u32_add_clamped(&crop->left_x, delta_x, iw);
+      break;
+    case SWAPPY_RESIZE_HIGH:
+      u32_add_clamped(&crop->right_x, delta_x, iw);
+      break;
+    case SWAPPY_RESIZE_BOTH: {
+      uint32_t width = crop->right_x - crop->left_x;
+      if (u32_add_clamped(&crop->left_x, delta_x, iw)) {
+        crop->right_x = crop->left_x + width;
+      } else if (u32_add_clamped(&crop->right_x, delta_x, iw)) {
+        crop->left_x = crop->right_x - width;
+      }
+      break;
+    }
+  }
+  switch (crop->resize_y) {
+    case SWAPPY_RESIZE_NONE:
+      break;
+    case SWAPPY_RESIZE_LOW:
+      u32_add_clamped(&crop->top_y, delta_y, ih);
+      break;
+    case SWAPPY_RESIZE_HIGH:
+      u32_add_clamped(&crop->bottom_y, delta_y, ih);
+      break;
+    case SWAPPY_RESIZE_BOTH: {
+      uint32_t height = crop->bottom_y - crop->top_y;
+      if (u32_add_clamped(&crop->top_y, delta_y, ih)) {
+        crop->bottom_y = crop->top_y + height;
+      } else if (u32_add_clamped(&crop->bottom_y, delta_y, ih)) {
+        crop->top_y = crop->bottom_y - height;
+      }
+      break;
+    }
+  }
+
+  uint32_t k;
+  if (crop->left_x > crop->right_x) {
+    k = crop->left_x;
+    crop->left_x = crop->right_x;
+    crop->right_x = k;
+    crop->resize_x = -crop->resize_x;
+  }
+  if (crop->top_y > crop->bottom_y) {
+    k = crop->top_y;
+    crop->top_y = crop->bottom_y;
+    crop->bottom_y = k;
+    crop->resize_y = -crop->resize_y;
+  }
+}
diff --git a/src/pixbuf.c b/src/pixbuf.c
index 8a63671..41cb43a 100644
--- a/src/pixbuf.c
+++ b/src/pixbuf.c
@@ -4,10 +4,12 @@
 #include <gio/gunixoutputstream.h>
 
 GdkPixbuf *pixbuf_get_from_state(struct swappy_state *state) {
-  guint width = cairo_image_surface_get_width(state->rendering_surface);
-  guint height = cairo_image_surface_get_height(state->rendering_surface);
-  GdkPixbuf *pixbuf = gdk_pixbuf_get_from_surface(state->rendering_surface, 0,
-                                                  0, width, height);
+  guint width = state->crop.right_x - state->crop.left_x;
+  guint height = state->crop.bottom_y - state->crop.top_y;
+  GdkPixbuf *pixbuf = gdk_pixbuf_get_from_surface(
+    state->rendering_surface,
+    state->crop.left_x, state->crop.top_y,
+    width, height);
 
   return pixbuf;
 }
@@ -120,6 +122,14 @@ void pixbuf_scale_surface_from_widget(struct swappy_state *state,
     goto finish;
   }
 
+  cairo_surface_t *visual_surface =
+      cairo_image_surface_create(CAIRO_FORMAT_ARGB32, image_width, image_height);
+
+  if (!visual_surface) {
+    g_error("unable to create visual surface");
+    goto finish;
+  }
+
   g_info("size of area to render: %ux%u", alloc->width, alloc->height);
 
 finish:
@@ -135,6 +145,19 @@ finish:
   }
   state->rendering_surface = rendering_surface;
 
+  if (state->visual_surface) {
+    cairo_surface_destroy(state->visual_surface);
+    state->visual_surface = NULL;
+  }
+  state->visual_surface = visual_surface;
+
+  state->crop = (struct swappy_crop){
+    .left_x = 0,
+    .top_y = 0,
+    .right_x = image_width,
+    .bottom_y = image_height,
+  };
+
   g_free(alloc);
 }
 
diff --git a/src/po/LINGUAS b/src/po/LINGUAS
index c153839..235c1d8 100644
--- a/src/po/LINGUAS
+++ b/src/po/LINGUAS
@@ -4,3 +4,4 @@ fr
 en
 pt_BR
 tr
+zh_CN
diff --git a/src/po/tr.po b/src/po/tr.po
index d9b2f7d..bcc74a1 100644
--- a/src/po/tr.po
+++ b/src/po/tr.po
@@ -1,16 +1,16 @@
 # Turkish translations for swappy package.
 # Copyright (C) 2020 THE swappy'S COPYRIGHT HOLDER
 # This file is distributed under the same license as the swappy package.
-# Oğuz Ersen <oguzersen@protonmail.com>, 2020.
+# Oğuz Ersen <oguz@ersen.moe>, 2020-2022.
 #
 msgid ""
 msgstr ""
 "Project-Id-Version: swappy\n"
 "Report-Msgid-Bugs-To: \n"
 "POT-Creation-Date: 2022-11-18 16:07-0500\n"
-"PO-Revision-Date: 2020-10-16 22:00+0300\n"
-"Last-Translator: Oğuz Ersen <oguzersen@protonmail.com>\n"
-"Language-Team: none\n"
+"PO-Revision-Date: 2022-11-25 10:36+0300\n"
+"Last-Translator: Oğuz Ersen <oguz@ersen.moe>\n"
+"Language-Team: Turkish <tr>\n"
 "Language: tr\n"
 "MIME-Version: 1.0\n"
 "Content-Type: text/plain; charset=UTF-8\n"
@@ -27,11 +27,11 @@ msgstr "Metin Boyutu"
 
 #: res/swappy.glade:592
 msgid "Fill shape"
-msgstr ""
+msgstr "Şekli Doldur"
 
 #: res/swappy.glade:597
 msgid "Toggle shape filling"
-msgstr ""
+msgstr "Şekil Doldurmayı Aç/Kapat"
 
 #: res/swappy.glade:671
 msgid "Toggle Paint Panel"
diff --git a/src/po/zh_CN.po b/src/po/zh_CN.po
new file mode 100644
index 0000000..03f8865
--- /dev/null
+++ b/src/po/zh_CN.po
@@ -0,0 +1,59 @@
+
+# English translations for swappy package.
+# Copyright (C) 2020 THE swappy'S COPYRIGHT HOLDER
+# This file is distributed under the same license as the swappy package.
+# Automatically generated, 2020.
+#
+msgid ""
+msgstr ""
+"Project-Id-Version: swappy\n"
+"Report-Msgid-Bugs-To: \n"
+"POT-Creation-Date: 2022-11-18 16:07-0500\n"
+"PO-Revision-Date: 2020-06-21 21:57-0400\n"
+"Last-Translator: Automatically generated\n"
+"Language-Team: none\n"
+"Language: zh_CN\n"
+"MIME-Version: 1.0\n"
+"Content-Type: text/plain; charset=UTF-8\n"
+"Content-Transfer-Encoding: 8bit\n"
+"Plural-Forms: nplurals=1; plural=0;\n"
+
+#: res/swappy.glade:456
+msgid "Line Width"
+msgstr "行宽"
+
+#: res/swappy.glade:526
+msgid "Text Size"
+msgstr "文本大小"
+
+#: res/swappy.glade:592
+msgid "Fill shape"
+msgstr "填充"
+
+#: res/swappy.glade:597
+msgid "Toggle shape filling"
+msgstr "切换填充状态"
+
+#: res/swappy.glade:671
+msgid "Toggle Paint Panel"
+msgstr "切换绘图板状态"
+
+#: res/swappy.glade:697
+msgid "Undo Last Paint"
+msgstr "撤销"
+
+#: res/swappy.glade:716
+msgid "Redo Previous Paint"
+msgstr "恢复"
+
+#: res/swappy.glade:735
+msgid "Clear Paints"
+msgstr "清除绘图"
+
+#: res/swappy.glade:763
+msgid "Copy Surface"
+msgstr "复制"
+
+#: res/swappy.glade:779
+msgid "Save Surface"
+msgstr "保存"
diff --git a/src/render.c b/src/render.c
index 5015856..705fdb4 100644
--- a/src/render.c
+++ b/src/render.c
@@ -384,6 +384,13 @@ static void render_background(cairo_t *cr, struct swappy_state *state) {
   cairo_paint(cr);
 }
 
+static void render_clear(cairo_t *cr) {
+  cairo_save(cr);
+  cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
+  cairo_paint(cr);
+  cairo_restore(cr);
+}
+
 static void render_blur(cairo_t *cr, struct swappy_paint *paint) {
   struct swappy_paint_blur blur = paint->content.blur;
 
@@ -506,16 +513,58 @@ static void render_paints(cairo_t *cr, struct swappy_state *state) {
   }
 }
 
+static void render_crop(cairo_t *cr, struct swappy_state *state) {
+  double x = state->crop.left_x;
+  double y = state->crop.top_y;
+  double w = state->crop.right_x - state->crop.left_x;
+  double h = state->crop.bottom_y - state->crop.top_y;
+
+  double iw = cairo_image_surface_get_width(state->rendering_surface);
+  double ih = cairo_image_surface_get_height(state->rendering_surface);
+
+  cairo_save(cr);
+
+  // Darkening overlay
+  cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, 0.5);
+  cairo_rectangle(cr, 0, 0, x, ih);
+  cairo_rectangle(cr, state->crop.right_x, 0, iw - state->crop.right_x, ih);
+  cairo_rectangle(cr, state->crop.left_x, 0, w, state->crop.top_y);
+  cairo_rectangle(cr, state->crop.left_x, state->crop.bottom_y, w, ih - state->crop.bottom_y);
+  cairo_close_path(cr);
+  cairo_fill(cr);
+
+  cairo_restore(cr);
+  cairo_save(cr);
+
+  // Crop border
+  cairo_set_source_rgba(cr, 1, 1, 1, 1);
+  cairo_set_line_width(cr, 3);
+  cairo_rectangle(cr, x, y, w, h);
+  cairo_close_path(cr);
+  cairo_stroke(cr);
+
+  cairo_restore(cr);
+}
+
 void render_state(struct swappy_state *state) {
-  cairo_surface_t *surface = state->rendering_surface;
-  cairo_t *cr = cairo_create(surface);
+  // Render what goes into the actual image
+  cairo_t *rendering_cr = cairo_create(state->rendering_surface);
 
-  render_background(cr, state);
-  render_image(cr, state);
-  render_paints(cr, state);
+  render_background(rendering_cr, state);
+  render_image(rendering_cr, state);
+  render_paints(rendering_cr, state);
 
-  cairo_destroy(cr);
+  cairo_destroy(rendering_cr);
+
+  // Render visual guides
+  cairo_t *visual_cr = cairo_create(state->visual_surface);
+
+  render_clear(visual_cr);
+  render_crop(visual_cr, state);
+
+  cairo_destroy(visual_cr);
 
   // Drawing is finished, notify the GtkDrawingArea it needs to be redrawn.
   gtk_widget_queue_draw(state->ui->area);
+  gtk_widget_queue_draw(state->ui->visual_area);
 }
diff --git a/swappy.1.scd b/swappy.1.scd
index 4fb6c0a..69cfd55 100644
--- a/swappy.1.scd
+++ b/swappy.1.scd
@@ -74,9 +74,12 @@ The following lines can be used as swappy's default:
 - *line_size* is the default line size (must be between 1 and 50)
 - *text_size* is the default text size (must be between 10 and 50)
 - *text_font* is the font used to render text, its format is pango friendly
-- *paint_mode* is the mode activated at application start (must be one of: brush|text|rectangle|ellipse|arrow|blur, matching is case-insensitive)
+- *paint_mode* is the mode activated at application start (must be one of: brush|text|rectangle|ellipse|arrow|blur|crop, matching is case-insensitive)
 - *early_exit* is used to make the application exit after saving the picture or copying it to the clipboard
 - *fill_shape* is used to toggle shape filling (for the rectangle and ellipsis tools) on or off upon startup
+- *auto_save* is used to toggle auto saving of final buffer to *save_dir* upon exit
+- *custom_color* is used to set a default value for the custom color. Accepted
+formats are: standard name (one of: https://github.com/rgb-x/system/blob/master/root/etc/X11/rgb.txt),  #rgb, #rrggbb, #rrrgggbbb, #rrrrggggbbbb, rgb(r,b,g), rgba(r,g,b,a)
 
 
 # KEY BINDINGS
@@ -93,6 +96,7 @@ The following lines can be used as swappy's default:
 - *o*: Switch to Ellipse
 - *a*: Switch to Arrow
 - *d*: Switch to Blur (d stands for droplet)
+- *c*: Switch to Crop
 
 - *R*: Use Red Color
 - *G*: Use Green Color
@@ -106,7 +110,9 @@ The following lines can be used as swappy's default:
 
 ## MODIFIERS
 
-- *Ctrl*: Center Shape (Rectangle & Ellipse) based on draw start
+- *Ctrl*:
+	- Rectangle & Ellipse: Center Shape based on draw start
+	- Crop: Draw a new crop rectangle instead of changing the existing one
 
 ## HEADER BAR

```

</details>

And finally, I add a basic rotate feature from
[pull 218 ⇗](https://github.com/jtheoof/swappy/pull/218/commits/c162862e7b94005433afc7573a43098412381f44).

<details>
<summary>Click to enlarge. It's a bit long.</summary>

```diff path=/etc/portage/patches/gui-apps/swappy/03-rotate.patch
From c162862e7b94005433afc7573a43098412381f44 Mon Sep 17 00:00:00 2001
From: Rohith Vinod <idliyout@gmail.com>
Date: Wed, 7 Jan 2026 23:06:46 +0530
Subject: [PATCH] feat(ui): very basic rotation support

* add support for rotating the image (left or right)
    - however, since this is a very basic implementation, it rasterizes
      the painted annotations like rectangles, ellipses, blur etc. onto
      the image and then rotates it.
    - the paint vector is cleared and the undo and redo buttons are greyed
      out.
* add two buttons and setup keybinds `Ctrl+Left` and `Ctrl+Right` for
  rotating images left and right respectively.
---
 include/application.h |   3 +
 include/swappy.h      |   4 +
 res/swappy.glade      |  53 ++++++++++++-
 src/application.c     | 169 ++++++++++++++++++++++++++++--------------
 4 files changed, 174 insertions(+), 55 deletions(-)

diff --git a/include/application.h b/include/application.h
index 112c050..fd48669 100644
--- a/include/application.h
+++ b/include/application.h
@@ -16,6 +16,9 @@ void pane_toggled_handler(GtkWidget *widget, struct swappy_state *state);
 void undo_clicked_handler(GtkWidget *widget, struct swappy_state *state);
 void redo_clicked_handler(GtkWidget *widget, struct swappy_state *state);
 
+void rot_left_clicked_handler(GtkWidget *widget, struct swappy_state *state);
+void rot_right_clicked_handler(GtkWidget *widget, struct swappy_state *state);
+
 gboolean draw_area_handler(GtkWidget *widget, cairo_t *cr,
                            struct swappy_state *state);
 gboolean draw_area_configure_handler(GtkWidget *widget,
diff --git a/include/swappy.h b/include/swappy.h
index 383ac56..8365554 100644
--- a/include/swappy.h
+++ b/include/swappy.h
@@ -135,6 +135,10 @@ struct swappy_state_ui {
   GtkRadioButton *arrow;
   GtkRadioButton *blur;
 
+  // Rotation
+  GtkButton *rot_left;
+  GtkButton *rot_right;
+
   GtkRadioButton *red;
   GtkRadioButton *green;
   GtkRadioButton *blue;
diff --git a/res/swappy.glade b/res/swappy.glade
index e12a14f..b06c58a 100644
--- a/res/swappy.glade
+++ b/res/swappy.glade
@@ -12,6 +12,16 @@
     <property name="can_focus">False</property>
     <property name="icon_name">edit-undo-symbolic</property>
   </object>
+  <object class="GtkImage" id="edit-rot-left">
+    <property name="visible">True</property>
+    <property name="can_focus">False</property>
+    <property name="icon_name">object-rotate-left-symbolic</property>
+  </object>
+  <object class="GtkImage" id="edit-rot-right">
+    <property name="visible">True</property>
+    <property name="can_focus">False</property>
+    <property name="icon_name">object-rotate-right-symbolic</property>
+  </object>
   <object class="GtkImage" id="img-clear-paints">
     <property name="visible">True</property>
     <property name="can_focus">False</property>
@@ -819,6 +829,47 @@
                 <property name="non_homogeneous">True</property>
               </packing>
             </child>
+
+            <child>
+              <object class="GtkButton" id="rot-left-button">
+                <property name="visible">True</property>
+                <property name="sensitive">True</property>
+                <property name="can_focus">False</property>
+                <property name="focus_on_click">False</property>
+                <property name="receives_default">True</property>
+                <property name="tooltip_text" translatable="yes">Rotate Left</property>
+                <property name="image">edit-rot-left</property>
+                <property name="always_show_image">True</property>
+                <signal name="clicked" handler="rot_left_clicked_handler" swapped="no"/>
+              </object>
+              <packing>
+                <property name="expand">True</property>
+                <property name="fill">True</property>
+                <property name="position">2</property>
+                <property name="non_homogeneous">True</property>
+              </packing>
+            </child>
+
+            <child>
+              <object class="GtkButton" id="rot-right-button">
+                <property name="visible">True</property>
+                <property name="sensitive">True</property>
+                <property name="can_focus">False</property>
+                <property name="focus_on_click">False</property>
+                <property name="receives_default">True</property>
+                <property name="tooltip_text" translatable="yes">Rotate Right</property>
+                <property name="image">edit-rot-right</property>
+                <property name="always_show_image">True</property>
+                <signal name="clicked" handler="rot_right_clicked_handler" swapped="no"/>
+              </object>
+              <packing>
+                <property name="expand">True</property>
+                <property name="fill">True</property>
+                <property name="position">3</property>
+                <property name="non_homogeneous">True</property>
+              </packing>
+            </child>
+
             <child>
               <object class="GtkButton" id="clear">
                 <property name="visible">True</property>
@@ -832,7 +883,7 @@
               <packing>
                 <property name="expand">True</property>
                 <property name="fill">True</property>
-                <property name="position">2</property>
+                <property name="position">4</property>
                 <property name="secondary">True</property>
                 <property name="non_homogeneous">True</property>
               </packing>
diff --git a/src/application.c b/src/application.c
index 975e572..2fafe5a 100644
--- a/src/application.c
+++ b/src/application.c
@@ -387,6 +387,98 @@ void copy_clicked_handler(GtkWidget *widget, struct swappy_state *state) {
   clipboard_copy_drawing_area_to_selection(state);
 }
 
+static void compute_window_size_and_scaling_factor(struct swappy_state *state) {
+  GdkRectangle workarea = {0};
+  GdkDisplay *display = gdk_display_get_default();
+  GdkWindow *window = gtk_widget_get_window(GTK_WIDGET(state->ui->window));
+  GdkMonitor *monitor = gdk_display_get_monitor_at_window(display, window);
+  gdk_monitor_get_workarea(monitor, &workarea);
+
+  g_assert(workarea.width > 0);
+  g_assert(workarea.height > 0);
+
+  if (state->window) {
+    g_free(state->window);
+    state->window = NULL;
+  }
+
+  state->window = g_new(struct swappy_box, 1);
+  state->window->x = workarea.x;
+  state->window->y = workarea.y;
+
+  double threshold = 0.75;
+  double scaling_factor = 1.0;
+
+  int image_width = gdk_pixbuf_get_width(state->original_image);
+  int image_height = gdk_pixbuf_get_height(state->original_image);
+
+  int max_width = workarea.width * threshold;
+  int max_height = workarea.height * threshold;
+
+  g_info("size of image: %ux%u", image_width, image_height);
+  g_info("size of monitor at window: %ux%u", workarea.width, workarea.height);
+  g_info("maxium size allowed for window: %ux%u", max_width, max_height);
+
+  int scaled_width = image_width;
+  int scaled_height = image_height;
+
+  double scaling_factor_width = (double)max_width / image_width;
+  double scaling_factor_height = (double)max_height / image_height;
+
+  if (scaling_factor_height < 1.0 || scaling_factor_width < 1.0) {
+    scaling_factor = MIN(scaling_factor_width, scaling_factor_height);
+    scaled_width = image_width * scaling_factor;
+    scaled_height = image_height * scaling_factor;
+    g_info("rendering area will be scaled by a factor of: %.2lf",
+           scaling_factor);
+  }
+
+  state->scaling_factor = scaling_factor;
+  state->window->width = scaled_width;
+  state->window->height = scaled_height;
+
+  g_info("size of window to render: %ux%u", state->window->width,
+         state->window->height);
+}
+
+static void rotate(struct swappy_state *state, GdkPixbufRotation rotation) {
+  GdkPixbuf *flattened = pixbuf_get_from_state(state);
+  GdkPixbuf *rotated = gdk_pixbuf_rotate_simple(flattened, rotation);
+  g_object_unref(flattened);
+
+  if (!rotated) {
+    g_warning("unable to rotate pixbuf");
+    return;
+  }
+
+  if (state->original_image) {
+    g_object_unref(state->original_image);
+  }
+  state->original_image = rotated;
+
+  if (state->original_image_surface) {
+    cairo_surface_destroy(state->original_image_surface);
+  }
+  state->original_image_surface =
+      gdk_cairo_surface_create_from_pixbuf(state->original_image, 1, NULL);
+
+  paint_free_all(state);
+  compute_window_size_and_scaling_factor(state);
+  gtk_widget_set_size_request(GTK_WIDGET(state->ui->area), state->window->width,
+                              state->window->height);
+
+  render_state(state);
+  update_ui_undo_redo(state);
+}
+
+static void action_rot_left(struct swappy_state *state) {
+  rotate(state, GDK_PIXBUF_ROTATE_COUNTERCLOCKWISE);
+}
+
+static void action_rot_right(struct swappy_state *state) {
+  rotate(state, GDK_PIXBUF_ROTATE_CLOCKWISE);
+}
+
 void control_modifier_changed(bool pressed, struct swappy_state *state) {
   if (state->temp_paint != NULL) {
     switch (state->temp_paint->type) {
@@ -455,6 +547,16 @@ void window_keypress_handler(GtkWidget *widget, GdkEventKey *event,
       case GDK_KEY_y:
         action_redo(state);
         break;
+      case GDK_KEY_Left:
+        if (event->state & GDK_CONTROL_MASK) {
+          action_rot_left(state);
+        }
+        break;
+      case GDK_KEY_Right:
+        if (event->state & GDK_CONTROL_MASK) {
+          action_rot_right(state);
+        }
+        break;
       default:
         break;
     }
@@ -577,6 +679,14 @@ void redo_clicked_handler(GtkWidget *widget, struct swappy_state *state) {
   action_redo(state);
 }
 
+void rot_left_clicked_handler(GtkWidget *widget, struct swappy_state *state) {
+  action_rot_left(state);
+}
+
+void rot_right_clicked_handler(GtkWidget *widget, struct swappy_state *state) {
+  action_rot_right(state);
+}
+
 gboolean draw_area_handler(GtkWidget *widget, cairo_t *cr,
                            struct swappy_state *state) {
   GtkAllocation *alloc = g_new(GtkAllocation, 1);
@@ -762,60 +872,6 @@ void transparent_toggled_handler(GtkWidget *widget,
   action_transparent_toggle(state, &toggled);
 }
 
-static void compute_window_size_and_scaling_factor(struct swappy_state *state) {
-  GdkRectangle workarea = {0};
-  GdkDisplay *display = gdk_display_get_default();
-  GdkWindow *window = gtk_widget_get_window(GTK_WIDGET(state->ui->window));
-  GdkMonitor *monitor = gdk_display_get_monitor_at_window(display, window);
-  gdk_monitor_get_workarea(monitor, &workarea);
-
-  g_assert(workarea.width > 0);
-  g_assert(workarea.height > 0);
-
-  if (state->window) {
-    g_free(state->window);
-    state->window = NULL;
-  }
-
-  state->window = g_new(struct swappy_box, 1);
-  state->window->x = workarea.x;
-  state->window->y = workarea.y;
-
-  double threshold = 0.75;
-  double scaling_factor = 1.0;
-
-  int image_width = gdk_pixbuf_get_width(state->original_image);
-  int image_height = gdk_pixbuf_get_height(state->original_image);
-
-  int max_width = workarea.width * threshold;
-  int max_height = workarea.height * threshold;
-
-  g_info("size of image: %ux%u", image_width, image_height);
-  g_info("size of monitor at window: %ux%u", workarea.width, workarea.height);
-  g_info("maxium size allowed for window: %ux%u", max_width, max_height);
-
-  int scaled_width = image_width;
-  int scaled_height = image_height;
-
-  double scaling_factor_width = (double)max_width / image_width;
-  double scaling_factor_height = (double)max_height / image_height;
-
-  if (scaling_factor_height < 1.0 || scaling_factor_width < 1.0) {
-    scaling_factor = MIN(scaling_factor_width, scaling_factor_height);
-    scaled_width = image_width * scaling_factor;
-    scaled_height = image_height * scaling_factor;
-    g_info("rendering area will be scaled by a factor of: %.2lf",
-           scaling_factor);
-  }
-
-  state->scaling_factor = scaling_factor;
-  state->window->width = scaled_width;
-  state->window->height = scaled_height;
-
-  g_info("size of window to render: %ux%u", state->window->width,
-         state->window->height);
-}
-
 static void apply_css(GtkWidget *widget, GtkStyleProvider *provider) {
   gtk_style_context_add_provider(gtk_widget_get_style_context(widget), provider,
                                  GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
@@ -871,6 +927,11 @@ static bool load_layout(struct swappy_state *state) {
   state->ui->undo = GTK_BUTTON(gtk_builder_get_object(builder, "undo-button"));
   state->ui->redo = GTK_BUTTON(gtk_builder_get_object(builder, "redo-button"));
 
+  state->ui->rot_left =
+      GTK_BUTTON(gtk_builder_get_object(builder, "rot-left-button"));
+  state->ui->rot_right =
+      GTK_BUTTON(gtk_builder_get_object(builder, "rot-right-button"));
+
   GtkWidget *area =
       GTK_WIDGET(gtk_builder_get_object(builder, "painting-area"));
```
</details>

Even despite all that, I sometimes rotate my monitor in odd ways.
Due to Wayland fractional scaling this means I need to use
gpu-screen-recorder to avoid glitches.

```bash path=/etc/portage/package.accept_keywords/screenshot
# had to submit the first non live-ebuild for this one
media-video/gpu-screen-recorder ~amd64
```

I also integrate OCR optical character recognition
into my screenshot setup; see [clipboard ⟹](/conf/clipboard).

## 3. runners

To open apps, I use gui-apps/anyrun::funroll,
a windows+R style app runner that also supports Unicode
character search. I packaged it myself
on [::funroll ⇗](https://github.com/swomf/overlay-funroll/tree/main/gui-apps/anyrun) and
set my <em class="blue">USE</em> flags to exclude what I don't need.

<div class="ml-3">
  <span class="bright">Note:</span>
  "Excluding what I don't need" here is probably useless.

  I don't save
  space, since the space to store the tarball distfile is larger than
  the savings from excluding kidex support; and I don't have a non-negligibly
  faster startup, as anyrun is used sporadically anyway, only to start up apps.
</div>

I also use fuzzel for an [emoji menu shortcut ⇗](https://github.com/swomf/dotfiles/blob/gentoo/gentoo/config/hypr/executable/fuzzel-emoji).

```bash path=/etc/portage/package.use/anyrun
gui-apps/anyrun applications dictionary randr rink shell stdin symbols translate
gui-apps/fuzzel png svg # just in case
```

## 4. aylur's gtk shell

My bar and notification daemon are ags (Aylur's GTK Shell),
since its TypeScript GTK support
is expressive, minimal, and doesn't need a special domain-specific language.
It depends on astal libs. Both are on ::funroll.

```bash path=/etc/portage/package.use/ags
# cava is a WIP since libcava is compiled differently than cava
gui-libs/astal apps auth battery bluetooth gjs greetd gtk3 gtk4 hyprland io lua mpris network notifd powerprofiles river wireplumber tray
dev-lang/vala valadoc
# network USE flag deps
net-libs/libnma vala
net-misc/networkmanager vala
# gtk USE flag deps
gui-libs/gtk4-layer-shell vala
gui-libs/gtk-layer-shell vala
```

<span class="red">TODO:</span> another breaking change has hit ags. I need to
update to v3 at some point.

## 5. file manager

For a file manager I use
app-misc/nnn (terminal-based) and
gnome-extra/nemo (gui-based) over competitors:

* xfce-base/thunar needs xfce-base/tumbler for thumbnails, which
turns a minimal app into a non-minimal megabyte eater
* x11-misc/pcmanfm isn't very maintained -- e.g. no
wayland trackpad scroll. [bug 1131 ⇗](https://sourceforge.net/p/pcmanfm/bugs/1131/)

Setting up app-misc/nnn thumbnailing is done via
a [single-file plugin ⇗](https://github.com/swomf/dotfiles/blob/gentoo/gentoo/config/nnn/plugins/preview-tui)
and requires the Kitty terminal (or any terminal supporting
the Kitty image protocol).

Setting up good Nemo thumbnailing is a bit manual.

- To thumbnail media larger than 1MB on Nemo, you
need to go into Preferences -> Preview -> Only for files smaller than...?
and increase the limit.
- media-video/ffmpegthumbnailer is responsible for video thumbnails.
- x11-libs/gdk-pixbuf and gui-libs/gdk-pixbuf-loader-webp
are responsible for image thumbnails.

```bash path=/etc/portage/package.use/filemanager
# nnn
app-misc/nnn nerdfonts
# nemo and dependencies
gnome-extra/nemo exif
dev-libs/libdbusmenu gtk3
# nemo thumbnailing
media-video/ffmpegthumbnailer gnome gtk jpeg png
x11-libs/gdk-pixbuf jpeg gif tiff
```
