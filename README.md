grub-2.0.0
==========

Grub2 git mirror - start from version 2.0.0 - using to extend other unofficial functions - Fork welcomed


Grub2 Homepage  http://www.gnu.org/software/grub/


===================
## changes:

### add command "osimg"

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
sudo apt-get install autogen    # not checked in configure but make trouble later in make
git clone <this repo>
git checkout origin osimg    
./autogen.sh
./configure
echo ""> grub-2.00/grub-2.00/docs/grub-dev.texi  # which will make error, just clear it
make -j4
sudo make install
sudo ./grub-install /dev/sdX (X should be replaced)
edit /boot/grub/grub.cfg is needed.
reboot, you can see grub version is "Grub-2.00-osimg".
```

### add default matched by title

`ver osimg-2`

eg. in grub.cfg set default="Windows"

than grub will select first title contains "Windows"

This is easier than set a integer value which may changes.



