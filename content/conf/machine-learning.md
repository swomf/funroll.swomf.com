# machine learning

I have 16GB ram and 6GB VRAM. Pretty much worthless. This article is about plumbing
llama-cpp's flags anyway (boring).

(Yeah I use it over ollama since it's more finer-grained.)

As for my llama-cpp <em class="blue">USE</em> flags:

- I don't enable cuda globally since some packages consider it
to mean "I want to compile GPU apps with the nvidia CUDA toolkit",
which pulls in dev-util/nvidia-cuda-toolkit. Pass
- openmp, supposedly it does parallelization help but for cpu stuff, so why not
- Building with only cuda and openmp got me
  ```
  0.00.205.178 E get_repo_commit: error: HTTPS is not supported. Please rebuild with one of:
    -DLLAMA_BUILD_BORINGSSL=ON
    -DLLAMA_BUILD_LIBRESSL=ON
    -DLLAMA_OPENSSL=ON (default, requires OpenSSL dev files installed)
  ```
  when I install from huggingface, so I add openssl.

```bash path=/etc/portage/package.use/ml
sci-misc/llama-cpp cuda openmp openssl
```

Example use of llama-cpp:

<pre><code><span class="magenta command"></span><span class="purple">llama-server</span> -hf ggml-org/gemma-3-4b-it-GGUF \
  --ctx-size 8192 --device CUDA0
</code></pre>

i.e. I have a hybrid GPU laptop.

## conclusion

Should I talk about imagegen instead? I have a simple but interesting setup
to make it run with nervous amount of "creativity". Not Gentoo-specific though.

Maybe that's better for my normal blog.
