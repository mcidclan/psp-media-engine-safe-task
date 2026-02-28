## PSP Media Engine Safe Task Library
A library to safely execute custom tasks on the Media Engine without disrupting syscalls dispatched to it by the SC.  

- Tasks are executed in parallel without blocking the SC
- Has a dedicated function to yield the current thread on SC side while waiting for the ME side process to complete
- The task is invoked through a custom syscall the ME side handler is patched for that purpose
- No manual soft reset is required, the first execution goes through a patch of getMeEDRAM
- Device sleep works properly as the base ME wrapper is not altered

Note: Tested on Slim and Phat  

## Requirement
- Clone and install psp-media-engine-custom-core library, see below.
- Clone this repository, then build and install the library via cmake.

## Usage
See samples directory.

## Disclamer
This project and code are provided as-is without warranty. Users assume full responsibility for any implementation or consequences. Use at your own discretion and risk

## Related work
[PSP Media Engine Reload](https://github.com/mcidclan/psp-media-engine-reload)  
[PSP Media Engine Custom Core](https://github.com/mcidclan/psp-media-engine-custom-core)  
[PSP Media Engine Cracking the unknown](https://github.com/mcidclan/psp-media-engine-cracking-the-unknown)  

*m-c/d*
