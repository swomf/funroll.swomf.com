# make.conf

Below, I set up my make.conf. Lots of stuff going on in there.

1. <em class="grey">compiler flags</em>: native CPU compilation, link-time optimization, mold linker
2. <em class="grey">global <em class="blue">USE</em> flags</em>: copying desktop flags
3. <em class="grey">nicer compiles</em>: setting up niceness and MAKEOPTS
4. <em class="grey">secure boot</em>: this section needs updating...
5. <em class="grey">other</em>
6. <em class="grey">auto-generated</em>: machine-built part of make.conf, such
as mirrors or cpuid2cpuflags

## 1. compiler flags

I use resolve-march-native, LTO, and the mold linker.

My machine has an `13th Gen Intel(R) Core(TM) i9-13900H (12+8)`. Sometimes
when updating sys-devel/gcc I get a

<pre><code><span class="red"> * </span>Different values of l1-cache-size detected!
<span class="red"> * </span>GCC will fail to bootstrap when comparing files with these flags.</code></pre>

due to my mix of power and efficiency cores.
([Bug 915389 ⇗](https://bugs.gentoo.org/915389))

So as Gentoo recommends, I use the output of
app-misc/resolve-march-native for CPU_FLAGS

```bash path=/etc/portage/make.conf
# CPU_FLAGS has no special meaning;
# I merely include it in COMMON_FLAGS later
CPU_FLAGS=" -march=alderlake -mabm -mno-cldemote \
-mno-kl -mno-sgx -mno-widekl -mshstk \
--param=l1-cache-line-size=64 --param=l1-cache-size=48 \
--param=l2-cache-size=24576 "
```

<div class="ml-3">
  <span class="bright">Aside.</span> Supposedly app-misc/resolve-march-native
  is meant to specify architecture-specific transformations in place of
  <em class="nowrap">-march=native</em>.

  But why does its output differ so wildly from

  <pre><code><span class="purple">gcc</span> -march=native -E -v - < /dev/null 2>&1 \
    | grep cc1 \
    | grep -o -- '- .*' \
    | cut -d\  -f2-</code></pre>

  which should probably be the same?
</div>

Next, add LTO [(Gentoo Wiki ⇗)](https://wiki.gentoo.org/wiki/LTO#Enabling_LTO_System-wide)
to save runtime RAM. Mix it with the mold linker, a drop-in replacement
for ld. (Supposedly. Now and then causes compile bugs on btrfs-progs: [Mold issue 1509 ⇗](https://github.com/rui314/mold/issues/1509))

```bash path=/etc/portage/make.conf
# c compiler flags
WARNING_FLAGS="-Werror=odr -Werror=lto-type-mismatch -Werror=strict-aliasing"
COMMON_FLAGS="-O2 -pipe -flto ${CPU_FLAGS} ${WARNING_FLAGS}"
CFLAGS="${COMMON_FLAGS}"
CXXFLAGS="${COMMON_FLAGS}"
FCFLAGS="${COMMON_FLAGS}"
FFLAGS="${COMMON_FLAGS}"

LDFLAGS="${LDFLAGS} -fuse-ld=mold"

# lang-specific flags
RUSTFLAGS="-C target-cpu=native" # rust is O3 by default
CGO_CFLAGS="${COMMON_FLAGS}"
CGO_CXXFLAGS="${COMMON_FLAGS}"
CGO_FFLAGS="${COMMON_FLAGS}"
CGO_LDFLAGS="${LDFLAGS}"

# USE FLAGS
USE="lto"
```

```bash path=/etc/portage/env/sys-fs/btrfs-progs
# chop off -fuse-ld=mold
# see https://github.com/rui314/mold/issues/1509
LDFLAGS="${LDFLAGS//-fuse-ld=mold/}"
```

I also recommend, when a package breaks with LTO (last time for me it was GIMP),
to use a package.env to temporarily disable it for a specific package.

```bash path=/etc/portage/env/no-lto
# These warnings were promoted to errors as they indicate likely runtime
# problems with LTO. Additively disable them since LTO is being removed.
WARNING_FLAGS="-Wno-error=odr -Wno-error=lto-type-mismatch -Wno-error=strict-aliasing"

CFLAGS="${CFLAGS} -fno-lto"
CXXFLAGS="${CXXFLAGS} -fno-lto"
FCFLAGS="${FCFLAGS} -fno-lto"
FFLAGS="${FFLAGS} -fno-lto"

USE="${USE} -lto"
```

## 2. global use flags

Most of my <em class="blue">USE</em> flags are automatically
inherited from my profile (amd 23.0 hardened, which is not a desktop profile).

Due to that, I need to add typical desktop stuff to global <em class="blue">USE</em>:

* <em class="grey">dbus</em>. Desktops use inter-process communication all the time.
* <em class="grey">X</em> and <em class="grey">wayland</em> to add gui support to many apps.
* <em class="grey">elogind</em> and <em class="grey">-systemd</em>. I use an OpenRC system, so this is [recommended ⇗](https://wiki.gentoo.org/wiki/Elogind).
* <em class="grey">pulseaudio</em> and <em class="grey">pipewire</em>. See [sound ⟹](/conf/sound) to understand these.
* <em class="grey">modules-sign</em>. It's somewhat helpful in secure boot <span class="grey">probably</span>.
* <em class="grey">introspection</em>. Lots of apps depend on other apps to have
GTK introspection as a <em class="blue">USE</em> flag, so it's too much effort
NOT to make it global.

```bash path=/etc/portage/make.conf
USE="${USE} dbus X wayland elogind -systemd pulseaudio pipewire modules-sign introspection"
```

## 3. nicer compiles

Often when I compile, I keep working. Then I don't want the CPU to give
Portage all of its priority —— especially if I have other apps running.

```bash path=/etc/portage/make.conf
# Extremely low priority
PORTAGE_SCHEDULING_POLICY="idle"
# Lowest priority
PORTAGE_NICENESS="19"
PORTAGE_IONICE_COMMAND="ionice -c 3 -p \${PID}"

# For parallelism, typically
#   min(half of ram in GB, half of cores)
# is recommended.
MAKEOPTS="-j8" 
```

Of course, if I WANT my CPU to work harder, I might run something like this:

<pre><code><span class="magenta command"></span><span class="purple">MAKEOPTS=-j12 sudo -E emerge</span> -uDNav @world --keep-going</code></pre>

## 4. secure boot

<strong class="red">TODO:</strong> I'm not even sure I need this for manual kernel installs.

<strong class="red">TODO:</strong> I'm not sure this calls to the sbctl keys correctly anyway.

```bash path=/etc/portage/make.conf
USE="${USE} secureboot"
MODULES_SIGN_KEY=/var/lib/sbctl/keys/db/db.key
MODULES_SIGN_CERT=/var/lib/sbctl/keys/db/db.pem
SECUREBOOT_SIGN_KEY=/var/lib/sbctl/keys/db/db.key
SECUREBOOT_SIGN_CERT=/var/lib/sbctl/keys/db/db.pem
```

## 5. other

```bash path=/etc/portage/make.conf
# VIDEO_CARDS="intel nvidia" # "deprecated" or whatever.
ACCEPT_LICENSE="*" # I'm not trying to run Trisquel lol

# for use with app-portage/elogv
PORTAGE_ELOG_SYSTEM="save"
PORTAGE_ELOG_CLASSES="warn error info log qa"


# binpkg stuff
FEATURES="${FEATURES} binpkg-request-signature"
```

## 6. auto-generated

```bash path=/etc/portage/make.conf
# NOTE: This stage was built with the bindist USE flag enabled

# This sets the language of build output to English.
# Please keep this setting intact when reporting bugs.
LC_MESSAGES=C.utf8

# auto-generated by mirrorselect
GENTOO_MIRRORS="https://mirror.clarkson.edu/gentoo/ \
    https://mirrors.mit.edu/gentoo-distfiles/ \
    https://gentoo.osuosl.org/ \
    https://mirrors.rit.edu/gentoo/ \
    rsync://mirrors.rit.edu/gentoo/ \
    https://mirror.servaxnet.com/gentoo/"
```

From <span class="purple">cpuid2cpuflags</span>:

```bash path=/etc/portage/package.use/00cpu-flags
*/* CPU_FLAGS_X86: aes avx avx2 avx_vnni bmi1 bmi2 f16c fma3 mmx mmxext pclmul popcnt rdrand sha sse sse2 sse3 sse4_1 sse4_2 ssse3 vpclmulqdq
```

<div class="ml-3">
  <span class="bright">Note:</span>
  cpuid2cpuflags DOES get updated.
  With each update, Gentoo's <em>USE_EXPAND</em> flags
  will more closely match your CPU capabilities, so I recommend rerunning this now and then.

  For example, the last time I reran this I got the new CPU flags
  <em class="grey">avx_vnni, bmi1, bmi2</em>.
</div>
