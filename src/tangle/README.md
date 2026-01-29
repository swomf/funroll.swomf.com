# tangle

Homegrown C to rip the code blocks out of our .md files,
and put them in appropriate Portage directories.

The term "tangle" comes from
[literate programming](https://en.wikipedia.org/wiki/Literate_programming)
(paradigm).

## bugs

(1) Triple backticks are expected.

(2) Also,

````
```bash EMPTY/some/path
text
```
````

will display /some/path as the path tagging the code block, but will
not tangle to that path. This is useful behavior and may be kept.
(It occurs because EMPTY is precisely 5 characters.)
