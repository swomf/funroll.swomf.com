# video

Arch Linux, like Gentoo, uses simplistic shell-like text files to jot down build instructions, via PKGBUILD. But Arch starts out with much more in their defaults in order to follow upstream; Gentoo wants you to build up to it yourself.

Suppose I want to do this: Start off with an Arch Linux-like
<em class="blue">USE</em> flag roster, then <span class="red">subtract</span>
as I please.

I'll show you how it's done, that is, I'll compare:

<table>
  <tbody><tr>
    <td>Arch Linux PKGBUILD</td>
    <td>Gentoo ebuild</td>
  </tr>
  <tr>
    <td><em class="purple">./configure</em> call arguments</td>
    <td><em class="purple">myeconfargs</em> declaration</td>
  </tr>
  <tr>
    <td><em class="purple">makedepends</em>/<em class="purple">optdepends</em>
      declarations
      <p class="m-0">Sometimes
        <em>./configure</em> is subtractive, so stuff is
        included by default if unspecified. Then a lib may be in
        <em>makedepends</em>/<em>optdepends</em>.</p>
    </td>
    <td><em class="purple">RDEPEND</em><p class="m-0"><em class="blue">USE</em> flags will trigger new dependencies; we'll see which is needed to match.</p>
    </td>
  </tr>
</tbody></table>

to edit the <em class="blue">USE</em> flags of ffmpeg.

I'll also tell you what the hell these flags mean
so you don't go ehrmagerdgentoo and disable subtle things
for no good reason.

<p class="m-0"><span class="green">green</span>  :
  local <em class="blue">USE</em> flag</p>

<p class="m-0"><span class="yellow">yellow</span> :
  global <em class="blue">USE</em> flag or enabled by profile</p>

<p class="mt-0"><span class="red">red</span>    :
  I ignore it</p>


<table>
  <thead>
    <tr>
      <th>Support for...</th>
      <th>Arch PKGBUILD</th>
      <th>Gentoo <em class="blue">USE</em> flag</th>
    </tr>
  </thead>
  <tr>
    <td>X11</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L59" target="_blank">depends ⇗</a> on libxext</td>
    <td><span class="green">X</span></td>
  </tr>
  <tr>
    <td>32-bit (x86) libraries</td>
    <td><em class="grey">idk lol</em></td>
    <td><span class="yellow">abi_x86_32</span> (pulled in by wine-proton)</td>
  </tr>
  <tr>
    <td>ALSA (sound architecture)</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L16" target="_blank">depends ⇗</a> on alsa</td>
    <td><span class="red">alsa</span> <em class="grey">(idk)</em></td>
  </tr>
  <tr>
    <td>AMD's Advanced Media Framework</td>
    <td><a href="" target="_blank">--enable-amf ⇗</a>, via aur/ffmpeg-full only </td>
    <td><span class="red">amf</span> (I have Intel/Nvidia)</td>
  </tr>
  <tr>
    <td>Adaptive Multi-Rate Audio, opencore</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L65" target="_blank">depends ⇗</a> on opencore-amr</td>
    <td><span class="red">amr</span> <em class="grey">(idk)</em></td>
  </tr>
  <tr>
    <td>Adaptive Multi-Rate Audio encoding, vo-amrwbenc</td>
    <td><a href="https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=ffmpeg-full#n93" target="_blank">depends ⇗</a> on vo-amrwbenc, via ffmpeg-full only</td>
    <td><span class="red">amrenc</span></td>
  </tr>
  <tr>
    <td>Blu-ray filesystems reading support</td>
  <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L36" target="_blank">--enable-libbluray ⇗</a></td>
    <td><span class="red">libbluray</span> (I probably won't use a bluray adapter)</td>
  </tr>
  <tr>
    <td>stereo-to-binaural filter</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L162" target="_blank">--enable-libbs2b ⇗</a></td>
    <td><span class="red">bs2b</span> <em class="grey">(idk)</em></td>
  </tr>
  <tr>
    <td>Enable bzip2 compression support</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L18" target="_blank">depends ⇗</a> on bzip2</td>
    <td><span class="yellow">bzip2</span> <em class="grey">(idk what its pulled in by ngl)</em></td>
  </tr>
  <tr>
    <td>Audio CDs reading</td>
    <td><a href="https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=ffmpeg-full#n276" target="_blank">--enable-libcdio ⇗</a>, via ffmpeg-full only </td>
    <td><span class="red">cdio</span></td>
  </tr>
  <tr>
    <td>Audio fingerprinting</td>
    <td><a href="https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=ffmpeg-full#n260" target="_blank">--enable-chromaprint</a>, via ffmpeg-full only</td>
    <td><span class="red">chromaprint</span></td>
  </tr>
  <tr>
    <td>libffmpeg.so for media playback in Chromium-based browsers like Opera and Vivaldi.</td>
    <td><em class="grey">Not sure if <a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L18" target="_blank">Arch's av_stream patch ⇗</a> and
      <a href="https://github.com/gentoo/gentoo/blob/master/media-video/ffmpeg/files/chromium-r3.patch" target="_blank" >Gentoo's av_stream/Makefile patch ⇗</a> are the same</em></td>
    <td><span class="red">chromium</span></td>
  </tr>
  <tr>
    <td>Codec2 low bitrate speech codec support</td>
    <td><a href="https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=ffmpeg-full#n277" target="_blank">--enable-libcodec2 ⇗</a>, via ffmpeg-full only </td>
    <td><span class="red">codec2</span></td>
  </tr>
  <tr>
    <td>GPU-accelerated filters via NVIDIA PTX, compiled with llvm-core/clang</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L150" target="_blank">--enable-cuda-llvm ⇗</a></td>
    <td><span class="green">cuda</span></td>
  </tr>
  <tr>
    <td>AV1 decoding</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L163" target="_blank">--enable-libdav1d ⇗</a></td>
    <td><span class="red">dav1d</span> <em class="grey">(idk)</em></td>
  </tr>
  <tr>
    <td>Make some extra docs</td>
    <td><a href="https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=ffmpeg-full#n253" target="_blank">--disable-htmlpages ⇗</a>, in ffmpeg-full but not ffmpeg? idk</td>
    <td><span class="red">doc</span></td>
  </tr>
  <tr>
    <td>libdrm hardware acceleration and KMS screencaps</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L164" target="_blank">--enable-libdrm ⇗</a></td>
    <td><span class="green">drm</span></td>
  </tr>
  <tr>
    <td>DVDs</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L165" target="_blank">--enable-libdvdnav --enable-libdvdread ⇗</a></td>
    <td><span class="red">dvd</span> (I probably won't ffmpeg right from a dvd adapter)</td>
  </tr>
  <tr>
    <td>Use Fraunhofer AAC for AAC in addition to ffmpeg's own implementation.
    AAC is used in mp4, m4v, mkv, mov.</td>
    <td><a href="https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=ffmpeg-full#n283" target="_blank">--enable-libfdk-aac ⇗</a>, via ffmpeg-full only </td>
    <td><span class="red">fdk</span> <em class="grey">(idk)</em></td>
  </tr>
  <tr>
    <td>flite text-to-speech filter</td>
    <td><a href="https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=ffmpeg-full#n284" target="_blank">--enable-libflite ⇗</a>, via ffmpeg-full only </td>
    <td><span class="red">flite</span></td>
  </tr>
  <tr>
    <td>Configure/customize font access via fontconfig</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L152" target="_blank">--enable-fontconfig ⇗</a></td>
    <td><span class="green">fontconfig</span></td>
  </tr>
  <tr>
    <td>frei0r filters</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L153" target="_blank">--enable-frei0r ⇗</a></td>
    <td><span class="red">frei0r</span> <em class="grey">(idk)</em></td>
  </tr>
  <tr>
    <td>Bidi support for the drawtext filter</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L168" target="_blank">--enable-libfribidi ⇗</a></td>
    <td><span class="green">fribidi</span></td>
  </tr>
  <tr>
    <td>Use libgcrypt for rtmp(t)e support (not needed if using any of USE=gmp,librtmp,openssl), and for obtaining random bytes (not needed if USE=openssl)</td>
    <td><a href="https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=ffmpeg-full#n262" target="_blank">--enable-libgcrypt ⇗</a>, ffmpeg-full only</td>
    <td><span class="red">gcrypt</span></td>
  </tr>
  <tr>
    <td>Game music format support via media-libs/game-music-emu</td>
    <td><a href="" target="_blank">--enable-libgme ⇗</a>, ffmpeg-full only</td>
    <td><span class="red">gme</span></td>
  </tr>
  <tr>
    <td>GNU MP library</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L154" target="_blank">--enable-gmp ⇗</a> on </td>
    <td><span class="green">gmp</span></td>
  </tr>
  <tr>
    <td>Use net-libs/gnutls for TLS/HTTPS support and other minor functions (has no effect if USE=openssl is set) </td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L155" target="_blank">--enable-gnutls ⇗</a></td>
    <td><span class="red">gnutls</span></td>
  </tr>
  <tr>
    <td>Don't force LGPL</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L156" target="_blank">--enable-gpl ⇗</a></td>
    <td><span class="yellow">gpl</span> (enabled by ebuild)</td>
  </tr>
  <tr>
    <td>GSM lossy speech compression codec</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L170" target="_blank">--enable-libgsm ⇗</a></td>
    <td><span class="green">gsm</span></td>
  </tr>
  <tr>
    <td>FireWire DV/HDV input devices</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L172" target="_blank">--enable-libiec61883 ⇗</a></td>
    <td><span class="green">iec61883</span> <em class="grey">(idk)</em></td>
  </tr>
  <tr>
    <td>FireWire/iLink IEEE1394 support (dv, camera, ...)</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L35" target="_blank">depends ⇗</a> on libavc1394, libraw1394</td>
    <td><span class="red">ieee1394</span> <em class="grey">(idk)</em></td>
  </tr>
  <tr>
    <td>JACK Audio Connection Kit</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L173" target="_blank">--enable-libjack ⇗</a></td>
    <td><span class="red">jack</span> <em class="grey">(I don't think I use it, I just use pipewire.
      idrk what jack is ngl)</em></td>
  </tr>
  <tr>
    <td>JPEG 2000</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L179" target="_blank">--enable-libopenjpeg ⇗</a></td>
    <td><span class="red">jpeg2k</span> <em class="grey">(idk)</em></td>
  </tr>
  <tr>
    <td>JPEG XL aka JXL</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/ffmpeg/-/blob/main/PKGBUILD#L174" target="_blank">--enable-libjxl ⇗</a> on </td>
    <td><span class="green">jpegxl</span></td>
  </tr>
  <tr>
    <td>Enable H.265/HEVC encoding support via media-libs/kvazaar</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">kvazaar</span></td>
  </tr>
  <tr>
    <td>Enable the ability to support ladspa plugins</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">ladspa</span></td>
  </tr>
  <tr>
    <td>Add support for MP3 encoding using LAME</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">lame</span></td>
  </tr>
  <tr>
    <td>Enable ICC profile support via media-libs/lcms</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">lcms</span></td>
  </tr>
  <tr>
    <td>Enable AV1 de/encoding via media-libs/libaom (warning: this is the reference implementation and is slower than the alternatives) </td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">libaom</span></td>
  </tr>
  <tr>
    <td>Enable ARIB text and caption decoding via media-libs/aribb24</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">libaribb24</span></td>
  </tr>
  <tr>
    <td>SRT/SSA/ASS (SubRip / SubStation Alpha) subtitle support</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">libass</span></td>
  </tr>
  <tr>
    <td>Add support for colored ASCII-art graphics</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">libcaca</span></td>
  </tr>
  <tr>
    <td>Enable internet Low Bitrate Codec de/encoding support via media-libs/libilbc</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">libilbc</span></td>
  </tr>
  <tr>
    <td>Enable Low Complexity Communication Codec de/encoding support via media-sound/liblc3</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">liblc3</span></td>
  </tr>
  <tr>
    <td>Enable use of GPU-accelerated filters from media-libs/libplacebo</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">libplacebo</span></td>
  </tr>
  <tr>
    <td>Enable Real Time Messaging Protocol support via media-video/rtmpdump in addition to FFmpeg's own implementation </td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">librtmp</span></td>
  </tr>
  <tr>
    <td>Enable use of the audio resampler from media-libs/soxr</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">libsoxr</span></td>
  </tr>
  <tr>
    <td>Enable Optical Character Recognition (OCR) filter support via app-text/tesseract</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">libtesseract</span></td>
  </tr>
  <tr>
    <td>Enable use of filters through media-libs/lv2</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">lv2</span></td>
  </tr>
  <tr>
    <td>Support for LZMA compression algorithm</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">lzma</span></td>
  </tr>
  <tr>
    <td>Add libmodplug support for playing SoundTracker-style music files</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">modplug</span></td>
  </tr>
  <tr>
    <td>Add support for NVIDIA Encoder/Decoder (NVENC/NVDEC) API for hardware accelerated encoding and decoding on NVIDIA cards (requires x11-drivers/nvidia-drivers)</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">nvenc</span></td>
  </tr>
  <tr>
    <td>Add support for the Open Audio Library</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">openal</span></td>
  </tr>
  <tr>
    <td>Enable OpenCL support (computation on GPU)</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">opencl</span></td>
  </tr>
  <tr>
    <td>Add support for OpenGL (3D graphics)</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">opengl</span></td>
  </tr>
  <tr>
    <td>Enable H.264 encoding support via media-libs/openh264</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">openh264</span></td>
  </tr>
  <tr>
    <td>Enable MPTM tracked music files decoding support via media-libs/libopenmpt</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">openmpt</span></td>
  </tr>
  <tr>
    <td>Enable using dev-libs/openssl for TLS/HTTPS support and other minor functions (USE=gnutls has no effect if set) </td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">openssl</span></td>
  </tr>
  <tr>
    <td>Enable Opus audio codec support</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">opus</span></td>
  </tr>
  <tr>
    <td>Enable libpostproc video post processing library support (should not disable this unless need to disable USE=gpl) </td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">postproc</span></td>
  </tr>
  <tr>
    <td>Add sound server support via media-libs/libpulse (may be PulseAudio or PipeWire)</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">pulseaudio</span></td>
  </tr>
  <tr>
    <td>Enable QR encode generation support via media-gfx/qrencode</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">qrcode</span></td>
  </tr>
  <tr>
    <td>Enable Intel Quick Sync Video support via media-libs/libvpl</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">qsv</span></td>
  </tr>
  <tr>
    <td>Enable QR decoding support via media-libs/quirc</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">quirc</span></td>
  </tr>
  <tr>
    <td>Enable AMQP stream support via net-libs/rabbitmq-c</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">rabbitmq</span></td>
  </tr>
  <tr>
    <td>Enable AV1 encoding support via media-video/rav1e</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">rav1e</span></td>
  </tr>
  <tr>
    <td>Enable time-stretching and pitch-shifting audio filter support via media-libs/rubberband</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">rubberband</span></td>
  </tr>
  <tr>
    <td>Add support for SAMBA (Windows File and Printer sharing)</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">samba</span></td>
  </tr>
  <tr>
    <td>Enable use of the Simple Direct Layer library (required for the ffplay command)</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">sdl</span></td>
  </tr>
  <tr>
    <td>Enable support for various GPU-accelerated filters using Vulkan compiled with media-libs/shaderc</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">shaderc</span></td>
  </tr>
  <tr>
    <td>Enable Snappy compression support via app-arch/snappy (required for Vidvox Hap encoder support) </td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">snappy</span></td>
  </tr>
  <tr>
    <td>Enable audio output support via media-sound/sndio</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">sndio</span></td>
  </tr>
  <tr>
    <td>Apply additional patches for efficient playback on some SoCs (e.g. ARM, RISC-V)</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">soc</span></td>
  </tr>
  <tr>
    <td>Add support for the speex audio codec (used for speech)</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">speex</span></td>
  </tr>
  <tr>
    <td>Enable Secure Reliable Transport (SRT) support via net-libs/srt</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">srt</span></td>
  </tr>
  <tr>
    <td>Enable SSH/SFTP support via net-libs/libssh</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">ssh</span></td>
  </tr>
  <tr>
    <td>Add support for SVG (Scalable Vector Graphics)</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">svg</span></td>
  </tr>
  <tr>
    <td>: Enable AV1 encoding support via media-libs/svt-av1</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">svt</span></td>
  </tr>
  <tr>
    <td>Add support for the Theora Video Compression Codec</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">theora</span></td>
  </tr>
  <tr>
    <td>Enable drawtext filter support via media-libs/freetype and media-libs/harfbuzz </td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">truetype</span></td>
  </tr>
  <tr>
    <td>Enable MP2 encoding support via media-sound/twolame in addition to FFmpeg's own implementation </td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">twolame</span></td>
  </tr>
  <tr>
    <td>Enable support for video4linux (using linux-headers or userspace libv4l libraries)</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">v4l</span></td>
  </tr>
  <tr>
    <td>Enable Video Acceleration API for hardware decoding</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">vaapi</span></td>
  </tr>
  <tr>
    <td>Enable the Video Decode and Presentation API for Unix acceleration interface</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">vdpau</span></td>
  </tr>
  <tr>
    <td>: Verify upstream signatures on distfiles</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">verify</span></td>
  </tr>
  <tr>
    <td>Enable video stabilization filter support via media-libs/vidstab</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">vidstab</span></td>
  </tr>
  <tr>
    <td>Enable Netflix's perceptual video quality assessment filter support via media-libs/libvmaf</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">vmaf</span></td>
  </tr>
  <tr>
    <td>Add support for the OggVorbis audio codec</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">vorbis</span></td>
  </tr>
  <tr>
    <td>Enable VP8 and VP9 de/encoding support via media-libs/libvpx in addition to FFmpeg's own implementation (for decoding only) </td>
    <td>(for webm encoding)<a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="green">vpx</span></td>
  </tr>
  <tr>
    <td>Add support for 3D graphics and computing via the Vulkan cross-platform API</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">vulkan</span></td>
  </tr>
  <tr>
    <td>Add support for the WebP image format</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">webp</span></td>
  </tr>
  <tr>
    <td>Enable h264 encoding using x264</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">x264</span></td>
  </tr>
  <tr>
    <td>Enable H.265/HEVC encoding support via media-libs/x265</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">x265</span></td>
  </tr>
  <tr>
    <td>Enable Dynamic Adaptive Streaming over HTTP (DASH) stream support using dev-libs/libxml2</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">xml</span></td>
  </tr>
  <tr>
    <td>Add support for xvid.org's open-source mpeg-4 codec</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">xvid</span></td>
  </tr>
  <tr>
    <td>Enable ZMQ command receiver filter and streaming support via net-libs/zeromq</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">zeromq</span></td>
  </tr>
  <tr>
    <td>Enable zscale filter support using media-libs/zimg</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">zimg</span></td>
  </tr>
  <tr>
    <td>Add support for zlib compression</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">zlib</span></td>
  </tr>
  <tr>
    <td>Enable teletext decoding support via media-libs/zvbi</td>
    <td><a href="" target="_blank">makedepends ⇗</a> on </td>
    <td><span class="red">zvbi</span></td>
  </tr>
</table>

```bash path=/etc/portage/package.use/ffmpeg
media-video/ffmpeg cuda fontconfig gmp ladspa libaom libass bluray bs2b libdrm fribidi glslang gsm iec61883 jpegxl modplug mp3 opus libplacebo rav1e svg rubberband snappy soxr speex srt ssh svt-av1 theora libv4l v4l vorbis webp x264 x265 libxml2 xvid zimg nvdec nvenc opencl opengl vulkan sdl lame -bluray vpx
media-libs/libvpx abi_x86_32
```

* no libdvdnav/dvdread/harfbuzz
* no libopencore_amrnb, libopencore_amrwb, openjpeg, openmpt,
* no libxcb, libzmq, shared, vapoursynth, version3, vulkan
* ignored JACK (i don't use jack), vmaf (some netflix thing), amf (i don't use amd)
* ignored frei0r (brings in opencv? idk)
* lame is for mp3
* no bluray

I also do video editing

```bash path=/etc/portage/package.use/kdenlive
media-libs/libsdl2 gles2
media-libs/mlt sdl frei0r ffmpeg qt6 xml
kde-frameworks/sonnet qml
dev-qt/qt5compat qml icu
media-libs/opencv contrib contribdnn qt6
dev-qt/qtbase libproxy icu #dafuq
kde-frameworks/kconfig qml
dev-qt/qtgui egl
sys-auth/polkit-qt qt6
kde-frameworks/prison qml
```

I rarely use OBS for screen recording, since it is heavy

```bash path=/etc/portage/package.accept_keywords/screen-recording
media-video/obs-studio ~amd64
gui-apps/wf-recorder ~amd64
```

subtitles

```bash path=/etc/portage/package.use/aegisub
dev-lang/luajit lua52compat
dev-libs/boost icu
```
