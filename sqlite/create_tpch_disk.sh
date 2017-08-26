#!/bin/bash

### FAT32 TEST DISK ###

DISK=my.disk
MOUNTDIR=tmpdisk
TPCDISK=TPC-H-small.db

# If no arg supplied, setup disk
if [ $# -eq 0 ]
then

  # Remove disk if exists
  rm -f $DISK
  # Preallocate space to a 4GB file
  truncate -s 256M $DISK
  # Create FAT32 filesystem on "my.disk"
  mkfs.ext3 $DISK

  # Create mountdir and mount
  mkdir -p $MOUNTDIR
  sudo mount -o rw $DISK $MOUNTDIR/
  sudo mkdir -p $MOUNTDIR/db/company

  rm -f blob.db

  cp ${TPCDISK} $MOUNTDIR/db/company/my.db
  cp ${TPCDISK} /db/company/my.db

  sync # Mui Importante
  sudo umount $MOUNTDIR/
  rmdir $MOUNTDIR

  chown kollerr:kollerr my.disk

# If "clean" is supplied, clean up
elif [ $1 = "clean" ]
then
  echo "> Cleaning up TEST DISK: $DISK"
  rm -f $DISK
fi
