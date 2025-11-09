# jumbo-builds

Sometimes we have a massive build.
Massive builds make Arch kids laugh at us,
so let's run them sparingly.

## 1. Minimizing recompiles

Suppose you compile a software, change a <em class="blue">USE</em> flag,
then recompile again. Rebuilding the same <em>.o</em> files wastes time.

To cache <em>.o</em> files, you might think of using ccache due to its
Portage integration.

This is <em class="red">old</em>.

Use sccache instead, discussed in a [footnote on the wiki's ccache page ⇗](https://wiki.gentoo.org/wiki/Ccache#See_also). I find this more relevant to
rebuilds of local .ebuild stuff so I discuss it in
[overlay-funroll ⇒](/conf/overlay-funroll) instead.

## 2. getbinpkg

Binary caches are a curious thing.

1. On the user side, Arch Linux is binary first, PKGBUILD later.
2. On the user side, NixOS is "technically" source first, binary later. However
Cachix is so massive that most of your packages are going to be binary downloads,
given that the same package-build-hash you're looking for is available.
3. On the user side, Gentoo is source first, binary much later. You typically
only manually override source builds; many even disrecommend it.

I use binary packages if I'm in a time crunch or if building from source
doesn't work for some reason. However Gentoo binary packages are often
unavailable for your <em class="blue">USE</em> flag list; it's easonable since
the amount of builds to cache would be upper-bounded by two-factorial
and lower-bounded by "typically-changed <em class="blue">USE</em> flag"-factorial.

However, the clever of us run into a sad bug: [Bug 337456 ⇗](https://bugs.gentoo.org/337456).

Observe:

```bash path=/etc/portage/env/getbinpkg
FEATURES="${FEATURES} getbinpkg"
```

```bash path=/etc/portage/package.use/jumbo-builds
net-libs/webkit-gtk keyring lcms pdf spell
media-libs/gst-plugins-base opengl #dep
```

```bash path=/etc/portage/package.env/jumbo-builds
net-libs/webkit-gtk getbinpkg
```

In the above conf files, one would expect for FEATURES=getbinpkg
to be only conditionally enabled for webkit-gtk. But on
attempted install, webkit-gtk will still only be built from source.

That's a Gentoo bug since 2010.

<div class="ml-3">
  <span class="bright">Preemption.</span>
  "Fix it yourself." Thanks smartass, but there'd be a lot of liaising to
  do first, since eclass architecting necessitates caution.
  (Just an excuse, since I haven't gotten to it yet, nor
  has anyone else who ran into bug 337456.)
</div>

## 3. surrender

Therefore the best thing to do is to

<pre><code><span class="magenta command">emerge </span>--oneshot --getbinpkg -av </code><em class="red">$some_jumbo_pkg</em></pre>

every time a @world update asks to upgrade such a package.
