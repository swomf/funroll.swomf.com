# General test

This file tests common use cases.

```javascript
In backticked blocks, specify a path=/etc/portage/package.use/kernel
to declare a config file.
```

```bash path=/etc/portage/package.use/kernel
1 In terms of RAM, (line 1)
```


```bash date=2025-01-27 path=/etc/portage/package.accept_keywords/kernel
                  (line 1)
2 We have no RAM. (line 2)
```

```nix path=/etc/configuration.nix
3 (line 1)
```
```bash date=test path=/etc/portage/package.use/kernel
4 More kernel stuff (line 1)
                    (line 2)
```
