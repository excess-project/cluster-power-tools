Big physical area patch
=======================

This is a patch against Linux 2.4.4 for the `bigphysarea' memory
allocation routines. This code allows you to reserve a large portion
of contiguous physical memory at boot time which can be
allocated/deallocated by kernel drivers.

This sort of hack is necessary for devices such as RAM-less video
framegrabbers which need a big chunk of contiguous physical RAM,
larger than whatever get_free_pages or kmalloc can provide. Also, this
memory is safe to remap_page_range() into user space, e.g., for
mmap().

To install:
	Patch your kernel with `bigphysarea.diff' in this directory
		cd /usr/src/linux
		patch -p1 < <path>/bigphysarea.diff
	Note: it could be the patch fails for Makefile. This is
	      NOT serious, it only means the EXTRAVERSION is
	      not set, so there will be no indication that
	      bigphysarea is compiled in. You could always
	      apply the change by hand.
	Rebuild your kernel

To use:
	Use the boot option
		bigphysarea=<number of pages>
	e.g. by adding a line
	append="bigphysarea=1024"
	to your /etc/lilo.conf to specify the number of pages to
	reserve. If you don't use this option then no pages will
	be reserved. Usage can be monitored through the proc
	filesystem, just type 'cat /proc/bigphysarea'

This code is based on code from M. Welsh (mdw@cs.cornell.edu).

Questions, comments, bug reports? Mail middelink@polyware.nl or
look at 
	http://www.polyware.nl/~middelink/En/hob-v4l.html#bigphysarea

for a new version.

Happy hacking!
  Pauline Middelink

Special thanks goes out to:
- Sebastian Prassl <Sebastian_Prassl@frm2.tu-muenchen.de>
  for finding an important unsigned/signed problem in the free-block
  search algorithm.
