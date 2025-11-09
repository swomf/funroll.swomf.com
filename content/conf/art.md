# art

Arch Linux, like Gentoo, uses simplistic shell-like text files
to jot down build instructions, via PKGBUILD. But Arch starts out with
much more in their defaults in order to follow upstream;
Gentoo wants you to build up to it yourself.

Suppose I want to do this: Start off with an Arch Linux-like
<em class="blue">USE</em> flag roster, then <span class="red">subtract</span>
as I please.

I'll show you how it's done, that is, I'll compare:

<table>
  <tr>
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
    <td><em class="purple">RDEPEND</em><p class="m-0"><em class="blue">USE</em> flags will trigger new dependencies; we'll see which is needed to match.</p></em>
    </td>
  </tr>
</table>

to edit the <em class="blue">USE</em> flags of
(1) ImageMagick,
(2) GIMP, and
(3) Krita.

I'll also tell you all about what the hell these flags mean,
so that you don't go ohmygodgentoo and disable subtle things
for no good reason.

## 1. imagemagick

<p class="m-0"><span class="green">green</span>  :
  local <em class="blue">USE</em> flag</p>

<p class="m-0"><span class="yellow">yellow</span> :
  global <em class="blue">USE</em> flag or enabled by profile</p>

<p class="m-0"><span class="red">red</span>    :
  I ignore it</p>

<table>
  <thead>
    <tr>
      <th>Support for...</th>
      <th>Arch PKGBUILD</th>
      <th>Gentoo <em class="blue">USE</em> flag</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>X11</td>
      <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/main/PKGBUILD?ref_type=heads%3A~%3Atext%3Dlibxext#L25" target="_blank">depends ⇗</a> on libxext</td>
      <td><span class="yellow">X</span>
        <a href="https://github.com/gentoo/gentoo/blob/9c88af29fc0b3cc7afb601fcd7db6bd53650434f/media-gfx/imagemagick/imagemagick-9999.ebuild#L84" target="_blank">⇗</a>
        (this is added via global <em class="blue">USE</em> flag)</td>
    </tr>
    <tr>
      <td>BZIP2 compression</td>
      <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/main/PKGBUILD?ref_type=heads%3A~%3Atext%3Dlibxext#L12" target="_blank">depends ⇗</a>
        on bzip2</td>
      <td><span class="green">bzip2</span></td>
    </tr>
    <tr>
      <td>Microsoft core fonts</td>
      <td><em class="grey">idk lol</em></td>
      <td><span class="red">corefonts</span> (I'll use a GUI if I want to add text)</td>
    </tr>
    <tr>
      <td>C++ API</td>
      <td><em class="grey">idk lol</em></td>
      <td><em class="green">cxx*</em> (included by
        <a href="https://github.com/gentoo/gentoo/blob/9395f0b9de586d7858e9cdaf4240589300a5b1ec/media-gfx/imagemagick/imagemagick-9999.ebuild#L30" target="_blank">default IUSE ⇗</a>)</td>
    </tr>
    <tr>
      <td>DJVU (pdf-like format)</td>
      <td>--with-djvu</td>
      <td> <span class="red">djvu</span> (I don't magick things into .djvu)</td>
    </tr>
    <tr>
      <td>Fastest Fourier Transform In The West (a library)</td>
      <td><code>--with-fftw</code></td>
      <td class="green">fftw</td>
    </tr>
    <tr>
      <td>fontconfig stuff</td>
      <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/main/PKGBUILD?ref_type=heads%3A~%3Atext%3Dlibxext#L15" target="_blank">depends ⇗</a>
        on fontconfig</td>
      <td><span class="red">fontconfig</span> (I'll use a GUI if I want to add text)</td>
    </tr>
    <tr>
      <td>.fpx (old bitmap format)</td>
      <td>--<em class="red">without</em>-fpx</td>
      <td><span class="red">fpx</span></td>
    </tr>
    <tr>
      <td>graphviz (graph visualization library)</td>
      <td><em class="grey">idk lol</em></td>
      <td><span class="red">graphviz</span> (I'll use a GUI if I want to add graphs)</td>
    </tr>
    <tr>
      <td>hardening security policy</td>
      <td>The upstream default is "open"</td>
      <td><span class="yellow">hardened</span> (sets policy to
        "<a href="https://github.com/gentoo/gentoo/blob/9395f0b9de586d7858e9cdaf4240589300a5b1ec/media-gfx/imagemagick/imagemagick-9999.ebuild#L197" target="_blank">limited ⇗</a>")</td>
    </tr>
    <tr>
      <td>High Dynamic Image Range</td>
      <td>--enable-hdri</td>
      <td class="green">hdri</td>
    </tr>
    <tr>
      <td>HEIF (annoying compressed photo format, phones use it all the time
        even though image viewers barely support it)</td>
      <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/main/PKGBUILD?ref_type=heads%3A~%3Atext%3Dlibxext#L55" target="_blank">makedepends ⇗</a>
        on heif</td>
      <td> <span class="green">heif</span> (I need magick to convert heif images)</td>
    </tr>
    <tr>
      <td>JBIG (old image compression for fax machines)</td>
      <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/main/PKGBUILD?ref_type=heads%3A~%3Atext%3Dlibxext#L54" target="_blank">makedepends ⇗ </a>
        and
        <a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/main/PKGBUILD?ref_type=heads%3A~%3Atext%3Dlibxext#L30" target="_blank">optdepends ⇗ </a>
        on jbigkit</td>
      <td><span class="red">jbig</span> (who cares about fax...?)</td>
    </tr>
    <tr>
      <td>JPEG</td>
      <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/07ccf50eaf50381d02e83c46f711be1e3a1f73bd/PKGBUILD#L36" target="_blank">optdepends ⇗</a>
        and
        <a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/07ccf50eaf50381d02e83c46f711be1e3a1f73bd/PKGBUILD#L36" target="_blank">makedepends ⇗</a>
        on libjpeg-turbo</td>
      <td class="green">jpeg</td>
    </tr>
    <tr>
      <td>JPEG2000</td>
      <td><code>--with-openjp2</code></td>
      <td><span class="red">jpeg2k</span> (rare; also it was a
        <a href="https://www.lambdatest.com/web-technologies/jpeg2000" target="_blank">failure ⇗</a>
        compared to jpegxl, is
        not back-compatible with jpeg, etc.)</td>
    </tr>
    <tr>
      <td>JPEG XL</td>
      <td>--with-jxl</td>
      <td class="green">jpegxl</td>
    </tr>
    <tr>
      <td>LittleCMS (color profile management library)</td>
      <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/07ccf50eaf50381d02e83c46f711be1e3a1f73bd/PKGBUILD#L20" target="_blank">depends ⇗</a> on lcms2</td>
      <td><span class="green">lcms</span> (common, e.g.
        <a href="https://docs.digikam.org/fr/color_management/color_management_basis.html#:~:text=lcms" target="_blank">DigiKam ⇗</a>,
        <a href="https://docs.krita.org/de/reference_manual/preferences/color_management_settings.html#:~:text=Little%20CMS" target="_blank">Krita ⇗</a>,
        <a href="https://github.com/gentoo/gentoo/blob/f935fa5dd9442782c59df31bfdca0a25a570de60/media-gfx/gimp/gimp-9999.ebuild#L53" target="_blank">GIMP ⇗</a>)</td>
    </tr>
    <tr>
      <td>lqr (smart image upsizing via <em class="grey">-liquid-rescale</em>)</td>
      <td>--with-lqr</td>
      <td><span class="red">lqr</span> (a little too specialized,
        even GIMP would need an external plugin)</td>
    </tr>
    <tr>
      <td>LZMA/XZ compression</td>
      <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/main/PKGBUILD?ref_type=heads%3A~%3Atext%3Dlibxext#L27" target="_blank">depends ⇗</a>
        on xz (xz-utils supports lzma)</td>
      <td><span class="green">lzma</span></td>
    </tr>
    <tr>
      <td>OpenCL for gpu computation</td>
      <td>--enable-opencl</td>
      <td class="green">opencl</td>
    </tr>
    <tr>
      <td>OpenEXR (advanced hdr and compositing)</td>
      <td>--with-openexr</td>
      <td><span class="red">openexr</span>
        (this is more for
        <a href="https://en.wikipedia.org/wiki/OpenEXR#Overview" target="_blank">professional compositing pipelines ⇗</a>)</td>
    </tr>
    <tr>
      <td>OpenMP (parallel computing library)</td>
      <td><a href="https://imagemagick.org/script/openmp.php" target="_blank">probably default ⇗</a></td>
      <td><span class="green">openmp</span></td>
    </tr>
    <tr>
      <td>RaQM text layout library</td>
      <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/main/PKGBUILD?ref_type=heads%3A~%3Atext%3Dlibxext#L24" target="_blank">depends ⇗</a> on libraqm</td>
      <td><span class="red">pango</span> (I'll use a GUI if I want to add text)</td>
    </tr> 
    <tr>
      <td>Perl bindings</td>
      <td>--with-perl</td>
      <td><span class="green">perl</span> (in case I use someone's Perl script)
        (btw the PERL_FEATURES are set in <a href="https://www.gentoo.org/support/news-items/2024-05-07-perl-features-use-expand.html" target="_blank">a different way ⇗</a>)</td>
    </tr>
    <tr>
      <td>PNG</td>
      <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/main/PKGBUILD?ref_type=heads%3A~%3Atext%3Dlibxext#L23" target="_blank">depends ⇗</a> on libpng</td>
      <td> <em class="green">png</em> <a href="https://github.com/gentoo/gentoo/blob/9c88af29fc0b3cc7afb601fcd7db6bd53650434f/media-gfx/imagemagick/imagemagick-9999.ebuild#L67" target="_blank">⇗</a>
        (included by <a href="https://github.com/gentoo/gentoo/blob/9395f0b9de586d7858e9cdaf4240589300a5b1ec/media-gfx/imagemagick/imagemagick-9999.ebuild#L32" target="_blank">default IUSE ⇗</a>)</td>
    </tr>
    <tr>
      <td>PostScript/GhostScript (text render programming language)</td>
      <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/main/PKGBUILD?ref_type=heads%3A~%3Atext%3Dlibxext#L51" target="_blank">makedepends ⇗</a>
        and <a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/main/PKGBUILD?ref_type=heads%3A~%3Atext%3Dlibxext#L51" target="_blank">optdepends ⇗</a>
        on GhostScript</td>
      <td><span class="red">postscript</span> (I'll use a GUI if I want to add text)</td>
    </tr>
    <tr>
      <td>quantum depth value</td>
      <td>default upstream is <a href="https://imagemagick.org/Magick++/Quantum.html" target="_blank">64 ⇗</a></td>
      <td><span class="red">q8</span>, <span class="red">q32</span></td>
    </tr>
    <tr>
      <td>RAW (format for photographs from higher-end cameras)</td>
      <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/main/PKGBUILD?ref_type=heads%3A~%3Atext%3Dlibxext#L58" target="_blank">makedepends ⇗</a> on libraw</td>
      <td><span class="red">raw</span> (I don't use those cameras)</td>
    </tr>
    <tr>
      <td class="grey">static build (makes the libraries magick calls out to get bundled in instead. rare use case)</td>
      <td class="grey">--disable-static</td>
      <td class="red">static</td>
    </tr>
    <tr>
      <td class="grey">compile-time testing</td>
      <td><em class="grey">who cares?</em></td>
      <td><span class="red">test</span></td>
    </tr>
    <tr>
      <td>SVG</td>
      <td>--with-rsvg</td>
      <td><span class="green">svg</span></td>
    </tr>
    <tr>
      <td>TIFF image format</td>
      <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/main/PKGBUILD?ref_type=heads%3A~%3Atext%3Dlibxext#L36" target="_blank">optdepends ⇗</a> on libtiff</td>
      <td class="red">tiff <a href="https://github.com/gentoo/gentoo/blob/master/media-gfx/imagemagick/imagemagick-9999.ebuild#L67" target="_blank"> ⇗</a></td>
    </tr>
    <tr>
      <td>freetype font rendering</td>
      <td><a class="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/main/PKGBUILD?ref_type=heads%3A~%3Atext%3Dlibxext#L16">depends ⇗</a> on freetype2</td>
      <td><span class="red">truetype</span> (again idc about fonts)</td>
    </tr>
    <tr>
      <td>WebP image format (great format, annoying to support)</td>
      <td>--with-webp</td>
      <td class="green">webp</td>
    </tr>
    <tr>
      <td>.wmf (old Windows image format)</td>
      <td>--with-wmf</td>
      <td><span class="red">wmf</span> (was deprecated
        <a href="https://en.wikipedia.org/wiki/Windows_Metafile#:~:text=deprecated" target="_blank">ages ⇗</a> ago)</td>
    </tr>
    <tr>
      <td>XML I/O</td>
      <td>--with-xml</td>
      <td><span class="green">xml</span> (xml is used for confs probably, not sure)</td>
    </tr>
    <tr>
      <td>.ora OpenRaster support</td>
      <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/main/PKGBUILD?ref_type=heads%3A~%3Atext%3Dlibxext#L40" target="_blank">depends ⇗</a> on libzip</td>
      <td><span class="red">zip</span> (too rare, I don't care)</td>
    </tr>
    <tr>
      <td>ZLIB (implements DEFLATE compression algorithm, which is old but common)</td>
      <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/main/PKGBUILD?ref_type=heads%3A~%3Atext%3Dlibxext#L28" target="_blank">depends ⇗</a> on zlib</td>
      <td class="green">zlib</td>
    </tr></tbody>
</table>

Summarily, use this quite-horizontally-wide conf file.

```bash path=/etc/portage/package.use/imagemagick
media-gfx/imagemagick bzip2 fftw hdri heif jpeg jpegxl lcms lzma opencl openmp perl png svg webp xml zlib
# dependency for heif
media-libs/libheif x265
```


## 2. gimp

<table>
  <thead>
    <tr>
      <th>Support for...</th>
      <th>Arch PKGBUILD</th>
      <th>Gentoo <em class="blue">USE</em> flag</th>
    </tr>
    <tr>
  </thead>
  <tr>
    <td>X11</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/main/PKGBUILD?ref_type=heads%3A~%3Atext%3Dlibxext#L25" target="_blank">depends ⇗</a> on libxext</td>
    <td class="yellow">X</td>
  </tr>
  <tr>
    <td>aalib (lib to make ASCII graphics)</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/gimp/-/blob/main/PKGBUILD?ref_type=heads#L40" target="_blank">depends ⇗</a> on aalib</td>
    <td><span class="red">aalib</span>
      (ascii fidelity is wonky)</td>
  </tr>
  <tr>
    <td>ALSA support so MIDI knobs/pedals can change brush stuff</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/gimp/-/blob/main/PKGBUILD?ref_type=heads#L81" target="_blank">optdepends ⇗</a> on alsa-lib</td>
    <td><span class="red">alsa</span> (a bit advanced for me)</td>
  </tr>
  <tr>
    <td>Add extra documentation (API, Javadoc, etc). It is recommended to enable per package instead of globally</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/gimp/-/blob/main/PKGBUILD?ref_type=heads#L73" target="_blank">makedepends ⇗</a> on gtk-doc</td>
    <td><span class="red">doc</span></td>
  </tr>
  <tr>
    <td>.fits format support (deep space images)</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/gimp/-/blob/main/PKGBUILD?ref_type=heads#L82" target="_blank">optdepends ⇗</a>
      and
      <a href="https://gitlab.archlinux.org/archlinux/packaging/packages/gimp/-/blob/main/PKGBUILD?ref_type=heads#L73" >makedepends ⇗</a>
      on cfitsio</td>
    <td><span class="red">fits</span> (I rarely get raw deep-space imagedumps)</td>
  </tr>
  <tr>
    <td>remote filesystem drag-and-drop support</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/gimp/-/blob/main/PKGBUILD?ref_type=heads#L74" target="_blank">makedepends ⇗</a>
      and
      <a href="https://gitlab.archlinux.org/archlinux/packaging/packages/gimp/-/blob/main/PKGBUILD?ref_type=heads#L86" target="_blank">optdepends ⇗</a>
      on gvfs</td>
    <td><span class="green">gnome</span> (you'd be surprised; this isn't UI bloat)</td>
  </tr>
  <tr>
    <td>the dastardly heif/heic format</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/gimp/-/blob/main/PKGBUILD?ref_type=heads#L43" target="_blank">depends ⇗</a> on libheif</td>
    <td><span class="green">heif</span></td>
  </tr>
  <tr>
    <td>GJS (Gnome JavaScript) support</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/gimp/-/blob/main/PKGBUILD?ref_type=heads#L84">makedepends ⇗</a>
      and
      <a href="https://gitlab.archlinux.org/archlinux/packaging/packages/gimp/-/blob/main/PKGBUILD?ref_type=heads#L84" target="_blank">optdepends ⇗</a>
      on gjs</td>
    <td><span class="red">javascript</span> (too niche)</td>
  </tr>
  <tr>
    <td>jpeg2000</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/gimp/-/blob/main/PKGBUILD?ref_type=heads#L59" target="_blank">depends ⇗</a> on openjpeg2</td>
    <td><span class="red">jpeg2k</span></td>
  </tr>
  <tr>
    <td>JPEGXL image support</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/gimp/-/blob/main/PKGBUILD?ref_type=heads#L45" target="_blank">depends ⇗</a> on libjxl</td>
    <td><span class="green">jpegxl</span></td>
  </tr>
  <tr>
    <td>Enable Lua scripting support</td>
    <td><em class="grey">idk lol</em></td>
    <td><span class="red">lua</span>
      (luajit target
      is specified as USE_EXPAND)</td>
  </tr>
  <tr>
    <td>mng (animation support for png, superseded by apng)</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/gimp/-/blob/main/PKGBUILD?ref_type=heads#L46" target="_blank">depends ⇗</a> on mng</td>
    <td><span class="red">mng</span> (too rare, I don't care)</td>
  </tr>
  <tr>
    <td>OpenEXR (advanced hdr and compositing)</td>
    <td>--with-openexr</td>
    <td><span class="red">openexr</span> (ditto from earlier)</td>
  </tr>
  <tr>
    <td>OpenMP (parallel computing library)</td>
    <td><em class="grey">probably default idk</em></td>
    <td><span class="green">openmp</span></td>
  </tr>
  <tr>
    <td>PostScript language</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/gimp/-/blob/main/PKGBUILD?ref_type=heads#L67" target="_blank">makedepends ⇗</a>
      and
      <a href="https://gitlab.archlinux.org/archlinux/packaging/packages/gimp/-/blob/main/PKGBUILD?ref_type=heads#L83" target="_blank">optdepends ⇗</a>
      on ghostscript</td>
    <td><span class="red">postscript</span> (.ps is more for LaTeX stuff)</td>
  </tr>
  <tr>
    <td class="grey">python single target</td>
    <td class="grey">Arch doesn't build stuff like this</td>
    <td class="grey">auto-set by global USE_EXPAND directives</td>
  </tr>
  <tr>
    <td class="grey">compile-time tests</td>
    <td class="grey">who cares</td>
    <td><span class="red">test</span></td>
  </tr>
  <tr>
    <td>udev (device discovery, <span class="grey">probably related to gvfs or tablet device hotplugging idk</span>)</td>
    <td><em class="grey">idk lol</em></td>
    <td><span class="green">udev</span> (I use a drawing tablet
      and may un/replug it while GIMP is open)</td>
  </tr>
  <tr>
    <td>libunwind (call stack unwinding for debugging)</td>
    <td><span class="grey">probably not</span></td>
    <td><span class="red">unwind</span></td>
  </tr>
  <tr>
    <td>Vala support (a language for building GTK stuff)</td>
    <td><span class="grey">idk</span></td>
    <td><span class="red">vala</span></td>
  </tr>
  <tr>
    <td>Experimental high-DPI-centric vector icons support</td>
    <td><em class="yellow"> none :3</em></td>
    <td><span class="green">vector-icons</span></td>
  </tr>
  <tr>
    <td>Wayland support</td>
    <td>certainly supported. Gentoo only checks because it has
      a <a href="https://github.com/gentoo/gentoo/blob/f935fa5dd9442782c59df31bfdca0a25a570de60/media-gfx/gimp/gimp-9999.ebuild#L156" target="_blank">non-Wayland system bugfix ⇗</a></td>
    <td><span class="yellow">wayland</span></td>
  </tr>
  <tr>
    <td>WebP (common web format)</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/gimp/-/blob/main/PKGBUILD?ref_type=heads#L50" target="_blank">depends ⇗</a> on libwebp</td>
    <td><span class="green">webp</span></td>
  </tr>
  <tr>
    <td>.wmf (old Windows image format)</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/gimp/-/blob/main/PKGBUILD?ref_type=heads#L51" target="_blank">depends ⇗</a> on libwmf</td>
    <td><span class="red">wmf</span> (ditto from earlier)</td>
  </tr>
  <tr>
    <td>.xpm (pixmap graphics format for X window system)</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/gimp/-/blob/main/PKGBUILD?ref_type=heads#L57" target="_blank">depends ⇗</a> on libxpm</td>
    <td><span class="red">xpm</span>
      (absolutely
      <a href="https://en.wikipedia.org/wiki/X_PixMap" target="_blank">ancient ⇗</a>)</td>
  </tr>
</table>

Summarily,

```bash path=/etc/portage/package.use/gimp
media-libs/gegl cairo lcms
media-libs/babl lcms
media-gfx/gimp gnome heif jpegxl openmp udev vector-icons webp
```


```bash path=/etc/portage/package.accept_keywords/gimp
media-gfx/gimp ~amd64
media-libs/babl ~amd64
media-libs/gegl ~amd64
```

## 3. krita

I'm too lazy.

<table>
  <thead>
    <tr>
      <th>Support for...</th>
      <th>Arch PKGBUILD</th>
      <th>Gentoo <em class="blue">USE</em> flag</th>
    </tr>
    <tr>
  </thead>
  <tr>
    <td>Advanced color management with opencolorio</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/krita/-/blob/main/PKGBUILD#L41" target="_blank">depends ⇗</a> on opencolorio</td>
    <td><span class="green">color-management</span></td>
  </tr>
  <tr>
    <td>debug codepaths</td>
    <td><span class="grey">no</span></td>
    <td><span class="red">debug</span></td>
  </tr>
  <tr>
    <td>Fastest Fourier Transform In The West</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/krita/-/blob/main/PKGBUILD#L13" target="_blank">depends ⇗</a> on fftw</td>
    <td><span class="green">fftw</span></td>
  </tr>
  <tr>
    <td>Add GIF image support</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/krita/-/blob/main/PKGBUILD#L13" target="_blank">depends ⇗</a> on giflib</td>
    <td><span class="green">gif</span> (I have giflib on my system anyway)</td>
  </tr>
  <tr>
    <td>gsl (calculation library for better stability and precision for brush strokes)</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/krita/-/blob/main/PKGBUILD#L20" target="_blank">depends ⇗</a> on gsl</td>
    <td><span class="green">gsl</span></td>
  </tr>
  <tr>
    <td>dastardly .heif format</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/krita/-/blob/main/PKGBUILD#L55" target="_blank">makedepends ⇗</a>
      and
      <a href="https://gitlab.archlinux.org/archlinux/packaging/packages/krita/-/blob/main/PKGBUILD#L55" target="_blank">optdepends ⇗</a>
    on libheif</td>
    <td><span class="green">heif</span></td>
  </tr>
  <tr>
    <td>Support for JPEG 2000, a wavelet-based image compression format</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/krita/-/blob/main/PKGBUILD#L43" target="_blank">depends ⇗</a> on openjpeg2</td>
    <td><span class="red">jpeg2k</span></td>
  </tr>
  <tr>
    <td>Add JPEG XL image support</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/krita/-/blob/main/PKGBUILD#L68" target="_blank">makedepends ⇗</a>
      and
      <a href="https://gitlab.archlinux.org/archlinux/packaging/packages/krita/-/blob/main/PKGBUILD#L13" target="_blank">optdepends ⇗</a>
      on libjxl</td>
    <td><span class="green">jpegxl</span></td>
  </tr>
  <tr>
    <td>Media Lovin' Toolkit (animation-playing runtime engine+ffmpeg export)</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/krita/-/blob/main/PKGBUILD#L40" target="_blank">depends ⇗</a> on mlt</td>
    <td><span class="green">media</span></td>
  </tr>
  <tr>
    <td>libmypaint (for pressure, tilt, dabbing, etc. brush support)</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/krita/-/blob/main/PKGBUILD#L57" target="_blank">makedepends ⇗</a>
      and <a href="https://gitlab.archlinux.org/archlinux/packaging/packages/krita/-/blob/main/PKGBUILD#L69" target="_blank">optdepends ⇗</a>
      on libmypaint</td>
    <td><span class="green">mypaint-brush-engine</span></td>
  </tr>
  <tr>
    <td>OpenEXR (advanced hdr and compositing)</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/krita/-/blob/main/PKGBUILD#L42" target="_blank">depends ⇗</a> on openexr</td>
    <td><span class="red">openexr</span> (ditto)</td>
  </tr>
  <tr>
    <td>PDF support</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/krita/-/blob/main/PKGBUILD#L70" target="_blank">optdepends ⇗</a> on poppler-qt5</td>
    <td><span class="red">pdf</span></td>
  </tr>
  <tr>
    <td class="grey">python single target</td>
    <td class="grey">Arch doesn't build stuff like this</td>
    <td class="grey">auto-set by global USE_EXPAND directives</td>
  </tr>
  <tr>
      <td>RAW (format for photographs from higher-end cameras)</td>
      <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/imagemagick/-/blob/main/PKGBUILD?ref_type=heads%3A~%3Atext%3Dlibxext#L34" target="_blank">depends ⇗</a> on libkdcraw5</td>
      <td><span class="red">raw</span> (I don't use those cameras)</td>
    </tr>
  <tr>
      <td class="grey">compile-time testing</td>
      <td><em class="grey">who cares?</em></td>
      <td><span class="red">test</span></td>
    </tr>
  <tr>
    <td>WebP (common web image format)</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/krita/-/blob/main/PKGBUILD#L38" target="_blank">depends ⇗</a>
      on libwebp</td>
    <td><span class="green">webp</span></td>
  </tr>
  <tr>
    <td>Enable usage of SIMD instructions via dev-cpp/xsimd</td>
    <td><a href="https://gitlab.archlinux.org/archlinux/packaging/packages/krita/-/blob/main/PKGBUILD#L62" target="_blank">makedepends </a> on xsimd</td>
    <td><span class="green">xsimd</span></td>
  </tr>
</table>

```bash path=/etc/portage/package.use/krita
media-gfx/krita color-management fftw gif gsl heif jpegxl media mypaint-brush-engine webp xsimd
# deps
media-libs/opencv features2d
dev-python/pyqt5 gui widgets declarative network
dev-qt/qtcore icu
```

## bonus: blender

Blender has good defaults on Gentoo, so I don't need to change any flags.

```bash path=/etc/portage/package.accept_keywords/blender
media-gfx/blender-bin ~amd64
```

## summary

All that for a drop of <s>blood</s> vector-icons.
