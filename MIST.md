# The Media Engine Safe Task library

I'm happy to share this with you today.

The me-safe-task library uses new ways to trigger custom tasks on the Media Engine, with the goal of not disrupting Syscalls from the main CPU to the Media Engine.

2 methods were first added. They involve patching the Media Engine core, which is based in kernel memory at address `0x88300000`.


## Method 1 — Syscall handler redirection

The first one patches the Media Engine Syscall handler, which is responsible for calling core functions from an entry index on the Syscall table. The idea was simply to redirect to a custom function when the index is invalid. However, it requires the additional patch of a function loaded and launched from the main CPU, triggering a cache update. The ideal candidate was `sceAudiocodecGetEDRAM`, whose call from the me-wrapper side, which dispatches the Syscall and triggers a soft interrupt toward the ME, uses the SoC hardware registers to perform a memory flush, ensuring that the written patches are visible. It then invokes `meCoreGetMeEdram` on the ME side, which was patched to execute an ICache invalidation, so that the patched Syscall handler and interrupt handler are properly taken into account by the ME instruction pipeline. Its use requires the `PSP_MODULE_AV_AVCODEC` module to be loaded. It is also worth noting that the ME handler natively contains `cacheOp` operations that seem to ensure a DCache and ICache fill on the address of the Syscall targeted function, though the exact address on which the cache operation is performed was not verified at dump time. An optional third patch was applied to the interrupt handler, in order to distinguish custom tasks from system tasks. Developing this method allowed me to lift the veil on certain things that were still unknown to me and gave me a better understanding of the subject. It works, but since the second method proved simpler, it deserved to be implemented.


## Method 2 — Existing Syscall hijacking

Here it is about hijacking an existing Syscall that is simple to patch. To do so, more information about the available Syscalls was needed, and therefore knowledge of the table containing their addresses. The first method described above already allowed preservation of the local EDRAM state as well as the function call process. It was therefore used to spy on the CPU registers before a Syscall function call, and to confirm the address where the table is based by fully dumping the 4MB of EDRAM (slim). This table is available [here](https://github.com/mcidclan/psp-media-engine-cracking-the-unknown/blob/main/media-engine-t2img.syscall-table.txt), and a function with very few instructions was chosen to be patched. It is involved in H.264 decoding and is therefore not called by the system when launching a homebrew in game mode. Its call, whether through H.264 related functions or directly, must in any case be explicitly initiated by the developer, which is an advantage offered by this method. Indeed, its first call will trigger an ICache update for itself, which is appreciated, since only the involved function needed to be patched and nothing else.

But here it is, this is not really viable in a context where the Media Engine has potentially already performed decoding, and apparently even less viable, for both methods, to be used on the XMB due to hot modifications on the host kernel area of the core.


## MIST — ME Inject Syscall via Transfer (Method 3)

Here comes the 3rd method. A way was needed to execute code on the Media Engine without writing to the reset vector, and without writing into the kernel area housing the core. With the (possibly incomplete) Syscall mapping we have available, the new idea was to find a way to inject a custom address into the Syscall table that would point outside of the core. Some time ago I was able to experiment with and understand how the DMACPLUS hardware registers work (many thanks again to davee for the advice). Outside of the functions implemented by Sony, it allows transferring data in kernel mode anywhere on the Media Engine's local EDRAM. I think you can see the solution coming, and it is fairly obvious: use the DMACPLUS in word transfer mode and copy 1 word which is simply the new address for our custom function. However, for the DMACPLUS to be executed on the Media Engine side, the corresponding bit in the bus clock enable register must first be set locally in order to enable clocking to the DMACPLUS. To do so, I used a restart method I had in stock that restarts the ME while preserving the initial reset vector. This restart is necessary in order to communicate the value to be written into the bus clock enable register via a shared register, designed and used by Sony to update this register on the ME side by passing information from the main CPU. This new method is good news, as it will allow direct observation of the behavior of the hardware components located on the Media Engine side.

VME and H.264 decoder, you are next!


## Final notes

The code library will be updated soon, keep an eye on the repository!

Thank you for reading.

*m-c/d*
