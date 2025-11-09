<style>
/* variables defined in monospace.css */
.conf-list td {
border: none;
}
.conf-list td:first-child {
text-align: right;
border-right: var(--border-thickness) solid var(--text-color);
white-space: nowrap;
width: 20ch;
}
.conf-list td:last-child {
text-align: left;
}
</style>
# all my confs

Welcome to the jungle. I'll be succinct.

My configuration topics are sorted into:

1. Useful — has a patch I wrote, is information-dense
2. Okay-ish — half is plumbing, is information-sparse
3. Obvious chores — obvious, almost boring plumbing

Within these categories, articles
are merely alphabetized.

## useful

Probably decent reads. Includes patches
and DIYing.

<table class="conf-list">
  <tr>
  <tr>
    <td><a href="/conf/art">art ⇒</a></td>
    <td>I stop you from disabling QoL features for no good reason,
      and explain how ImageMagick, GIMP, and Krita
      <em class="blue">USE</em> flags compare with Arch Linux PKGBUILDs.
    </td>
  </tr>
  <td><a href="/conf/books">books ⇒</a></td>
  <td>I write a darkmode patch for Foliate,
    an epub reader.
  </td>
  </tr>
  <tr>
    <td><a href="/conf/boot">boot ⇒</a></td>
    <td>How I boot from a unified kernel image.
      Covers the "why", and links to the "how".</td>
  </tr>
  <tr>
    <td><a href="/conf/graphics">graphics ⇒</a></td>
    <td>I install custom completions for prime-run
      via Portage install hook. Otherwise, I do boring flag plumbing.</td>
  </tr>
</table>


## okay-ish

Though there are some useful parts,
these articles are mostly plumbing,
such as adding obvious <em class="blue">USE</em>
flags or enabling unstable versions via _~amd64_.
Often short.

<table class="conf-list">
  <tr>
    <td><a href="/conf/citrix">citrix ⇒</a></td>
    <td>I discuss the Gentoo Package Manager Specification and
      its effects on Citrix.</td>
  </tr>
  <tr>
    <td><a href="/conf/desktop">desktop ⇒</a></td>
    <td>I patch a keybind change into swappy, a
      screenshot editor.
      <br />
      The rest is an exposition
      on my desktop setup, which is usually only
      interesting to the author.</td>
  </tr>
  <tr>
    <td><a href="/conf/gaijin">gaijin ⇒</a></td>
    <td>I set up Japanese input support via fcitx5+mozc, disable its LTO,
      and fix garbled characters on Japanese zipfile unzipping.</td>
  </tr>
  <tr>
    <td><a href="/conf/jumbo-builds">jumbo builds ⇒</a></td>
    <td>I try to be clever about getbinpkg, but fail
      due to bug 337456.</td>
  </tr>
  <tr>
    <td><a href="/conf/kernel">kernel ⇒</a></td>
    <td>I give pseudocode for a kernel chore and discuss
    the "subtractive" approach to kernel configuration.</td>
  </tr>
  <tr>
    <td><a href="/conf/make.conf">make.conf ⇒</a></td>
    <td>I workaround a mold linker bug and set up compile-time niceness.
      Other than that, I set up march=native, LTO, global <em class="blue">USE</em>
      and MAKEOPTS uninterestingly.
      <em class="red">Unfinished article.</em></td>
  </tr>
  <tr>
    <td><a href="/conf/math">math ⇒</a></td>
    <td>I explain TeXLive LaTeX flags, then fix R compilation by disabling LTO.</td>
  </tr>
  <tr>
    <td><a href="/conf/music">music ⇒</a></td>
    <td>I briefly celebrate the ebuild report I made about MuseScore
      that was fixed in 4.6.3.</td>
  </tr>
  <tr>
    <td><a href="/conf/nix">nix ⇒</a></td>
    <td>I mutter in the vague direction of NixOS (while still using systemwide
      nix-shell and flakes via the nix-guix overlay).
      Mostly <em class="blue">USE</em> flag plumbing.</td>
  </tr>
  <tr>
    <td><a href="/conf/nsxiv">nsxiv ⇒</a></td>
    <td>I patch the nsxiv image viewer to have a transparent
      background, with minor extra edits.</td>
  </tr>
  <tr>
    <td><a href="/conf/overlay-funroll">overlay-funroll ⇒</a></td>
    <td>I set up my custom overlay,
      <em class="purple">overlay-funroll</em>.
      <em class="red">Unfinished article.</em>
    </td>
  </tr>
  <tr>
    <td><a href="/conf/reading">reading ⇒</a></td>
    <td>I briefly patch default dark mode into app-text/evince.</td>
  </tr>
<tr>
    <td><a href="/conf/sound">sound ⇒</a></td>
    <td>I explain pipewire-pulse via a Gentoo NEWS item, compile easyeffects,
    then add an install hook to fix the deep-filter ladspa's shared object file
    for use with easyeffects.</td>
  </tr>
<tr>
    <td><a href="/conf/video">video ⇒</a></td>
    <td><em class="red">Unfinished article.</em></td>
  </tr>
<tr>
    <td><a href="/conf/zswap">zswap ⇒</a></td>
    <td>Define and implement zswap.</td>
  </tr>
</table>

## obvious chores

Articles that are worth less, but
<em class="purple">funroll</em> has to
compile code blocks from <em>somewhere</em>.

<table class="conf-list">
  <tr>
    <td><a href="/conf/binrepos.conf">binrepos.conf ⇒</a></td>
    <td>I use a non-hardened binary host harmlessly on a hardened x86-64-v3 system.</td>
  </tr>
  <tr>
    <td><a href="/conf/browser">browser ⇒</a></td>
    <td>I avoid the X <em class="blue">USE</em> flag
      on Mullvad Browser to avoid Gentoo-specific library issues. </td>
  </tr>
  <tr>
    <td><a href="/conf/clipboard">clipboard ⇒</a></td>
    <td>I set up obvious Tesseract <em class="blue">USE</em>
      flags for image-to-text recognition.</td>
  </tr>
  <tr>
    <td><a href="/conf/debug">debug ⇒</a></td>
    <td>I follow the Gentoo Wiki to add valgrind support to glibc.</td>
  </tr>
  <tr>
    <td><a href="/conf/editor">editor ⇒</a></td>
    <td>I enable obvious <em class="blue">USE</em> flags for LazyVim.</td>
  </tr>
  <tr>
    <td><a href="/conf/firmware">firmware ⇒</a></td>
    <td>(unmaintained) I prod at a worthless use of Gentoo's
      <em class="blue">savedconfig</em> feature.</td>
  </tr>
  <tr>
    <td><a href="/conf/games">games ⇒</a></td>
    <td>I plumb at nonsense with Wine. Then I enable kernel splitlock.
      Worth a look if you're having trouble, but rather boring. <em class="red">Unfinished article.</em></td>
  </tr>
  <tr>
    <td><a href="/conf/gestures">gestures ⇒</a></td>
    <td>Empty :(</td>
  </tr>
  <tr>
    <td><a href="/conf/gnome-deps">gnome-deps ⇒</a></td>
    <td>Boring flag plumbing.</td>
  </tr>
  <tr>
    <td><a href="/conf/office">office ⇒</a></td>
    <td>Boring flag plumbing.</td>
  </tr>
  <tr>
    <td><a href="/conf/programming-languages">program langs ⇒</a></td>
    <td>Boring flag plumbing. <em class="red">Unfinished article.</em></td>
  </tr>
  <tr>
    <td><a href="/conf/random-libs">random-libs ⇒</a></td>
    <td>Boring flag plumbing for miscellaneous libs.</td>
  </tr>
<tr>
    <td><a href="/conf/shell">shell ⇒</a></td>
    <td>I briefly set up the dash shell and plug my zsh theme.</td>
  </tr>
  <tr>
    <td><a href="/conf/theme">theme ⇒</a></td>
    <td>I add nerdfonts and kvantum qt5.
      <em class="red">Unfinished article.</em></td>
  </tr>
  <tr>
    <td><a href="/conf/tor">tor</a></td>
    <td>I setup torbrowser-launcher::torbrowser in three lines.</td>
  </tr>
</table>
