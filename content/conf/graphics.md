# graphics

Below, I:

1. Set up Nvidia cards
   1. Define global VIDEO_CARDS
   2. Install Nvidia drivers
   3. Perform secure Nvidia kernel object signing
2. Tell Portage to add custom <span class="purple">prime-run</span>
completions via install hook. Telling Portage reduces config drift.
3. Complain about CUDA's Docker support then give up on it.

## 1. Nvidia cards

With the following probe into my system:

<code><pre>
<span class="magenta command"></span><span class="purple">lspci</span> | <span class="purple">grep</span> -E 'VGA|3D'
0000:00:02.0 VGA compatible controller: Intel Corporation Raptor Lake-P [Iris Xe Graphics] (rev 04)
0000:01:00.0 VGA compatible controller: NVIDIA Corporation GA107BM / GN20-P0-R-K2 [GeForce RTX 3050 6GB Laptop GPU] (rev a1)
</pre></code>

I append the VIDEO_CARDS

```bash path=/etc/portage/package.use/00video_cards
# USE_EXPAND for my GPUs. for some reason we need
# -* or else amdgpu will appear on there? dumb but thats
# what the wiki wants us to do nowadays so whatevah.
*/* VIDEO_CARDS: -* intel nvidia
# vdpau and nvenc are just general
# "yeah support nvidia speedups thanks" USE flags.
*/* vdpau nvenc
# sometimes deptrees look weird when
# i exclude these USE flags
*/* vaapi vulkan opengl
```

<div class="ml-3">
<span class="bright">Note:</span> The wiki doesn't say to globally enable
  OpenGL. However, since packages have differing OpenGL
  defaults, sometimes having non-global OpenGL makes a package require
  <em>-opengl</em> on one of its dependencies. Annoying!
</div>

Then I install the following drivers, along with logging/testing software.

* <em class="grey">media-libs/libva-intel-media-driver</em>.
  * Log: <em class="grey">x11-apps/igt-gpu-tools</em>
* <em class="grey">x11-drivers/nvidia-drivers</em>
  * Log: <em class="grey">sys-process/nvtop</em>. <span class="purple">nvidia-smi</span> is bundled w/ drivers.
* We <em>don't</em> need to explicitly install <span class="yellow">media-libs/mesa</span>. It's installed as a dependency.
  * Test: <em class="grey">x11-apps/mesa-progs</em> (for <span class="purple">glxinfo</span>)

See [Gentoo: Intel ⇗](https://wiki.gentoo.org/wiki/Intel) and the VAAPI section. Also disable Nouveau and do other chores —— see
[Gentoo: Nvidia ⇗](https://wiki.gentoo.org/wiki/NVIDIA/nvidia-drivers#Manually_Compiled_Kernel)

```bash path=/etc/portage/package.use/graphics
x11-apps/igt-gpu-tools man
app-arch/xz-utils abi_x86_32 # required by igt-gpu-tools
*/* modules-sign             # as per gentoo: nvidia wiki
```

```bash path=/etc/portage/package.accept_keywords/graphics
x11-apps/igt-gpu-tools ~amd64
x11-drivers/nvidia-drivers ~amd64
# mesa deps:
gui-libs/egl-x11 ~amd64
gui-libs/eglexternalplatform ~amd64
```

After installing nvidia-drivers, if you have secure boot enabled
[like I do ⇗](/conf/kernel), you'll need to sign your Nvidia kernel objects.
For a manually compiled kernel, I do the following:

<pre><code><span class="magenta command"></span><span class="purple">./scripts/sign-file</span> sha512 \
  certs/signing_key.pem \
  certs/signing_key.x509 \
  /lib/modules/6.12.31-gentoo/video/nvidia-uvm.ko
<span class="grey"># with all of the .ko objects</span>.
<span class="grey"># You may need to do </span><span class="purple">modprobe</span> nvidia <span class="grey">afterward.</span>
</code></pre>

## 2. prime-run install hook

As for running programs with nvidia, I use <span class="purple">prime-run</span>
since it's the simplest. But it doesn't come with completions despite
being obvious to add; hence, I write my own completions
then keep track of it via Portage.

<div class="ml-3">
  <span class="bright">Note:</span>
  On other Linux distributions,
  you'd either need to write these to /usr manually (forgettable)
  or install them locally into ~/.local (needs to be done for every user).

  Thus this is merely a minor convenience.
</div>

prime-run is EAPI=7 which prevents me from inheriting the
shell-completion eclass, the "good practice" way to implement
the following. But it's painfully simple anyway. Makes
me wonder what the point of said eclass is. Without further ado:

```bash path=/etc/portage/env/x11-misc/prime-run
post_src_compile() {
  mkdir -p "${S}" || die
  cat << INSTALL_HOOK_EOF >> "${S}/zshcomp" || die
#compdef prime-run

_prime_run() {
  shift words
  (( CURRENT-=1 ))
  _normal
}

_prime_run
INSTALL_HOOK_EOF
  cat << INSTALL_HOOK_EOF >> "${S}/bashcomp" || die
complete -F _command prime-run
INSTALL_HOOK_EOF
}

post_src_install() {
  # We cannot inherit shell-completion in this ebuild due to
  # readonly EAPI=7 so we act out that eclass manually.
  insopts -m 0644
  insinto "${EPREFIX}/usr/share/zsh/site-functions/"
  newins "${S}/zshcomp" "_${PN}"
  insinto "${EPREFIX}/usr/share/bash-completion/completions"
  newins "${S}/bashcomp" "${PN}"
}
```

## 3. cuda

I also use Nvidia CUDA. However, I cannot get it working in docker
containers as of 2025-11-01 due to ABI issues. I
assume it's because cuda 13 container support is recent (Aug 2025) and
may not be supported in-tree.

```bash path=/etc/portage/package.accept_keywords/graphics
# cuda toolkit
dev-util/nvidia-cuda-toolkit ~amd64
```

```bash path=/etc/portage/env/no-mold
# chop off -fuse-ld=mold. See GentooWiki: mold
LDFLAGS="-Wl,-O1 -Wl,--as-needed"
```

```bash path=/etc/portage/package.env/cuda
app-containers/nvidia-container-toolkit no-mold
sys-libs/libnvidia-container no-mold
```

(I also had to run 'sudo nvidia-ctk runtime configure --runtime=docker' for Docker
at one point...)

## summary

Nvidia :(
