# booting

I have a mildly complicated boot setup
that loops around to being simple.

<figure>
<pre>
                                sbctl secure boot
┌──────────────┐  ┌<em class="yellow">/dev/sda1─►/efi</em>───────────────┐ ╭       ╮
│              │  │ 2. Unified Kernel Image      │ │made   │
│1. Just simple│  │ (no intermediate bootloader) │ │with   │
│laptop UEFI   ├─►│┌───────┐  ┌─────────────────┐│ │dracut.│
│(OEM default!)│  ││efistub├─►│tweaked initramfs││ ╰       ╯
│              │  │└───────┘  └─────────────────┘├─┐
└──────────────┘  └──────────────────────────────┘ │ 
                                                   │ password.
┌<em class="red">/dev/sda2─►/dev/mapper/cryptroot</em>───────────────┐  │ mounts then
│ 3. LUKS2 encryption (argon2id algorithm)      │  │ switches
│┌<em class="blue">/dev/mapper/cryptroot─►various</em>───────────────┐│  │ root
││ 4. BTRFS partition (flat subvolume layout)  ├╯  │
││Subvolume         Mount location             │◄──┘
││@           ────► /                          ├╮
││@home       ────► /home                      ││
││@var_log    ────► /var/log                   ││
││@snapshots  ────► /.snapshots                ││
│└─────────────────────────────────────────────┘│
└───────────────────────────────────────────────┘
</pre>
</figure>

There was a gotcha I had to write a Dracut module for, but this setup
is otherwise a typical product of dodging an intermediate bootloader.

Below, I'll discuss each step of the startup diagram in order
(as opposed to the chronologically-ordered setup commands).
In that sense, this is less of a guide (since I merely
link to the guides); rather, it's a catalogue of the
choices I made and why.

In fact, in terms of chronology:

1. I formatted space for the
EFI partition and LUKS2, then set up the BtrFS partition
and subvolumes inside the LUKS2 partition, installed Gentoo,
then set up a Unified Kernel Image with gentoo-kernel-bin
and Dracut.
2. After booting successfully once, I set up
secure boot within the UEFI, UKI, and sbctl-signed the
EFI app.
3. Some time later, I switched from gentoo-kernel-bin to
gentoo-sources.

# Boot process

## 1. UEFI

My ASUS laptop's
[UEFI firmware ⇗](https://wiki.archlinux.org/title/Arch_boot_process#UEFI)
happens to be standards-compliant. Hence, the UEFI can boot
straight into the kernel via efistub, as long as the UEFI
keeps a boot entry for it (discussed in §2).

<div class="ml-3">
  <span class="bright">Note:</span>
  The Gentoo wiki
  <a target="_blank" href="https://wiki.gentoo.org/wiki/Unified_kernel_image#Automated_EFI_stub_booting)">warns ⇗</a>
  that not all motherboards are fully UEFI-compliant. Too bad: they'll need to
  chainload an "intermediate bootloader" (GRUB2, systemd-boot, etc.) instead.
</div>

The UEFI also lets me proudly use secure boot.  
Why use secure boot? It's my answer to this question:

<blockquote>
  <span class="m-0">
    The EFI System Partition (ESP)
    <a href="https://wiki.archlinux.org/title/EFI_system_partition#Format_the_partition"><em>cannot</em> be encrypted ⇗</a>.</span>
    <span class="m-0">
      Then, how do you boot from an unencrypted ESP's boot application
      into an encrypted root, <em>safely</em>*?</span>
  <small class="m-0">
    *according to your threat model</small>
</blockquote>

My answer to this question evolved over time. (Skip if you don't care
about the thought process of protecting an encrypted root.)

<div id="lmao" style="display: flex; justify-content: space-between; width: calc(round(down, 100%, 1ch));">old ◀──<span style="text-align: right;">──► new</span></div>

<table>
  <thead>
    <tr>
      <th>Unencrypted <span class="nowrap">/boot,</span> normal bootloader</th>
      <th>Unencrypted ESP GRUB2 loads argon2id-encrypted <span class="nowrap">/boot</span> kernel</th>
      <th>Secure boot with Unified Kernel Image (UKI)</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td style="vertical-align: middle;">
        A <a href="https://twopointfouristan.wordpress.com/" target="_blank">2.4istan ⇗</a> attack is <em class="red">easy.</em></td>
      <td>
        A determined attacker able to 2.4istan will
        <span class="red">probably still mess with the EFI application</span>
        in
        <span class="nowrap">/efi.</span>
        No one would ever know.</td>
      <td>
        The kernel has no "personal information", so
        <span class="green">a good integrity-check is better than hiddenness.</span>
        (Someone <em>can</em> still
        <a href="https://en.wikipedia.org/wiki/Nonvolatile_BIOS_memory#:~:text=UEFI%20settings%20are%20still%20lost%20if%20the%20CMOS%20battery%20fails" target="_blank">remove the CMOS ⇗</a>
        though)</td>
    </tr>
    <tr>
      <td style="vertical-align: middle;">Easy/lazy setup</td>
      <td>
        <a href="https://wiki.archlinux.org/title/Dm-crypt/Device_encryption#Configuring_the_kernel_parameters" target="_blank">Decryption needs to be run twice ⇗</a>:
        <ol style="margin-bottom: 0;">
          <li>
            GRUB2 <span class="red">decrypts</span> the partition holding <span class="nowrap">/boot</span></li>
          <li>
            GRUB2 loads <span class="nowrap">/boot's</span> kernel and initramfs, then passes away</li>
          <li>
            The kernel <span class="red">decrypts</span> the root partition (even if <span class="nowrap">/boot</span> was already on it) then does mount+switch_root</li>
        </ol>
      </td>
      <td>
        UKI with secure boot <span class="green">dodges needing:</span>
        <ul class="m-0">
          <li>
            Multiple decryption (boots faster!)
          </li>
          <li>
            <a href="https://leo3418.github.io/collections/gentoo-config-luks2-grub-systemd/packages.html#grub-212)" target="_blank">the Steinhardt GRUB2 argon2id patch ⇗</a>.
            No intermediate bootloader supports argon2id
          </li>
          <li>
            <a href="https://wiki.archlinux.org/title/Dm-crypt/Encrypting_an_entire_system#Preparing_the_disk_6" target="_blank">
            Using pbkdf2 instead of argon2id ⇗</a>
            for disk encryption
          </li>
        </ul>
      </td>
    </tr>
  </tbody>
</table>

To do so, I use app-crypt/[sbctl ⇗](https://github.com/Foxboron/sbctl?tab=readme-ov-file#key-creation-and-enrollment)
for managing my secure boot keys.

<div class="ml-3">
  <p class="m-0">Why sbctl?</p>
  <ul>
    <li>
      It's more distro-agnostic than the
      <a href="https://wiki.gentoo.org/wiki/Secure_Boot#Installation" target="_blank">secureboot USE flag ⇗</a></li>
    <li>
      It's less manual than <a href="https://www.rodsbooks.com/efi-bootloaders/controlling-sb.html#creatingkeys" target="_blank">Roderick W. Smith's method ⇗</a>
      <ul><li>
        The
        <a href="https://github.com/Foxboron/sbctl/tree/master/contrib" target="_blank">install hook ⇗</a>
        exists but I still sbctl-sign manually.</li></ul></li>
  </ul>
  <p class="m-0">What keys do I use?</p>
  <ul>
    <li>
      Self-signed (as expected)</li>
    <li>Microsoft defaults
      <ul><li>
        My laptop has discrete Nvidia and no TPM2 eventlog;
        I don't want to risk
        <a href="https://github.com/Foxboron/sbctl/wiki/FAQ#option-rom" target="_blank">bricking anything ⇗</a>.
        </li></ul></li>
  </ul>
</div>

And finally, the UEFI menu is password protected.  
(Imagine enabling secure boot but letting the F2 key dodge it.)

<p><span class="bright">Takeaways:</span>
At any time convenient to you (e.g. before or after your first successful boot):</p>

<ol>
  <li>Add a UEFI password</li>
  <li>
    Put UEFI Secure Boot into
    <a href="https://wiki.gentoo.org/wiki/Secure_Boot#Installing_the_keys" target="_blank">setup mode ⇗</a>,
    then boot</li>
  <li>Compile app-crypt/sbctl</li>
  <li>
    Follow the sbctl
    <a href="https://github.com/foxboron/sbctl?tab=readme-ov-file#key-creation-and-enrollment" target="_blank">
      README ⇗</a>
    and manpage. If /efi is unfindable, set the ESP_PATH (<a href="https://github.com/Foxboron/sbctl/issues/207#issuecomment-1652239359" target="_blank">issues/207 ⇗</a>)</li>
</ol>

## 2. Unified Kernel Image

In a [normal boot setup ⇗](https://wiki.archlinux.org/title/Arch_boot_process#Feature_comparison):

<style>
</style>


<ol style="margin-left: 1ch;">
  <li>
    The UEFI firmware performs:
    <ul>
      <li>
        <strong class="yellow">finding:</strong>
        A boot entry must be set in the UEFI firmware menu
        if the EFI application's path isn't
        <a href="https://www.linuxfromscratch.org/blfs/view/svn/postlfs/grub-setup.html#:~:text=EFI/BOOT/BOOTX64.EFI" target="_blank" class="nowrap">/EFI/BOOT/BOOTX64.EFI ⇗</a></li>
      <li>
      <strong class="blue">verifying:</strong>
        only occurs if secure boot is enabled.</li>
      <li>
        <strong class="magenta">loading: </strong>
        The UEFI passes the baton to the EFI application ——
        typically some "intermediate bootloader" that lets
        the user pick the kernel or operating system they want to run for
        the next boot step.
    </ul>
  </li>
  <li>
    The bootloader finds and loads the kernel (e.g.
    <a href="https://en.wikipedia.org/wiki/vmlinuz" target="_blank">vmlinuz ⇗</a>)
    and attaches the
    <a href="https://sourcemage.org/HowTo/initramfs" target="_blank">initramfs ⇗</a>
    (a small ram rootfs; has <span class="nowrap">/dev,</span> <span class="nowrap">/bin,</span> etc.)</li>
  <li>
    The kernel prepares hardware modules; the ram rootfs
    does fsck and <em>mounts partitions</em></li>
  <li>
    The rootfs is
    <a href="https://github.com/brgl/busybox/blob/abbf17abccbf832365d9acf1c280369ba7d5f8b2/util-linux/switch_root.c#L54" target="_blank">freed ⇗</a>
    after <a href="https://sourcemage.org/HowTo/initramfs#:~:text=switch_root" target="_blank">switch_root ⇗</a></li>
</ol>

With a Unified Kernel Image (UKI), all of that is <em>bundled.</em> Instead of
a separate some_bootloader.efi, vmlinuz, and an initramfs, we have
one file, e.g. <span class="nowrap">/efi/EFI/Linux/hash-1.2.3-gentoo-dist.efi</span>.

On my system:

<ol style="margin-left: 1ch;">
  <li>
    The UEFI firmware runs through:
    <ul>
      <li>
        <strong class="yellow">finding: </strong>
        sys-boot/uefi-mkconfig
        <a href="https://github.com/projg2/installkernel-gentoo/blob/05e2ba01182c1e8edb5211ddedb31e693868bde6/hooks/95-efistub-uefi-mkconfig.install#L32" target="_blank">autogenerates ⇗</a>
        my boot entries</li>
      <li><strong class="blue">verifying: </strong>
        thanks to sbctl</li>
      <li><strong class="magenta">loading: </strong>
        The UEFI only "knows" how to run the UKI since
        <a href="https://wiki.archlinux.org/title/EFI_boot_stub" target="_blank">efistub ⇗</a>
        turned the bundle into an EFI application
        <ul>
          <li><strong>Disadvantage:</strong> Secure boot + UKI =
            I can't change boot params. DON'T CARE</li>
        </ul>
      </li>
    </ul>
  </li>
  <li>
    efistub (not an intermediate <a href="https://wiki.archlinux.org/title/EFI_boot_stub#Using_UEFI_directly" target="_blank">bootloader ⇗</a>) loads
  the kernel and initramfs kept within the UKI. My UKI is made with<ol class="incremental" type="1">
      <li>Dracut (choice discussed later)</li>
      <li><span class="strike">gentoo-kernel-bin</span>
        gentoo-sources (discussed in <a href="/conf/kernel">kernel ⟹</a>)</li>
      <li>installkernel</li>
    </ol></li>
  <li>
    The kernel prepares hardware modules; the kernel and initramfs
    decrypt my LUKS2 partition (discussed in §3) and mounts the subvolumes of the
    BTRFS partition within that partition onto 
    <span class="nowrap">/system_root</span>
    (discussed in §4).</li>
  <li>
    After a default Dracut module (a shellscript) asserts that
    <span class="nowrap">/system_root</span>
    looks sane, it runs `switch_root system_root`.</li>
</ol>

<span class="bright">Takeaways:</span>

As a [consequence ⇗](https://github.com/gentoo/gentoo/blob/master/sys-kernel/installkernel/installkernel-50.ebuild#L48)
of Dracut, I need systemd-utils*. Otherwise, follow the
[Gentoo wiki ⇗](https://wiki.gentoo.org/wiki/Unified_kernel_image#Systemd_kernel-install), but
install uefi-mkconfig instead of kernel-bootcfg.
Then reinstall the Linux kernel, ensuring it is
sbctl-signed and uefi-mkconfigged. 

<small>
*Alternatively, you could probably hand-roll your own UKI
  to dodge the Dracut/kernel-install dep.</small>

```bash path=/etc/portage/package.use/kernel
sys-kernel/installkernel uki efistub dracut
sys-apps/systemd-utils boot kernel-install
```

```bash path=/etc/portage/package.accept_keywords/uefi-mkconfig
sys-boot/uefi-mkconfig ~amd64
```

## 3. LUKS2 ENCRYPTION

I use Linux Unified Key Setup (LUKS2) to wrap my
BTRFS drive (discussed in §4) with encryption.
For safety, I [back up ⇗](https://gitlab.com/cryptsetup/cryptsetup/-/wikis/FrequentlyAskedQuestions#6-backup-and-data-recovery)
the LUKS header offsite.

<div class="ml-3">
  <span class="bright">Refresher: What's LUKS?</span> Skip if familiar.

  /dev/sda is a block device, a special file that gives you access to
  some hardware "device" like an NVMe drive.
  <a href="https://wiki.archlinux.org/title/Device_file#Block_devices" target="_blank">Arch Wiki ⇗</a>

  Anything in /dev/mapper is a "virtual" block device -- it's
  a kind of middleman between you and the actual hardware block device.
  This is handled by the kernel "device mapper" subsystem.
  <a href="https://en.wikipedia.org/wiki/Device_mapper#:~:text=Device%20mapper%20works%20by%20passing%20data" target="_blank">Wikipedia ⇗</a>

  When you run <em>cryptsetup</em>, you call to the
  <a href="https://gitlab.com/cryptsetup/cryptsetup/-/wikis/DMCrypt" target="_blank">dm-crypt ⇗</a>
  system, which uses the kernel "device mapper" and kernel crypto API to:

  <ol class="ol-override">
    <li class="ol-li-override"><em>luksFormat</em>:
    Put a header (the LUKS header) onto the block device to
    describe some metadata.
    <a href="https://wiki.archlinux.org/title/Data-at-rest_encryption#:~:text=additional%20convenience%20layer" target="_blank">ArchWiki ⇗</a></li>
    <li class="ol-li-override"><em>luksOpen</em>:
    Decrypt the block device (via password or keyfile), then
      map it to some virtual block device in /dev/mapper.</li>
  </ol>

  This facilitates transparent (live) encryption:
  
<figure><pre>
    ╭────────────╮               ╭───────────╮
    │   mapped   │               │  mounted  │
    ┴            ▼               ┴           ▼
/dev/sda2      /dev/mapper/something       /home/somebody
    ▲            ┬               ▲           ┬       ▲
    │write actual│ translate msg │msg sent to│       │
    │crypted data│ w/ encryption │ middleman │    You write
    ╰────────────╯               ╰───────────╯    to a file
</pre></figure>
  </div>

In order to boot with this setup, the initramfs must know:

<ol>
  <li>that we need to decrypt and map
    <span class="nowrap">/dev/sda2</span> into
    <span class="nowrap">/dev/mapper/cryptroot</span></li>
  <li>where to mount
    <span class="nowrap">/dev/mapper/cryptroot's</span>
    volumes into</li>
</ol>

Since I use Dracut for the initramfs, I used the
Dracut section for disk encryption in the
[Gentoo wiki ⇗](https://wiki.gentoo.org/wiki/Full_Disk_Encryption_From_Scratch#Dracut).
I also added `rd.luks.name`
(to mount as <span class="nowrap">/dev/mapper/cryptroot</span>)
and `rd.luks=1` for convenience. (See the
[Arch wiki ⇗](https://wiki.archlinux.org/title/Dracut#LVM_/_software_RAID_/_LUKS)
and [dracut(8) manpage ⇗](https://man.archlinux.org/man/dracut.8.en#:~:text=rd.luks=).)

<span class="bright">Takeaways:</span>

```bash path=/etc/dracut.conf
# part 1: enable cryptsetup
add_dracutmodules+=" dm crypt "
kernel_cmdline+=" rd.luks.uuid=ad5e66e7-e890-4565-95f1-37f27600c8d4 \
rd.luks.name=ad5e66e7-e890-4565-95f1-37f27600c8d4=cryptroot \
root=UUID=310bf892-743d-4e57-b48d-4ccaa4265416 rd.luks=1 "
```

<pre><code><span style="color:#6A737D;"># Don't copy my UUIDs in the /etc/dracut.conf kernel command line!
# Find and specify your own for <em>rd.luks.uuid=<span class="red">X</span></em> and <em>root=UUID=<span class="blue">X</span></em></span>
<span class="magenta command"></span><span class="purple">sudo</span> lsblk -o name,fstype,uuid,mountpoints
NAME          FSTYPE      UUID                                 MOUNTPOINTS
sda
├─sda1        vfat        5085-8014                            /efi
└─sda2        crypto_LUKS <em class="red">ad5e66e7-e890-4565-95f1-37f27600c8d4</em>
  └─cryptroot btrfs       <em class="blue">310bf892-743d-4e57-b48d-4ccaa4265416</em>
</code></pre>

## 4. BTRFS

BetterFS is a fantastic filesystem for desktops.

<table>
  <thead>
    <tr>
      <th></th>
      <th colspan="2">Why BtrFS is better (■-■¬)</th>
      <th>Why BtrFS is worse (◞‸◟；)</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td style="vertical-align: middle;">ext4</td>
      <td rowspan="2">BtrFS supports
        <a target="_blank" href="https://btrfs.readthedocs.io/en/latest/Compression.html">transparent compression ⇗</a> often doubling de facto disk space and reducing writes.
        <a target="_blank" href="https://www.phoronix.com/review/btrfs-zstd-compress/2">Benchmarks ⇗</a> <p>(compression is my primary use case)</p></td>
      <td rowspan="2">BtrFS subvolumes make incremental snapshotting a cakewalk.
        <a target="_blank" href="https://btrfs.readthedocs.io/en/latest/Subvolumes.html">BtrFS docs ⇗</a>
<p><span class="red">Though,</span> actual backups are more danger-resistant.</p>
    </td>
      <td>ext4 is super battletested (<span class="red">though,</span> Facebook uses BtrFS.
        <a target="_blank" href="https://lwn.net/Articles/824855/">article ⇗</a>)</td>
    </tr>
    <tr>
      <td style="vertical-align: middle;">xfs</td>
      <td>xfs is more for
        <a target="_blank" href="https://wiki.archlinux.org/title/XFS#:~:text=proficient%20at%20parallel%20IO">parallel workloads at scale ⇗</a>
        so idk.</td>
    </tr>
    <tr>
      <td>zfs</td>
      <td>BtrFS uses
        <a target="_blank" href="https://en.wikipedia.org/wiki/ZFS#Deduplication">less RAM ⇗</a> (my ZFS dealbreaker)</td>
      <td>BtrFS is in-tree and easier to set up</td>
      <td>idk. ask a freeBSD user</td>
    </tr>
  </tbody>
</table>

<p class="mb-0">
  I use a "flat" subvolume layout.
  <a target="_blank" href="https://archive.kernel.org/oldwiki/btrfs.wiki.kernel.org/index.php/SysadminGuide.html#Layout">Sysadmin guide ⇗</a>.
</p>

As an example, here's an approximation of how my BtrFS setup
was made. It doesn't include the earlier steps, i.e. cryptsetup, mkfs.btrfs,
and emergence of relevant packages such as btrfs-progs:

<pre><code><span class="grey"># Something I find odd: you need to mount the main
  btrfs filesystem <em>before</em> making subvols —— instead of on
  the device, you make subvols on the mountpoint. <em>e.g.</em></span>
<span class="purple">mount</span> -t btrfs /dev/mapper/<em class="red">$cryptdevice</em> \
  --mkdir /mnt

<span class="purple">btrfs</span> subvolume create /mnt/@
<span class="purple">btrfs</span> sub c /mnt/@home
<span class="purple">btrfs</span> sub c /mnt/@var_log
<span class="purple">btrfs</span> sub c /mnt/@snapshots
<span class="purple">umount</span> /mnt

<span class="grey"># I use <a
      target="_blank"
      href="https://opensource.com/article/20/6/linux-noatime#a-bit-about-file-timestamps"
      class="!grey">noatime ⇗</a> to stop disk writes that
  I don't care about (i.e. access time writes)</span>
<span class="purple">mount</span> -t btrfs -o \
  noatime,compress=zstd,subvol=@ \
  /dev/mapper/<em class="red">$cryptdevice</em> \
  --mkdir /mnt
<span class="purple">mount</span> -t btrfs -o \
  noatime,compress=zstd,subvol=@home \
  /dev/mapper/<em class="red">$cryptdevice</em>
  --mkdir /mnt/home
<span class="purple">mount</span> -t btrfs -o \
  noatime,compress=zstd,subvol=@var_log \
  /dev/mapper/cryptroot --mkdir /mnt/var/log
<span class="purple">mount</span> -t btrfs -o \
  noatime,compress=zstd,subvol=@snapshots \
  /dev/mapper/<em class="red">$cryptdevice</em>
  --mkdir /mnt/.snapshots

<span class="grey"># Boot sector</span>
<span class="purple">mount</span> /dev/<em class="yellow">$sda1_probably</em> --mkdir /mnt/efi
</code></pre>

Unfortunately, due to the flat subvolume layout shown above, Dracut doesn't think that
<em>/system_root</em> appears sane during boot time.

I haven't run into this before, but I trial-and-errored in the sources
and wrote my own Dracut module to fix it. (Dracut modules are just
shell scripts with four or so expected functions.) And of course, to
keep it managed by my package manager, I added it as a Portage patch
(see below).

<span class="bright">Takeaways:</span>

A flat BtrFS subvolume layout might need finagling:

```bash path=/etc/dracut.conf
add_dracutmodules+=" actually-normal-fstab "
use_fstab="yes"
add_fstab+=" /etc/fstab "
```


```diff path=/etc/portage/patches/sys-kernel/dracut/actually-normal-fstab-module.patch
diff --git a/modules.d/99actually-normal-fstab/module-setup.sh b/modules.d/99actually-normal-fstab/module-setup.sh
new file mode 100755
index 00000000..a9607e19
--- /dev/null
+++ b/modules.d/99actually-normal-fstab/module-setup.sh
@@ -0,0 +1,15 @@
+#!/bin/sh
+
+# called by dracut
+check() {
+    return 0
+}
+
+# called by dracut
+depends() {
+    echo fs-lib
+}
+
+install() {
+    inst_hook mount 99 "$moddir/mount-normal-fstab.sh"
+}
diff --git a/modules.d/99actually-normal-fstab/mount-normal-fstab.sh b/modules.d/99actually-normal-fstab/mount-normal-fstab.sh
new file mode 100755
index 00000000..8b6358f5
--- /dev/null
+++ b/modules.d/99actually-normal-fstab/mount-normal-fstab.sh
@@ -0,0 +1,34 @@
+#!/bin/sh
+
+set -x
+
+type getarg > /dev/null 2>&1 || . /lib/dracut-lib.sh
+type det_fs > /dev/null 2>&1 || . /lib/fs-lib.sh
+
+fstab_mount() {
+    test -e "$1" || return 1
+    info "Mounting from $1"
+    # Don't use --target-prefix since it will fail to mount '/' (already 'mounted')
+
+    sed -e '/\t\/efi\S*/d' -e '/btrfs defaults 0 0/d' -e 's/\t\//\t\/sysroot\//' "$1" > /tmp/fstab
+    mount --all --fstab /tmp/fstab
+    return 0
+}
+
+# systemd will mount and run fsck from /etc/fstab and we don't want to
+# run into a race condition.
+if [ -z "$DRACUT_SYSTEMD" ]; then
+    [ -f /etc/fstab ] && fstab_mount /etc/fstab
+fi
+
+# prefer $NEWROOT/etc/fstab.sys over local /etc/fstab.sys
+if [ -f "$NEWROOT"/etc/fstab ]; then
+    fstab_mount "$NEWROOT"/etc/fstab
+elif [ -f "$NEWROOT"/\@/etc/fstab ]; then
+    # in case of btrfs flat volumes, where root is a "@" subvol
+    fstab_mount "$NEWROOT"/\@/etc/fstab
+elif [ -f /etc/fstab ]; then
+    fstab_mount /etc/fstab
+fi
+
+set +x
```

## Conclusion

I hope this isn't a <span class="red">Gentoo-specific</span>
problem that let me use <span class="red">Gentoo-specific tools</span>
to solve a <span class="red">Gentoo-specific problem.</span>

Maybe I should have used
<a target="_blank" href="">µgRD ⇗</a> for boot setup instead?
It's specifically designed for Gentoo users while also being reasonably cross-distribution.

Anyway, that's my Unified Kernel Image-based secure boot setup. You can
do this on any distro, really. Unless your motherboard is wack.

See also: [kernel ⟹](/conf/kernel)
