# math

I set up

1. LaTeX, and explain its flags (detailed-ish)
2. R with disabled LTO, mentioning nix-shell (short)
3. Octave (uninteresting)

## 1. latex

Blah blah, LaTeX collections.

Not to handhold, but
you can just check <em class="grey nowrap">/var/db/repos/gentoo/dev-texlive/texlive-<em class="red">$USE_FLAG_NAME</em></em> if the <em class="blue">USE</em> flag is a TeXLive collection, e.g. luatex.

<p class="m-0"><span class="green">green</span>  :
  local <em class="blue">USE</em> flag</p>

<p class="m-0"><span class="yellow">yellow</span> :
  global <em class="blue">USE</em> flag or enabled by profile</p>

<p class="mt-0"><span class="red">red</span>    :
  I ignore it</p>

<table>
  <thead>
    <tr>
      <th>feature</th>
      <th>flag</th>
    </tr>
  </thead>
  <tr>
    <td>X11 I guess?</td>
    <td><span class="yellow">X</span> (from make.conf)</td>
  </tr>
  <tr>
    <td>Multi-byte east Asian characters</td>
    <td><span class="red">cjk</span></td>
  </tr>
  <tr>
    <td>ConTeXt macros. Less common to use.</td>
    <td><span class="red">context</span></td>
  </tr>
<tr>
    <td>Goodies such as
      <em class="blue">titlesec</em>
      and
      <em class="blue">enumitem</em></td>
    <td><span class="green">extra</span></td>
  </tr>
  <tr>
    <td>Chess images, etc.</td>
    <td><span class="red">games</span></td>
  </tr>
<tr>
    <td><em class="blue">graphicx</em>,
      <em class="blue">tikz</em>,
      <em class="blue">pgfplot</em>...</td>
    <td><span class="green">graphics</span></td>
  </tr>
  <tr>
    <td>Special figures/citations/etc. for law, critical texts, linguistics, e.g.
    <em class="blue">bibleref</em>,
      <em class="blue">adtrees</em>,
      <em class="blue">jura</em></td>
    <td><span class="red">humanities</span></td>
  </tr>
  <tr>
    <td>LuaLaTeX has a better macro language and better Unicode support,
    but can be slower</td>
    <td><span class="green">luatex</span></td>
  </tr>
  <tr>
    <td>metapost (niche parametrized PostScript vector art macros)</td>
    <td><span class="red">metapost</span></td>
  </tr><tr>
    <td><em class="grey">probably png-related stuff...? maybe tikz needs it? idk</em></td>
    <td><span class="green">png</span></td>
  </tr>
  <tr>
    <td>more PostScript stuff</td>
    <td><span class="red">pstricks</span></td>
  </tr>
<tr>
    <td>goodies such as <em class="grey">apa7</em></td>
    <td><span class="red">publishers</span></td>
  </tr>
  <tr>
    <td>must-haves like
      <em class="grey">amsmath</em>,
      <em class="grey">amssymb</em>,
      <em class="grey">physics</em></td>
    <td><span class="green">science</span></td>
  </tr>
  <tr>
    <td>latex to html conversion (but I use pandoc-bin)</td>
    <td><span class="red">tex4ht</span></td>
  </tr>
  <tr>
    <td>more latex to html conversion (...pandoc-bin)</td>
    <td><span class="red">texi2html</span></td>
  </tr>
  <tr>
    <td><em class="grey">what situations make you need special freetype fonts? idk</em></td>
    <td><span class="red">truetype</span></td>
  </tr>
  <tr>
    <td>XeTeX (as opposed to LuaTeX or pdflatex)</td>
    <td><span class="red">xetex</span></td>
  </tr>
  <tr>
    <td><span class="grey">idk when we use xml files, it hasn't come up</span></td>
    <td><span class="red">xml</span></td>
  </tr>
</table>

```bash path=/etc/portage/package.use/math
app-text/texlive extra graphics luatex png science
```

## 2. R

A lot of R packages fail to compile under LTO.

```bash path=/etc/portage/package.use/math
dev-lang/R cairo perl png # cairo+png for image gen support
sys-libs/zlib minizip
```

```bash path=/etc/portage/package.env/no-lto
dev-lang/R no-lto
```

If you want to skip compiling, you can run R in a nix-shell,
though graphics will be an enormous hassle. See [nix ⇗](/conf/nix).

## 3. Octave

Simple <em class="blue">USE</em> flags for the
open-source equivalent to MATLAB.

```bash path=/etc/portage/package.use/math
sci-mathematics/octave gnuplot imagemagick
```
