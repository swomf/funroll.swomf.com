# firmware

<div class="ml-3">
  <span class="bright">Notice.</span> Unmaintained.
  <p class="m-0">Currently my system uses
    <em class="nowrap grey">/etc/portage/savedconfig/sys-kernel/linux-firmware-YYYYMMDD</em>
    as its source of truth, and I couldn't care less about removing anything.
    The only difference I can tell that removing firmware does, is that
    it reduces the amount of modules I sign when I rebuild my kernel.</p>
</div>

<hr />

Regarding firmware, I:

* Didn't remove i915 from linux-firmware because dmesg says my CPU's iGPU needs at least one of it.

This is kind of a not-so-useful feature I'm only prodding at because
it's easy in Gentoo.

```bash
[  +0.002039] i915 0000:00:02.0: [drm] Finished loading DMC firmware i915/adlp_dmc.bin (v2.20)
[  +0.009797] Loading firmware: i915/adlp_guc_70.bin
[  +0.003092] Loading firmware: i915/tgl_huc.bin
```

<p class="red">CONFIG FILE REMOVED BECAUSE IT'S TOO LONG ANYWAY</p>
