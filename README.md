grub-2.0.0
==========

Grub2 git mirror - start from version 2.0.0 - using to extend other unofficial functions - Fork welcomed


Grub2 Homepage  http://www.gnu.org/software/grub/


===================
branch "osimg"
===================
add command "osimg"

eg. in grub.cfg

menuentry "Pure64 /boot" {
	insmod part_msdos
	insmod ext2
	set root='hd0,msdos7'	
	osimg	/boot/pure64.sys 0x8000
}

This means load a OS image on a ext2 partition(/dev/sda7) with path(filename) /boot/pure64.sys, load it to memory 0000:0x8000 and run it.

Command:
osimg FILE STARTUP_OFFISET

The FILE will be loaded into STARTUP_OFFISET and run from there.
Remember don't let STARTUP_OFFISET to mass up with the reserved memory locations, or it will not work correctly.

Background story:
The x86 machinie read first 512 bytes from a harddisk to memory 0000:0x7c00 to boot.
This function removed the limitation of "512 bytes" and "from 0x7C00". 
When you write a hobbist OS, maybe even without filesystem routines. It should be a good help.





