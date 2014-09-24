/* osimg.c - boot a general hobbist OS */
/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2002,2004,2007,2009,2010  Free Software Foundation, Inc.
 *
 *  GRUB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GRUB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GRUB.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <grub/loader.h>
#include <grub/machine/chainloader.h>
#include <grub/machine/memory.h>
#include <grub/file.h>
#include <grub/err.h>
#include <grub/device.h>
#include <grub/disk.h>
#include <grub/misc.h>
#include <grub/types.h>
#include <grub/partition.h>
#include <grub/memory.h>
#include <grub/dl.h>
#include <grub/command.h>
#include <grub/msdos_partition.h>
#include <grub/machine/biosnum.h>
#include <grub/cpu/floppy.h>
#include <grub/i18n.h>
#include <grub/video.h>
#include <grub/mm.h>
#include <grub/fat.h>
#include <grub/ntfs.h>
#include <grub/i386/relocator.h>

GRUB_MOD_LICENSE ("GPLv3+");

static grub_dl_t my_mod;
static int boot_drive;
static grub_addr_t boot_part_addr;
static struct grub_relocator *rel;
static grub_addr_t loader_addr;



static grub_err_t
grub_osimg_boot (void)
{
  struct grub_relocator16_state state = {
    .edx = boot_drive,
    .esi = boot_part_addr,
    .ds = 0,
    .es = 0,
    .fs = 0,
    .gs = 0,
    .ss = 0,
    .cs = 0,
    .sp = loader_addr,
    .ip = loader_addr,
    .a20 = 0
  };
  grub_video_set_mode ("text", 0, 0);

  return grub_relocator16_boot (rel, state);
}

static grub_err_t
grub_osimg_unload (void)
{
  grub_relocator_unload (rel);
  rel = NULL;
  grub_dl_unref (my_mod);
  return GRUB_ERR_NONE;
}

static void
grub_osimg_cmd (const char *filename, grub_addr_t memoffset)
{
  grub_file_t file = 0;
  grub_device_t dev;
  int drive = -1;
  grub_addr_t part_addr = 0;
  grub_uint8_t *bs, *ptable;


  rel = grub_relocator_new ();
  if (!rel)
    goto fail;


  grub_dl_ref (my_mod);

  /* here */
  loader_addr = memoffset;

  grub_file_filter_disable_compression ();
  file = grub_file_open (filename);
  if (! file)
    goto fail;

  {
    grub_relocator_chunk_t ch;
    grub_err_t err;

    err = grub_relocator_alloc_chunk_addr (rel, &ch, memoffset,file->size);

    if (err)
      goto fail;
    bs = get_virtual_current_address (ch);
    err = grub_relocator_alloc_chunk_addr (rel, &ch,
					   GRUB_MEMORY_MACHINE_PART_TABLE_ADDR,
					   64);
    if (err)
      goto fail;
    ptable = get_virtual_current_address (ch);
  }

  /* Read the all file.  */
  if (grub_file_read (file, bs, file->size)
      != (int) file->size)
    {
      if (!grub_errno)
	grub_error (GRUB_ERR_BAD_OS, N_("premature end of file %s"),
		    filename);

      goto fail;
    }


  grub_file_close (file);

  /* Obtain the partition table from the root device.  */
  drive = grub_get_root_biosnumber ();
  dev = grub_device_open (0);
  if (dev && dev->disk && dev->disk->partition)
    {
      grub_disk_t disk = dev->disk;

      if (disk)
	{
	  grub_partition_t p = disk->partition;

	  if (p && grub_strcmp (p->partmap->name, "msdos") == 0)
	    {
	      disk->partition = p->parent;
	      grub_disk_read (disk, p->offset, 446, 64, ptable);
	      part_addr = (GRUB_MEMORY_MACHINE_PART_TABLE_ADDR
			   + (p->index << 4));
	      disk->partition = p;
	    }
	}
    }


  if (dev)
    grub_device_close (dev);

  /* Ignore errors. Perhaps it's not fatal.  */
  grub_errno = GRUB_ERR_NONE;

  boot_drive = drive;
  boot_part_addr = part_addr;

  grub_loader_set (grub_osimg_boot, grub_osimg_unload, 1);
  return;

 fail:

  if (file)
    grub_file_close (file);

  grub_dl_unref (my_mod);
}

static grub_err_t
grub_cmd_osimg (grub_command_t cmd __attribute__ ((unused)),
		      int argc, char *argv[])
{
	long val;
	char *endptr, *str;
	str = argv[1];

  if (argc != 2)
    return grub_error (GRUB_ERR_BAD_ARGUMENT, N_("args: <filename> <memory_offset>"));

  grub_errno = 0;
  val = grub_strtoul(str, &endptr, 0);

  if (grub_errno || (endptr == str)) {
        return grub_error (GRUB_ERR_BAD_ARGUMENT, N_("args: <filename> <memory_offset> eg /boot/os.img 0x7c00"));
  }

  grub_osimg_cmd (argv[0], val);

  return grub_errno;
}

static grub_command_t cmd;

GRUB_MOD_INIT(osimg)
{
  cmd = grub_register_command ("osimg", grub_cmd_osimg,
			       N_("FILE MEM_OFFSET)"),
			       N_("Load a general hobbist OS."));
  my_mod = mod;
}

GRUB_MOD_FINI(osimg)
{
  grub_unregister_command (cmd);
}
