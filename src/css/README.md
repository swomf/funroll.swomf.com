# css

The following files are concatenated into
`main.css` at build time.

## monospace.css

`00monospace.css` is based on the
[Monospace Web](https://owickstrom.github.io/the-monospace-web/),
and contains css mainly for HTML tags.

* Changes from upstream
  * `<pre><code>` blocks have borders (see src/md2html)
  * Remove ligatures by using JetBrains Mono NL
  * Grandchildren of `<ol>` that are `<li>` do not increase the `<li>` counter
  * Force dark mode
  * Add STIX as fallback for math symbols (though, STIX is not monospace)

## additions.css

`01additions.css` contains css mainly for classes,
such as colors, manual overrides, and other utility classes.

If you use these for your own literate programming setup,
edit them to your liking, as the current
configuration is specific to `funroll.swomf.com`.
