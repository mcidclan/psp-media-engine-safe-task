## PSP Media Engine Safe Task Library
A library to safely execute custom tasks on the Media Engine without disrupting syscalls dispatched to it by the SC.  

- Tasks are executed in parallel without blocking the SC
- Has a dedicated function to yield the current thread on SC side while waiting for the ME side process to complete
- In classic version, the task is invoked through a custom syscall, the ME side handler is patched for that purpose (load/unload module is necessary)
- In mini version, the task is invoked through a customized syscall function, which is patched for that purpose (no need to load/unload modules)
- No manual soft reset is required, for the classic version the first execution goes through a patch of getMeEDRAM, the mini version is self-sustained
- Device sleep works properly as the base ME wrapper is not altered

Note: Classic version tested on Slim and Phat, mini version tested on Slim (WIP)

## Requirement
- Clone and install psp-media-engine-custom-core library, see below.
- Clone this repository, then build and install the library via cmake.

## Usage
See samples directory.

## Contribution Guidelines

### AI-assisted development

AI tools may be used as development aids. However, the following rules apply strictly:

* All commits must be authored by a human contributor (pseudonyms are perfectly acceptable).
* The commit history must not contain any AI attribution as author or co-author.
* Contributors must fully review, understand, and validate all submitted code before opening a pull request.
* Contributors are expected to be able to explain and justify their changes during code review.
* The contributor is responsible for ensuring their code does not break existing functionality, including dependencies and the overall library behavior.

*In short: AI can assist, but humans must retain full ownership of the work.*

Pull requests that include AI attribution in commits, or that are not clearly understood and validated by the contributor, will be rejected.

### License compatibility

All code submitted to this repository must be compatible with the MIT License. Dependencies or code snippets under more restrictive licenses (e.g. GPL, LGPL, proprietary) are not accepted. Contributors are responsible for verifying that any third-party code they include is under a permissive license granting at least the same level of freedom as MIT.

## Disclamer
This project and code are provided as-is without warranty. Users assume full responsibility for any implementation or consequences. Use at your own discretion and risk

## Related work
[PSP Media Engine Reload](https://github.com/mcidclan/psp-media-engine-reload)  
[PSP Media Engine Custom Core](https://github.com/mcidclan/psp-media-engine-custom-core)  
[PSP Media Engine Cracking the unknown](https://github.com/mcidclan/psp-media-engine-cracking-the-unknown)  

*m-c/d*
