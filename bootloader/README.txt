
UEFI Bootloader
---------------

UEFI is a replacement to the BIOS, not only as the system firmware but also changes how the computer boots.
previously the computer would start in 16 bits mode, then jump to 32 and then to 64, all that process being the responsibility of the bootloader.
If the system is of 64 bits and the uefi firmware is also of 64 bits the system boot directly into 64 bits, meaning the bootloader binary
must be a 64 bits executable.

It also removes old interfaces like VGA and VESA when dealing with the gpu, but in turn provides alternate ways of talking with the hardware
all those new interfaces are defined as protocols, which you need to ask for to the firmware, as such to load a file instead of parsing the disk and
reading the filesystem directly, you ask for the appropiate protocol that is able of understandding the disk and the filesystem, and then you use the API
of that protocol to load the file.


Steps
-----

* firmware load the bootloader and starts executin the entry point, at this stage the bootloader is a regular program.
* clean up the screen and setup a more appropiate video mode
* allocate memory for the arguments that will be passed to the kernel
* load the kernel file form disk
* parse the ELF header of the kernel
* obtain the system memory map(is needed to exit the boot services)
* exit boot services, this destroys the uefi environment and we can no longer use anything that requires the boot services(and we can no longer return to the firmware)
* fill the the struct that will be passed to the kernel
* jump into the kernel with the information extracted from the ELF header
