# tor

I use the www-client/torbrowser-launcher::torbrowser package.

Just run

<pre><code><span class="magenta command"></span><span class="purple">sudo eselect</span> repository enable torbrowser</code></pre>

then add

```bash path=/etc/portage/package.accept_keywords/overlays
*/*::torbrowser ~amd64
```

```bash path=/etc/portage/package.use/tor
app-crypt/gpgme python # required by torbrowser-launcher
```

Consider reading <em class="grey">/var/db/repos/torbrowser/Readme.md</em>
