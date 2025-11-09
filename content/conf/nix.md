# nix

I like the File System Hierarchy (FHS), but I also enjoy
Nix flakes.

<span class="bright">Note.</span> This "like" is more than just a useless partiality.
I collaborate on projects that assume FHS —— for example,
projects that use an _install.sh_ —— meaning that pure NixOS
may get in the way.

The FHS thing may be workaroundable
with containers,
or testing via QEMU,
or via <span class="purple">patchelf</span>
or <span class="purple">steam-run</span>
or any of the hacks NixOS uses to get by.
Negatives notwithstanding, I have reason to believe
NixOS may be my future, final distro.

Until then, I use the nix-guix overlay rather than a intra-user installation.

```bash path=/etc/portage/repos.conf/nix-guix.conf
[nix-guix]
location = /var/db/repos/nix-guix
sync-type = git
sync-uri = https://github.com/trofi/nix-guix-gentoo.git
```

```bash path=/etc/portage/package.accept_keywords/overlays
*/*::nix-guix ~amd64
```

And I use flakes, since they lock better.
I didn't bother to read about its controversy.

(nix.conf was configured [as per the Nix wiki ⇗](https://nixos.wiki/wiki/flakes#:~:text=/etc/nix/nix.conf))

```bash path=/etc/nix/nix.conf
experimental-features = nix-command flakes
```

```bash path=/etc/portage/package.use/nix
dev-libs/boehm-gc cxx
virtual/libcrypt static-libs
sys-libs/libxcrypt static-libs
```

```bash path=/etc/portage/package.accept_keywords/nix
dev-libs/editline ~amd64
dev-libs/libcpuid ~amd64
dev-cpp/toml11 ~amd64
dev-util/cargo-c ~amd64 # avoids slot conflict in 2025-04-30
                        # because libgit2 should be version 1.9
```
