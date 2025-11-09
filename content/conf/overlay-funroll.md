# overlay-funroll

I run a local overlay called [funroll ⇗](https://github.com/swomf/overlay-funroll).

Below I'll explain how I...

1. use it.
2. made it.
3. actively develop it (sccache, metagen, pycargoebuild...).

## how I use it

Instead of syncing from Git, I sync from my own machine. It's a weird intermediary
method I find convenient for developing.

```bash path=/etc/portage/repos.conf/funroll.conf
[funroll]
location = /var/db/repos/funroll
sync-type = git
sync-uri = /home/user/git/overlay-funroll
```

"Gentoo overlays must mark themselves with ~amd64" blah blah blah blah

```bash path=/etc/portage/package.accept_keywords/overlays
*/*::guru ~amd64
*/*::funroll ~amd64
*/*::gentoo-zh
*/*::benzene-overlay
```

## how I made it

Though the
[wiki describes making your own overlay ⇗](https://wiki.gentoo.org/wiki/Creating_an_ebuild_repository),
I actually used a different method versus the wiki.

That is, I made an ebuild skeleton git repo on my local user then
synced from that.

<pre><code><span class="grey"># Prepare a git repository</span>
<span class="magenta command"></span><span class="purple">mkdir</span> overlay-funroll
<span class="magenta command"></span><span class="purple">cd</span> overlay-funroll
<span class="magenta command"></span><span class="purple">git</span> init

<span class="grey"># Manually set up repo skeleton stuff</span>
<span class="magenta command"></span><span class="purple">mkdir</span> metadata profiles
<span class="magenta command"></span><span class="purple">echo</span> funroll > profiles/repo_name
<span class="magenta command"></span><span class="purple">echo</span> 8 > profiles/eapi
<span class="magenta command"></span><span class="purple">cat</span> >> metadata/layout.conf << <span class="magenta">EOF</span>
<span class="grey">masters = gentoo
thin-manifests = true
sign-manifests = false</span>
<span class="magenta">EOF</span>

<span class="grey"># say hi to the world</span>
<span class="magenta command"></span><span class="purple">nvim</span> README.md

<span class="grey"># save changes</span>
<span class="magenta command"></span><span class="purple">git</span> add --all
<span class="magenta command"></span><span class="purple">git</span> commit -m init
</code></pre>

## how I develop it

<strong class="red">TODO:</strong> Finish writing this section

https://devmanual.gentoo.org/general-concepts/manifest/

https://devmanual.gentoo.org/ebuild-writing/variables/index.html

https://devmanual.gentoo.org/ebuild-writing/functions/

pkgdev manifest -m 

Ebuild devs need stuff like pkgcheck, cargo license, pycargoebuild, metagen, blah blah



```bash path=/etc/portage/package.accept_keywords/ebuild-dev
dev-util/cargo-license ~amd64
```

I work with Rust often so I need a way to cache stuff lol

I use [dev-util/sccache](https://wiki.gentoo.org/wiki/Sccache)

```bash path=/etc/portage/package.accept_keywords/ebuild-dev
dev-util/sccache ~amd64
```

```bash path=/etc/sandbox.d/20sccache
# Allow write access to sccache cache directory
SANDBOX_WRITE="/var/cache/sccache/"
```

```bash path=/etc/portage/env/sccache-enabled
# when you're working on some kind of
#     `ebuild abc/defg merge`
# you will need to add it to package.env :)
RUSTC_WRAPPER=/usr/bin/sccache
SCCACHE_DIR=/var/cache/sccache
SCCACHE_MAX_FRAME_LENGTH=104857600
```
