# books

I have an eye accessibility issue that I address as such:

* I use app-text/foliate to read epubs. My special use case is:
  * I rotate the screen to portrait to see longer amounts of text
  by keybinding to [this Hyprland-specific shell script ⇗](https://github.com/swomf/dotfiles/blob/gentoo/gentoo/config/hypr/executable/spin)
  * I patch Foliate to add a jet-black-by-default background theme, by Gentoo patch.

It's odd that I needed to use Gentoo here. Unlike
what upstream Foliate purports, adding to

~/.config/com.github.johnfactotum.Foliate/themes/themes.json

did nothing on my machine; thus I had to add the theme at build time.

```bash path=/etc/portage/patches/app-text/foliate/blackbriar-theme.patch
diff --git a/src/themes.js b/src/themes.js
index df053fe..dd10f25 100644
--- a/src/themes.js
+++ b/src/themes.js
@@ -49,6 +49,12 @@ export const themes = [
         light: { fg: '#2e3440', bg: '#eceff4', link: '#5e81ac' },
         dark: { fg: '#d8dee9', bg: '#2e3440', link: '#88c0d0' },
     },
+    {
+        name: 'blackbriar', label: _('Blackbriar'),
+        light: { fg: '#e0e0e0', bg: '#000000', link: '#77bbee' },
+        dark: { fg: '#e0e0e0', bg: '#000000', link: '#77bbee' },
+    },
+
 ]
 
 for (const { file, name } of utils.listDir(pkg.configpath('themes'))) try {
```

In other news, I use
[Atkinson Hyperlegible ttf](https://www.brailleinstitute.org/freefont/)
as a replacement for the serif. It is designed for individuals with
low vision and is available as <span class="nowrap">media-fonts/atkinson-hyperlegible::GURU.</span>

## Reminder: How to switch Foliate theme

1. Select a book
2. Collapse the left sidebar
3. Select the three-dot menu on the top right
4. Enable inversion, dark mode, and select the Blackbriar theme

My theme has both a "light" and "dark" mode, but "light" does not
color-invert images. If I bothered I'd patch this for my specific use-case.
