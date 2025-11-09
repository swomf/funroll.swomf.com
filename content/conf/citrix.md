# citrix

Citrix is one example of an advantage Arch Linux has over Gentoo.

On Gentoo, <span class="red">Citrix needs two (2) interventions:</span>

(1) Citrix is marked unstable on ~amd64 and needs to be
<em class="blue">ACCEPT_KEYWORD</em>-ed.

```bash path=/etc/portage/package.accept_keywords/remote
net-misc/icaclient ~amd64
```

(2) Citrix is PKG_FETCH restricted. This means you need to
download Citrix manually, in your browser, into your DISTFILES
directory (/var/cache/distfiles/). Observe this excerpt from
the [ebuild source ⇗](https://github.com/gentoo/gentoo/blob/79b5f80bb72cb4976d96421d816d205e2838725b/net-misc/icaclient/icaclient-25.05.0.44.ebuild#L112).

```bash
pkg_nofetch() {
	elog "Download the client file ${A} from
	https://www.citrix.com/downloads/workspace-app/"
	elog "and place it into your DISTDIR directory."
}
```

It's not even a clean download since
Citrix gives nondescript names
to their icaclient tarballs, like <em class="grey">linuxx64-25.08.0.88.tar.gz</em>.

On Arch Linux, <span class="green">the fetch step needs zero interventions.</span>

From the [aur/icaclient PKGBUILD ⇗](https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=icaclient):

```bash
url='https://www.citrix.com/downloads/workspace-app/linux/workspace-app-for-linux-latest.html'
# ...
_dl_urls_="$(curl -sL "$url" | grep -F ".tar.gz?__gda__")"
_dl_urls="$(echo "$_dl_urls_" | grep -F "$pkgver.tar.gz?__gda__")"
_source64=https:"$(echo "$_dl_urls" | sed -En 's|^.*rel="(//.*/linuxx64-[^"]*)".*$|\1|p')"
_sourceaarch64=https:"$(echo "$_dl_urls" | sed -En 's|^.*rel="(//.*/linuxarm64-[^"]*)".*$|\1|p')"
source=('citrix-configmgr.desktop'
        'citrix-conncenter.desktop'
        'citrix-wfica.desktop'
        'citrix-workspace.desktop'
        'wfica.sh'
        'wfica_assoc.sh'
        'ctxcwalogd.service'
        'ctxusbd.service')
source_x86_64=("$pkgname-x64-$pkgver.tar.gz::$_source64")
source_aarch64=("$pkgname-arm64-$pkgver.tar.gz::$_sourceaarch64")
```

All the finagling is done automatically.

<p class="red">Why doesn't Gentoo catch up?</p>

The package manager is simply not designed this way.

Not only
is SRC_URI expected to be declared statically (for metadata and caching purposes),
but if you tried to run curl or sed inside SRC_URI, it would violate
the Gentoo Package Manager Specification.

> [PMS §12.3 "Ebuild-specific commands" ⇗](https://dev.gentoo.org/~ulm/pms/head/pms.html#ebuildspecific-commands)
>
> The following commands will always be available in the ebuild environment, provided by the package manager.
> Except where otherwise noted, they may be internal (shell functions or aliases) or external commands available in PATH;
> where this is not specified, ebuilds may not rely upon either behaviour.
> Unless otherwise specified, <span class="red">it is an error if an ebuild calls any of these commands in global scope.</span>

This rule is implemented in [ebuild.sh ⇗](https://github.com/gentoo/portage/blob/63287ed70ff2fc7c6f0d97e168c30d148d054d5d/bin/ebuild.sh#L107):

```bash
command_not_found_handle() {
  die "External commands disallowed while sourcing ebuild: ${*}"
}
```

This is due to a differing implementation of security. For example, when using an unknown PKGBUILD on Arch,
pacman wrappers such as paru print out the file for you to audit. This is
less ergonomic on Gentoo since ebuild trust isn't as differentiated.

Furthermore, the icaclient PKGBUILD is updated frequently, since pkgver has to match
the actual tarball stored at Citrix's workspace-app-for-linux-latest.html download page.

Assuming that the page is guaranteed, however, a script like the following could work

```bash
PV="25.05.0.44"
HOMEPAGE="https://www.citrix.com/downloads/workspace-app/legacy-workspace-app-for-linux/workspace-app-for-linux-latest14.html"
SRC_URI="$(curl -sL "${HOMEPAGE}" \
| grep -F ".tar.gz?__gda__" \
| grep -F "${PV}.tar.gz?__gda__" \
| sed -En 's|^.*rel="(//.*/linuxx64-[^"]*)".*$|https:\1|p')"
```

and would create a SRC_URI in the format

<span class="grey">https://downloads.citrix.com/25013/linuxx64-25.05.0.44.tar.gz?\_\_gda\_\_=exp=<em class="purple">\$some_unix_expiration_time</em>\~acl=/*\~hmac=<em class="purple">\$some_hash_based_auth_code</em></span>

<span class="bright">Takeaways:</span>

Use the script just above this section to avoid the browser. You'll need to know
in advance whether your <em class="blue">\$PV</em> is indeed stored at that <em class="blue">\$HOMEPAGE</em>, though.
