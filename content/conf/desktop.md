# desktop

Below are some utilities I edit to make my desktop comfortable.

1. I set up gui-wm/hyprland, a window manager that uses the
Wayland display protocol.
2. I set up screenshotting, patching a poor keybind out of gui-wm/swappy.
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
But I hate that swappy closes when I hit Q, so I patch it out.

```bash path=/etc/portage/patches/gui-apps/swappy/remove-quit-button.patch
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

```bash path=/etc/portage/package.accept_keywords/screenshot
# had to submit the first non live-ebuild for this one
media-video/gpu-screen-recorder ~amd64
```

I also integrate OCR into my screenshot setup; see [clipboard ⟹](/conf/clipboard).

## 3. runners

To open apps, I use gui-apps/anyrun::funroll,
a windows+R style app runner. I packaged it myself
on [::funroll ⇗](https://github.com/swomf/overlay-funroll/tree/main/gui-apps/anyrun) and
set my <em class="blue">USE</em> flags to exclude what I don't need.

<div class="ml-3">
  <span class="bright">Note:</span>
  Excluding what I don't need here is probably useless. I don't save
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
