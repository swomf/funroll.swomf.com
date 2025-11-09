# binrepos.conf

Despite being on the amd64/23.0/hardened profile, my x86-64-v3
binhost isn't hardened. As of 2025-11-09 this
has caused me no issues despite possibly being bad practice.

<div class="ml-3">
  <span class="bright">Specifics:</span>
  PAX, a kernel/user memory security thing, used to make Gentoo Hardened have
  myriad build edits compared to normal Gentoo.

  PAX is <a href="https://wiki.gentoo.org/wiki/Project:Hardened" target="_blank">deprecated ⇗</a>.

  Therefore mixing binhosts will probably be fine.
</div>


I also add binpkg-request-signature for key verification (see the [wiki ⇗](https://wiki.gentoo.org/wiki/Gentoo_Binary_Host_Quickstart#Package_signature_verification
)).


```bash path=/etc/portage/binrepos.conf/gentoobinhost.conf
[gentoobinhost]
priority = 1
sync-uri = https://distfiles.gentoo.org/releases/amd64/binpackages/23.0/x86-64-v3
```

```bash fakepath=/etc/portage/make.conf
/etc/portage/make.conf
# From binrepos.conf.md
FEATURES="${FEATURES} binpkg-request-signature"
```

There was one network issue I had to work around.

After removing <span class="nowrap grey">/etc/portage/gnupg</span> and running

<div class="italics ml-3"><span class="purple">sudo</span> getuto</div>

my WiFi
hung after primary openPGP keys were generated. I discovered that
my WiFi was blocking the hkps:// protocol, so I had to use mobile data
to run the <span class="purple">getuto</span> part.
