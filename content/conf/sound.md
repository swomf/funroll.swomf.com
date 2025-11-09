# sound

Below,

1. I set up Pipewire via a Gentoo NEWS item.
2. I deshittify my mic.
   1. I set up easyeffects.
   2. I add a Portage post-install hook to cleanly fix media-filter's .so
   (shared object) location.

## 1. pipewire

I use pipewire.

For installation I followed
[this news item (2022-07-29)](https://www.gentoo.org/support/news-items/2022-07-29-pipewire-sound-server.html)
as seen in `eselect news read`. I also added bluetooth, of course.

```bash path=/etc/portage/package.use/sound
media-video/pipewire sound-server bluetooth
media-video/pulseaudio -daemon
```

## 2.1. easyeffects

Oh no! Microphone quality sucks.

I install media-sound/pavucontrol to set up mic profiles (easy level),
and media-sound/easyeffects to filter out the cafetorium I'm in (medium level).

Read about EasyEffects on the [ArchWiki ⇗](https://wiki.archlinux.org/title/PipeWire#EasyEffects).

```bash path=/etc/portage/package.use/sound
# idk what these are, but there are so few so i add them all.
media-sound/easyeffects calf mda-lv2 zamaudio
# pulled in as dep for above
media-plugins/calf lv2
```

## 2.2 deep-filter ladspa

EasyEffects has a deep noise remover plugin. I've used it;
it's light yet highly effective, given you have a GPU.

Unfortunately it expects the shared object to be in a
different place from where Gentoo expects it to be.

Easy fix.

```bash path=/etc/portage/package.use/sound
media-sound/deep-filter ladspa
```

```bash path=/etc/portage/package.accept_keywords/sound
media-sound/deep-filter ~amd64
```

```bash path=/etc/portage/env/media-sound/deep-filter
post_src_install() {
  # easyeffects expects /usr/lib64/libdeep_filter_ladspa.so
  #   to be accessible at
  # /usr/lib64/ladspa/libdeep_filter_ladspa.so
  if use ladspa; then
    ebegin "Symlinking /usr/lib64/ladspa/libdeep_filter_ladspa.so -> /usr/lib64/libdeep_filter_ladspa.so"
    mkdir -p "${D}/usr/lib64/ladspa" || die
    ln -rsf "${D}/usr/lib64/libdeep_filter_ladspa.so" "${D}/usr/lib64/ladspa/libdeep_filter_ladspa.so" || die
    eend ${?}
  fi
}
```

<em class="grey">Insert a complaint about Linux mic defaults and needing an ML model here.</em>
