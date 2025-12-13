# games

I use Steam for typically-sourced games, and wine-proton for games.
Below is uninteresting stuff that gives no Gentoo-specific advantages.

## steam

As per the [wiki ⇗](https://wiki.gentoo.org/wiki/Steam),
add the repository steam-overlay and configure:

```bash path=/etc/portage/package.use/steam
app-accessibility/at-spi2-core  abi_x86_32
app-arch/bzip2                  abi_x86_32
app-arch/lz4                    abi_x86_32
app-arch/zstd                   abi_x86_32
dev-db/sqlite                   abi_x86_32
dev-lang/rust-bin               abi_x86_32
dev-libs/dbus-glib              abi_x86_32
dev-libs/elfutils               abi_x86_32
dev-libs/expat                  abi_x86_32
dev-libs/fribidi                abi_x86_32
dev-libs/glib                   abi_x86_32
dev-libs/gmp                    abi_x86_32
dev-libs/icu                    abi_x86_32
dev-libs/json-glib              abi_x86_32
dev-libs/libevdev               abi_x86_32
dev-libs/libffi                 abi_x86_32
dev-libs/libgcrypt              abi_x86_32
dev-libs/libgpg-error           abi_x86_32
dev-libs/libgudev               abi_x86_32
dev-libs/libgusb                abi_x86_32
dev-libs/libpcre2               abi_x86_32
dev-libs/libtasn1               abi_x86_32
dev-libs/libunistring           abi_x86_32
dev-libs/libusb                 abi_x86_32
dev-libs/libxml2                abi_x86_32
dev-libs/lzo                    abi_x86_32
dev-libs/nettle                 abi_x86_32
dev-libs/nspr                   abi_x86_32
dev-libs/nss                    abi_x86_32
dev-libs/openssl                abi_x86_32
dev-libs/wayland                abi_x86_32
dev-util/spirv-tools            abi_x86_32
dev-util/sysprof-capture        abi_x86_32
gnome-base/librsvg              abi_x86_32
gui-libs/libdecor               abi_x86_32
llvm-core/llvm                  abi_x86_32
media-gfx/graphite2             abi_x86_32
media-libs/alsa-lib             abi_x86_32
media-libs/flac                 abi_x86_32
media-libs/fontconfig           abi_x86_32
media-libs/freetype             abi_x86_32
media-libs/glu                  abi_x86_32
media-libs/harfbuzz             abi_x86_32
media-libs/lcms                 abi_x86_32
media-libs/libepoxy             abi_x86_32
media-libs/libglvnd             abi_x86_32
media-libs/libjpeg-turbo        abi_x86_32
media-libs/libogg               abi_x86_32
media-libs/libpng               abi_x86_32
media-libs/libpulse             abi_x86_32
media-libs/libsdl2              abi_x86_32
media-libs/libsndfile           abi_x86_32
media-libs/libva                abi_x86_32
media-libs/libvorbis            abi_x86_32
media-libs/mesa                 abi_x86_32
media-libs/openal               abi_x86_32
media-libs/opus                 abi_x86_32
media-libs/tiff                 abi_x86_32
media-sound/lame                abi_x86_32
media-sound/mpg123-base         abi_x86_32
media-video/pipewire            abi_x86_32
net-dns/c-ares                  abi_x86_32
net-dns/libidn2                 abi_x86_32
net-libs/gnutls                 abi_x86_32
net-libs/libasyncns             abi_x86_32
net-libs/libndp                 abi_x86_32
net-libs/libpsl                 abi_x86_32
net-libs/nghttp2                abi_x86_32
net-libs/nghttp3                abi_x86_32
net-misc/curl                   abi_x86_32
net-misc/networkmanager         abi_x86_32
net-print/cups                  abi_x86_32
sys-apps/dbus                   abi_x86_32
sys-apps/systemd                abi_x86_32
sys-apps/systemd-utils          abi_x86_32
sys-apps/util-linux             abi_x86_32
llvm-core/clang                 abi_x86_32
sys-libs/gdbm                   abi_x86_32
sys-libs/gpm                    abi_x86_32
sys-libs/libcap                 abi_x86_32
sys-libs/libudev-compat         abi_x86_32
sys-libs/ncurses                abi_x86_32
sys-libs/pam                    abi_x86_32
sys-libs/readline               abi_x86_32
sys-libs/zlib                   abi_x86_32
virtual/glu                     abi_x86_32
virtual/libelf                  abi_x86_32
virtual/libiconv                abi_x86_32
virtual/libintl                 abi_x86_32
virtual/libudev                 abi_x86_32
virtual/libusb                  abi_x86_32
virtual/opengl                  abi_x86_32
virtual/rust                    abi_x86_32
x11-libs/cairo                  abi_x86_32
x11-libs/extest                 abi_x86_32
x11-libs/gdk-pixbuf             abi_x86_32
x11-libs/gtk+                   abi_x86_32
x11-libs/gtk+                   abi_x86_32
x11-libs/libdrm                 abi_x86_32
x11-libs/libICE                 abi_x86_32
x11-libs/libpciaccess           abi_x86_32
x11-libs/libSM                  abi_x86_32
x11-libs/libvdpau               abi_x86_32
x11-libs/libX11                 abi_x86_32
x11-libs/libXau                 abi_x86_32
x11-libs/libxcb                 abi_x86_32
x11-libs/libXcomposite          abi_x86_32
x11-libs/libXcursor             abi_x86_32
x11-libs/libXdamage             abi_x86_32
x11-libs/libXdmcp               abi_x86_32
x11-libs/libXext                abi_x86_32
x11-libs/libXfixes              abi_x86_32
x11-libs/libXft                 abi_x86_32
x11-libs/libXi                  abi_x86_32
x11-libs/libXinerama            abi_x86_32
x11-libs/libxkbcommon           abi_x86_32
x11-libs/libXrandr              abi_x86_32
x11-libs/libXrender             abi_x86_32
x11-libs/libXScrnSaver          abi_x86_32
x11-libs/libxshmfence           abi_x86_32
x11-libs/libXtst                abi_x86_32
x11-libs/libXxf86vm             abi_x86_32
x11-libs/pango                  abi_x86_32
x11-libs/pixman                 abi_x86_32
x11-libs/xcb-util-keysyms       abi_x86_32
x11-misc/colord                 abi_x86_32
# nvidia
gui-libs/egl-gbm                abi_x86_32
gui-libs/egl-wayland            abi_x86_32
gui-libs/egl-x11                abi_x86_32
x11-drivers/nvidia-drivers      abi_x86_32
```

```bash path=/etc/portage/package.accept_keywords/overlays
*/*::steam-overlay ~amd64
```

```bash path=/etc/portage/package.accept_keywords/steam
games-util/game-device-udev-rules
sys-libs/libudev-compat
```

and install steam-launcher.

I use wine-proton (aka Valve's fork of Wine) because yeah.

```bash path=/etc/portage/package.use/wine
# wine
x11-libs/libXcursor             abi_x86_32
x11-libs/libXfixes              abi_x86_32
x11-libs/libXi                  abi_x86_32
x11-libs/libXrandr              abi_x86_32
x11-libs/libXrender             abi_x86_32
x11-libs/libXxf86vm             abi_x86_32
x11-libs/libXcomposite          abi_x86_32
sys-apps/dbus                   abi_x86_32
media-libs/fontconfig           abi_x86_32
media-libs/libglvnd             abi_x86_32
media-libs/libsdl2              abi_x86_32
net-libs/gnutls                 abi_x86_32
media-libs/freetype             abi_x86_32
media-libs/vulkan-loader        abi_x86_32 layers
media-libs/vulkan-layers        abi_x86_32
dev-util/vulkan-utility-libraries abi_x86_32
x11-libs/libX11                 abi_x86_32
x11-libs/libXext                abi_x86_32
media-libs/alsa-lib             abi_x86_32
dev-libs/glib                   abi_x86_32
media-libs/gst-plugins-base     abi_x86_32
media-libs/gstreamer            abi_x86_32
media-libs/libpulse             abi_x86_32
sys-libs/libunwind              abi_x86_32
dev-libs/wayland                abi_x86_32
x11-libs/libxkbcommon           abi_x86_32
media-plugins/gst-plugins-meta  abi_x86_32
media-libs/gst-plugins-good     abi_x86_32
media-plugins/gst-plugins-pulse abi_x86_32
media-libs/gst-plugins-bad      abi_x86_32
app-arch/bzip2                  abi_x86_32
x11-libs/libdrm                 abi_x86_32
media-libs/libva                abi_x86_32
x11-libs/libxcb                 abi_x86_32
x11-libs/libXau                 abi_x86_32
x11-libs/libXdmcp               abi_x86_32
x11-libs/libpciaccess           abi_x86_32
sys-libs/zlib                   abi_x86_32
dev-libs/libxml2                abi_x86_32
dev-libs/libffi                 abi_x86_32
media-libs/libsndfile           abi_x86_32
net-libs/libasyncns             abi_x86_32
media-libs/flac                 abi_x86_32
media-libs/libogg               abi_x86_32
media-libs/libvorbis            abi_x86_32
media-libs/opus                 abi_x86_32
media-sound/lame                abi_x86_32
media-sound/mpg123-base         abi_x86_32
sys-libs/libcap                 abi_x86_32
sys-libs/pam                    abi_x86_32
sys-libs/gdbm                   abi_x86_32
sys-libs/readline               abi_x86_32
sys-libs/ncurses                abi_x86_32
x11-libs/pango                  abi_x86_32
x11-libs/libXv                  abi_x86_32
media-libs/graphene             abi_x86_32
media-libs/libpng               abi_x86_32
media-libs/libjpeg-turbo        abi_x86_32
dev-libs/fribidi                abi_x86_32
media-libs/harfbuzz             abi_x86_32
x11-libs/cairo                  abi_x86_32
x11-libs/libXft                 abi_x86_32
dev-libs/lzo                    abi_x86_32
x11-libs/pixman                 abi_x86_32
media-gfx/graphite2             abi_x86_32
dev-libs/icu                    abi_x86_32
dev-libs/libpcre2               abi_x86_32
sys-apps/util-linux             abi_x86_32
dev-libs/libtasn1               abi_x86_32
dev-libs/libunistring           abi_x86_32
dev-libs/nettle                 abi_x86_32
dev-libs/gmp                    abi_x86_32
net-dns/libidn2                 abi_x86_32
media-video/pipewire            abi_x86_32
gui-libs/libdecor               abi_x86_32
media-libs/mesa                 abi_x86_32
dev-libs/expat                  abi_x86_32
dev-util/spirv-tools            abi_x86_32
llvm-core/llvm                  abi_x86_32
x11-libs/libxshmfence           abi_x86_32
x11-libs/xcb-util-keysyms       abi_x86_32
app-arch/zstd                   abi_x86_32
x11-libs/gtk+                   abi_x86_32
app-accessibility/at-spi2-core  abi_x86_32
media-libs/libepoxy             abi_x86_32
x11-libs/gdk-pixbuf             abi_x86_32
x11-libs/libXdamage             abi_x86_32
gnome-base/librsvg              abi_x86_32
x11-libs/libXtst                abi_x86_32
virtual/libintl                 abi_x86_32
virtual/libudev                 abi_x86_32
sys-apps/systemd-utils          abi_x86_32
dev-lang/rust-bin               abi_x86_32
virtual/libiconv                abi_x86_32
virtual/opengl                  abi_x86_32
virtual/glu                     abi_x86_32
media-libs/glu                  abi_x86_32
# wine-proton
dev-libs/libgcrypt              abi_x86_32
dev-libs/libgpg-error           abi_x86_32
media-plugins/gst-plugins-libav abi_x86_32
media-video/ffmpeg              abi_x86_32
virtual/zlib                    abi_x86_32
# wine-proton gstreamer, etc.
media-libs/libbs2b              abi_x86_32
media-libs/dav1d                abi_x86_32
media-sound/gsm                 abi_x86_32
media-libs/libiec61883          abi_x86_32
sys-libs/libavc1394             abi_x86_32
sys-libs/libraw1394             abi_x86_32
media-libs/libjxl               abi_x86_32
media-libs/libaom               abi_x86_32
media-libs/libass               abi_x86_32
media-libs/libplacebo           abi_x86_32
media-libs/libmodplug           abi_x86_32
media-libs/rubberband           abi_x86_32
app-arch/snappy                 abi_x86_32
media-libs/speex                abi_x86_32
net-libs/srt                    abi_x86_32
net-libs/libssh                 abi_x86_32
media-libs/svt-av1              abi_x86_32
media-libs/libtheora            abi_x86_32
media-libs/libv4l               abi_x86_32
media-libs/libwebp              abi_x86_32
media-libs/x264                 abi_x86_32
media-libs/x265                 abi_x86_32
media-libs/xvid                 abi_x86_32
media-libs/zimg                 abi_x86_32
media-libs/libsamplerate        abi_x86_32
sci-libs/fftw                   abi_x86_32
media-libs/shaderc              abi_x86_32
dev-libs/xxhash                 abi_x86_32
dev-util/glslang                abi_x86_32
app-arch/brotli                 abi_x86_32
dev-cpp/highway                 abi_x86_32
virtual/opencl                  abi_x86_32
dev-libs/opencl-icd-loader      abi_x86_32
```

Also see /var/db/repos/gentoo/app-emulation/wine-proton/files/README.gentoo to set
the following defaults in your zshrc:

```bash
export WINEFSYNC=1 # requires linux kernel >=5.16
export WINE_LARGE_ADDRESS_AWARE=1 # helps 32bit memory
# WINEPREFIX=/path/to setup_dxvk.sh install --symlink # requires app-emulation/dxvk
# WINEPREFIX=/path/to setup_vkd3d_proton.sh install --symlink # requires app-emulation/vkd3d-proton
```

using waydroid

```bash path=/etc/portage/package.use/android
sys-libs/libcap static-libs
```

In the kernel I also hit split lock for some stuff.

https://www.phoronix.com/news/Linux-Splitlock-Hurts-Gaming

```bash path=/etc/dracut.conf
kernel_cmdline+=" split_lock_detect=off "
```

```bash path=/etc/portage/package.accept_keywords/gaming
gui-wm/gamescope ~amd64
media-libs/vkroots ~amd64
```
