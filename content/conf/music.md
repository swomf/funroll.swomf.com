# music

## 1. musescore

I found a bug ([#952950 ⇗](https://bugs.gentoo.org/952950)) in Gentoo's packaging for MuseScore. They patched it for me, as of 4.6.3 (not 4.6.2).

CELEBRATION 🎉🎉🎉🎉🎉🎉🎉

```bash path=/etc/portage/package.use/music
media-sound/musescore jack test video
x11-base/xorg-server xvfb # for video
```

```bash path=/etc/portage/package.use/sound
media-video/pipewire pipewire-alsa
```

## 2. reaper

Reaper is like WinRar: It's "paid".

```bash path=/etc/portage/package.accept_keywords/music
media-sound/reaper-bin
```

```bash path=/etc/portage/package.use/music
media-sound/reaper-bin ffmpeg mp3
```
