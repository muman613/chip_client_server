#!/bin/bash
################################################################################
#	MODULE		:	build.sh
#	PROJECT		:	em8600_probe
#	PROGRAMMER	:	Michael A. Uman
#	DATE		:	Oct 2007
#	PURPOSE		:	Rebuild all architectures
################################################################################
echo 'Automated build'

TARGET=chipinterf

if [ ! -d bin ]; then
	mkdir bin
fi
#	Make i386 dir
if [ ! -d bin/i386 ]; then
	mkdir bin/i386	
fi
#	Make mips dir
if [ ! -d bin/mips ]; then
	mkdir bin/mips
fi
#	Make arm dir
if [ ! -d bin/arm ]; then
	mkdir bin/arm
fi

OLDFLAGS=$RMCFLAGS

unset RMCFLAGS
rmcflags tango2 withhost ruauser
make 2>&1 > /dev/null clean
make
cp $TARGET bin/i386

unset RMCFLAGS
rmcflags tango2 standalone ruakernel
make 2>&1 > /dev/null clean
make
cp $TARGET bin/mips

unset RMCFLAGS
rmcflags tango15 standalone ruakernel
make 2>&1 > /dev/null clean
make
cp $TARGET bin/arm

export RMCFLAGS=$OLDFLAGS

