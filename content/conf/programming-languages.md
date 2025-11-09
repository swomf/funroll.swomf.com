# programming languages

Here's an irony of using Gentoo:

* On other distributions (Arch), you can get started on
random programming languages with a simple install.
* On Gentoo, it's the same... expect you "often have to" compile. CRINGE.

So I use *-bin packages for programming languages where i can, to
avoid compiling.

Still, to me, the disadvantages of needing to compile are outweighed
by the advantages of needing to compile. For example, if I want to learn
some build system or praxis, I can grep through ebuilds on my machine
instead of letting ChatGPT hallucinate.

Whatever

```bash path=/etc/portage/package.accept_keywords/programming-languages
# cpp (manpages)
app-doc/cppman ~amd64
# haskell
dev-haskell/*
dev-lang/ghc  # glasgow compiler
# NOTE | I don't use the ::haskell repo, I keep that local.
#      | haskell repo takes too long to eix-update lmao
# zig
app-eselect/eselect-zig
dev-lang/zig-bin
# godot
dev-games/godot ~amd64 # i dont use dotnet since
                       # csharp doesn't export to html5
```

```bash path=/etc/portage/package.use/programming-languages
# python
dev-lang/python tk # for some legacy tkinter software
# rust
dev-lang/rust-bin clippy doc prefix rust-analyzer rust-src rustfmt
# node
# i wish i could replace npm calls with pnpm...
net-libs/nodejs npm
```

<div class="grey">
<strong class="red">TODO: </strong>
cover and explain graphite polyhedral loop optimization, POLLY, etc.

maybe move it to a different file?

https://wiki.gentoo.org/wiki/GCC_optimization#Graphite

```bash path=/etc/portage/package.use/gcc
sys-devel/gcc graphite
```

</div>
