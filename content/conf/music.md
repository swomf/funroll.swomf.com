# music

## 1. musescore

I found a bug ([#952950 ⇗](https://bugs.gentoo.org/952950)) in Gentoo's packaging for MuseScore.
Lovely Gentoo maintainers patched it for me, as of ebuild 4.6.3 (not 4.6.2).

CELEBRATION 🎉🎉🎉🎉🎉🎉🎉

```bash path=/etc/portage/package.use/music
media-sound/musescore video
x11-base/xorg-server xvfb # for video
```

```bash path=/etc/portage/package.use/sound
media-video/pipewire pipewire-alsa
```

On the other hand the MuseScore devs added a change where
important env variables now have a MU_ prepended.

This is annoying because I use fractional scaling, so I need to
go out of my way to set MU_QT_QPA_PLATFORM=wayland
(not merely QT_QPA_PLATFORM=wayland) to make MuseScore
avoid XWayland which
doesn't support fractional scaling.
([pull 29618 ⇗](https://github.com/musescore/MuseScore/pull/29618)).

Pretty lame but I just patch the .desktop file.

```bash path=/etc/portage/patches/media-sound/musescore/edit-pull-29618.patch
diff --git a/buildscripts/packaging/Linux+BSD/org.musescore.MuseScore.desktop.in b/buildscripts/packaging/Linux+BSD/org.musescore.MuseScore.desktop.in
index e086e39ab..404759cab 100644
--- a/buildscripts/packaging/Linux+BSD/org.musescore.MuseScore.desktop.in
+++ b/buildscripts/packaging/Linux+BSD/org.musescore.MuseScore.desktop.in
@@ -12,7 +12,7 @@ Comment=Create, play and print beautiful sheet music
 Comment[ru]=Визуальный редактор нотных партитур
 Comment[fr]=Gravure de partitions musicales
 Icon=mscore@MUSE_APP_INSTALL_SUFFIX@
-Exec=mscore@MUSE_APP_INSTALL_SUFFIX@ %U
+Exec=env MU_QT_QPA_PLATFORM=wayland mscore@MUSE_APP_INSTALL_SUFFIX@ %U
 Terminal=false
 MimeType=application/x-musescore;application/x-musescore+xml;x-scheme-handler/musescore;application/vnd.recordare.musicxml;application/vnd.recordare.musicxml+xml;application/x-mei+xml;audio/midi;application/x-bww;application/x-biab;application/x-capella;audio/x-gtp;application/x-musedata;application/x-overture;audio/x-ptb;application/x-sf2;application/x-sf3;
 Categories=AudioVideo;Audio;Graphics;2DGraphics;VectorGraphics;RasterGraphics;Publishing;Midi;Mixer;Sequencer;Music;Qt;
```

## 2. reaper

Reaper, a digital audio workstation,
has a similar vibe to WinRAR:

It's paidn't.

```bash path=/etc/portage/package.accept_keywords/music
media-sound/reaper-bin
```

```bash path=/etc/portage/package.use/music
media-sound/reaper-bin ffmpeg mp3
```
