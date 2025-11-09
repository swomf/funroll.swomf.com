# browser

I use firefox-bin and mullvad-browser-bin instead of compiling,
since upstream's optimizations are so well-architected that I can't hope to
match their maintenance.

The only thing I change is
<span class="red">removing a downstream Gentoo patch</span>
that made Mullvad have library issues at one point. Unfortunate.

```bash path=/etc/portage/package.use/browser
www-client/mullvad-browser-bin -X
```
