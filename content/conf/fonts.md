# fonts

Gentoo's fonts felt clunky-ish to me compared to other distros.

Below, I set up:
1. plumbing media-fonts/nerdfonts to use the sourcecodepro variant
2. a fix for "⚠" rendering as a thin bitmap in Firefox
3. a fix for "→" rendering too small in Firefox

Surprisingly 2 and 3 were different bugs; 2 was Gentoo,
and 3 was my font being super meh.

## 1. plumbing

```bash path=/etc/portage/package.use/nerdfonts
media-fonts/nerdfonts sourcecodepro
# media-fonts/noto cjk # this USE flag was deleted after 20250701
                       # machines with 20250601 may still see the USE flag in eix
```

## 2. "⚠" was a thin bitmap

Firefox drew "⚠" as a tiny pixelly bitmap glyph. I assumed I was missing a
Noto font bc Misc Fixed was being used as fallback chain (F12 inspector,
fonts debug).
Nah it was because the fontconfig rule that rejects bitmap fonts
wasn't actually running.

At one point I had `70-no-bitmaps.conf` enabled, maybe by a package
or something, but Gentoo upstream migrated to
`70-no-bitmaps-except-emoji.conf`
(see media-libs/fontconfig/fontconfig-2.18.2.ebuild line 205 ish;
upstream caution is Clobber Nothing which works usually, just not now).
But the include is relative, optional, the target only exists in `conf.avail`,
and so a useless rule failed without telling me anything pretty much.

Not my fault.

If this is you, enable the real one and rebuild the cache:

<pre><code><span class="magenta command"></span><span class="purple">eselect</span> fontconfig disable 70-no-bitmaps.conf
<span class="magenta command"></span><span class="purple">eselect</span> fontconfig enable 70-no-bitmaps-except-emoji.conf
<span class="magenta command"></span><span class="purple">fc-cache</span> -fv
</code></pre>

Why would a file be selected but useless lol? :|

## 3. "→" was too slender for my eyes

Not fallback related lmao. It's because Firefox was rendering "→" with
Liberation Sans, which it picked up through GTK because nwg-look had
exported the default _Cantarell 11_ - a font I don't have installed,
which is also why nwg-look showed me an empty font field. But that's not why
I got screwed here.

Liberation Sans indeed has U+2192, so fallback never ran.
The arrow is just tiny. So I set _Nimbus Sans Regular_ in nwg-look.
I chose this over Arial Bold (corefonts; weird UI handling cuz it's old)
and Arimo (which Liberation Sans literally derived from lol, it
had the same garbage arrow).

## Cool aside on Mullvad Browser

www-client/mullvad-browser isolates its rendering with bundled fonts and
its own fontconfig. It has other stuff bundled like STIX Math and the like,
instead of using system fonts (fingerprint reasons).
I used it as reference to see why characters were screwy.
