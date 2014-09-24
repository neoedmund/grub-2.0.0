grub-2.0.0
==========

Grub2 git mirror - start from version 2.0.0 - using to extend other unofficial functions - Fork welcomed


Grub2 Homepage  http://www.gnu.org/software/grub/


===================
branch "osimg"
===================
add command "osimg"

example grub.cfg
```
menuentry "BareMetal w/ Pure64 /boot" {
	insmod part_msdos
	insmod ext2
	set root='hd0,msdos7'	
	osimg	/boot/bm.sys 0x8000
}
```

This means load a OS image on a ext2 partition(/dev/sda7) with path(filename) /boot/bm.sys, load it to memory 0000:0x8000 and run it.

Command:
```
osimg FILE STARTUP_OFFSET
```

The FILE will be loaded into STARTUP_OFFSET and run from there.
Remember don't let STARTUP_OFFISET to mass up with the reserved memory locations(see http://wiki.osdev.org/Memory_Map_(x86) ), or it will not work correctly.

Background story:
The x86 machinie read first 512 bytes from a harddisk to memory 0000:0x7c00 to boot.
This function removed the limitation of "512 bytes" and "from 0x7C00". 
When you write a hobbist OS, maybe even without filesystem routines. It should be a good help.

Usage:
```
git clone <this repo>
mkdir build
cd build
../grub-2.00/configure
make -j4
make install
./grub-install /dev/sdX (X should be replaced)
edit /boot/grub/grub.cfg is needed.
reboot, you can see grub version is "Grub-2.00-osimg".
```


