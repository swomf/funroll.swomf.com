# zswap

Below, I install more ram (lol).

## what is zswap?

What's the RAM lifecycle? Where does zswap come in?

1. A program runs.
2. It sends memory to RAM —— random access memory.
3. Some parts of RAM aren't used that often, or it's getting
full.
4. Memory is <em>evicted</em> from RAM into the next "swap"
pool.

So swap is like backup RAM elsewhere, a slower but extra
storage unit. (On Windows, it's called a pagefile.)

Swap is usually on the disk. (NVMEs are better than SSDs here.)
But sometimes... swap can be in RAM itself. Not only that,
but swap can be stored in a compressed manner. That means
that you can have 4G of swap inside of RAM, compressed to 2-3x
its size —— how many gigs is that?

Storing swap in RAM is called zswap.

## how to implement zswap?

Typically zswap is configured using either
kernel parameters or a service.

Since my kernel parameters aren't really editable (UKI+secure boot), I use an OpenRC
service —— specifically, I just tell the `local` service that I have
[a script ⇗](https://wiki.gentoo.org/wiki/Zswap#Alternative:_using_local.d) to run on service start.

```bash path=/etc/local.d/50-zswap.start
# configure zswap
echo zstd > /sys/module/zswap/parameters/compressor
echo 1 > /sys/module/zswap/parameters/enabled
# echo 20 > /sys/module/zswap/parameters/max_pool_percent # default
# echo zbud > /sys/module/zswap/parameters/zpool # why is this a default? TODO learn
```

Note that zswap needs to be backed by a swapfile. See the
[BTRFS docs ⇗](https://btrfs.readthedocs.io/en/latest/Swapfile.html).

<pre><code><span class="magenta command"></span><span class="purple">btrfs</span> filesystem mkswapfile --size 2G /swapfile</code></pre>
