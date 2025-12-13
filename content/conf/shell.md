# shell

I use zsh with some extra stuff, like the [netmask zsh theme ⇗](https://github.com/netmask-zsh-theme), the only perfect ohmyzsh theme.

However, for startup scripts, I use dash shell. Gentoo
devs recommend —— citation needed —— that
Bashisms be avoided in startup scripts.

Meanwhile, Dash is a POSIX shell meant for startup scripts.

It falls together.

```bash path=/etc/portage/package.use/shell
app-alternatives/sh -bash dash
```

```bash path=/etc/portage/package.accept_keywords/shell
app-shells/zoxide ~amd64
app-shells/fish           # decent completions for sudo
app-shells/ohmyzsh **     # for my perfect theme
```
