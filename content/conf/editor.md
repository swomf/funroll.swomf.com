# editor

There is nothing interesting nor Gentoo-specific below.

I use VSCode occasionally, but I tend to have
many projects open at once, so I use Neovim+LazyVim.

For LazyVim:

```bash path=/etc/portage/package.accept_keywords/vim
dev-vcs/lazygit ~amd64
```

For neovim:

```bash path=/etc/portage/package.use/vim
dev-lua/lpeg lua_targets_lua5-4
```
