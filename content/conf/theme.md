# theme

Le font.

```bash path=/etc/portage/package.use/nerdfonts
media-fonts/nerdfonts sourcecodepro
# media-fonts/noto cjk # this USE flag was deleted after 20250701
                       # machines with 20250601 may still see the USE flag in eix
```

Le kvantum so i can have QT6 look like my gtk style

```bash path=/etc/portage/package.use/kvantum
x11-themes/kvantum qt5
```

```bash path=/etc/portage/package.accept_keywords/kvantum
x11-themes/kvantum
```

<strong class="red">TODO:</strong> I'm not sure kvantum is working... (╥﹏╥)
