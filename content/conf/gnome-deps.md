# GNOME dependencies

I don't use GNOME, but I'm fine using its libraries. Below is
uninteresting dependency USE-flag plumbing.

For example, app-office/obsidian, a note-taking suite, uses some of
these.

```bash path=/etc/portage/package.use/gnome
# for xdg-utils (e.g. file pickers)
# used by apps such as Signal
app-text/xmlto text
# GNOME crypto library
app-crypt/gcr gtk
# for pango, a dep of gtk
media-libs/freetype harfbuzz
```
