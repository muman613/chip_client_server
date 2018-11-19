#
#	NOTE:
#	Set BASE to base of dcc2 branch
#

GCCVER= 	3.4.2
#GCCVER= 	4.0.4
SOURCES=	chipdb.c chipinterf.c daemon.c dbgimplementation.c \
			interface.c msglog.c request.c utils.c
OBJ=		$(SOURCES:%.c=%.o)

NETCONNECTION=${BASE}/llad/network
BASE=../../..

ifneq '$(filter -DEM86XX_MODE=EM86XX_MODEID_STANDALONE, $(RMCFLAGS))' ''
POPTDIR=../../externlib/popt-1.7

ifneq '$(filter -DEM86XX_CHIP=EM86XX_CHIPID_TANGO15, $(RMCFLAGS))' ''
DCTCONNECTION=${BASE}/llad/direct
CROSS=/utils/gcc/2.95.3/arm-elf-uclibc/i386/bin
CC=${CROSS}/arm-elf-gcc
LD=${CROSS}/arm-elf-ld
STRIP=${CROSS}/arm-elf-strip
RMCFLAGS += -DLLAD_DIRECT=1
INCLUDEDIRS= -I ${BASE} -I ${POPTDIR}
#LIBDIRS= -L ${POPTDIR}/.libs
LIBDIRS=-L ../../lib/arm
LDFLAGS = ${LIBDIRS} -Wl,-elf2flt="-s32768"
LLADDIR=../libs/arm
else
DCTCONNECTION=${BASE}/llad/direct
CROSS=/utils/gcc/$(GCCVER)/mipsel-linux-uclibc/i686/bin/
CC=${CROSS}/mipsel-linux-uclibc-gcc
LD=${CROSS}/mipsel-linux-uclibc-ld
STRIP=${CROSS}/mipsel-linux-uclibc-strip
INCLUDEDIRS= -I ${BASE} -I ${POPTDIR}
#LIBDIRS= -L ${POPTDIR}/.libs
LIBDIRS=-L ../../lib/mips
LDFLAGS = ${LIBDIRS} -Wl -static -static-libgcc
LLADDIR=../libs/mips
endif

else	# Build for withhost

DCTCONNECTION=${BASE}/llad/mum
CC=gcc
LD=ld
STRIP=strip
INCLUDEDIRS= -I ${BASE}
LLADDIR=../libs/i386
LDFLAGS= -static -static-libgcc

endif

#
#DEBUGOPTS=-g3
WARNINGS= -Wall -Wchar-subscripts -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wsign-compare -Wuninitialized
#CPPFLAGS = -I ${BASE} -I ${POPTDIR} ${RMCFLAGS}
CPPFLAGS = ${WARNINGS} ${INCLUDEDIRS} ${RMCFLAGS} ${DEBUGOPTS}

LIBS=$(LLADDIR)/libllad.a

%.o:%.c
	${CC} -c ${CPPFLAGS} -O $*.c
	

chipinterf: ${OBJ} ${OBJECTS}
	${CC} -o chipinterf ${OBJ} ${LIBS}  ${LDFLAGS} -lpopt
	${STRIP} chipinterf
	chmod 0777 chipinterf

dep:	.PHONY
	makedepend ${SOURCES}
	
clean:
	rm -f chipinterf
	rm -f interface
	rm -f *.o

.PHONY:
	@true
	
# DO NOT DELETE

chipdb.o: StdAfx.h /usr/include/stdio.h /usr/include/features.h
chipdb.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
chipdb.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
chipdb.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
chipdb.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
chipdb.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
chipdb.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
chipdb.o: /usr/include/endian.h /usr/include/bits/endian.h
chipdb.o: /usr/include/sys/select.h /usr/include/bits/select.h
chipdb.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
chipdb.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
chipdb.o: /usr/include/alloca.h /usr/include/unistd.h
chipdb.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
chipdb.o: /usr/include/getopt.h /usr/include/netdb.h
chipdb.o: /usr/include/netinet/in.h /usr/include/stdint.h
chipdb.o: /usr/include/bits/wchar.h /usr/include/sys/socket.h
chipdb.o: /usr/include/sys/uio.h /usr/include/bits/uio.h
chipdb.o: /usr/include/bits/socket.h /usr/include/limits.h
chipdb.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
chipdb.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
chipdb.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
chipdb.o: /usr/include/asm/sockios.h /usr/include/bits/in.h
chipdb.o: /usr/include/bits/byteswap.h /usr/include/rpc/netdb.h
chipdb.o: /usr/include/bits/netdb.h /usr/include/sys/time.h
chipdb.o: /usr/include/sys/wait.h /usr/include/signal.h
chipdb.o: /usr/include/bits/signum.h /usr/include/bits/siginfo.h
chipdb.o: /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h
chipdb.o: /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h
chipdb.o: /usr/include/sys/resource.h /usr/include/bits/resource.h
chipdb.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
chipdb.o: /usr/include/arpa/inet.h /usr/include/popt.h daemon.h chipinterf.h
chipdb.o: chipdb.h interface.h version.h msglog.h /usr/include/string.h
chipinterf.o: StdAfx.h /usr/include/stdio.h /usr/include/features.h
chipinterf.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
chipinterf.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
chipinterf.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
chipinterf.o: /usr/include/libio.h /usr/include/_G_config.h
chipinterf.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
chipinterf.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
chipinterf.o: /usr/include/sys/types.h /usr/include/time.h
chipinterf.o: /usr/include/endian.h /usr/include/bits/endian.h
chipinterf.o: /usr/include/sys/select.h /usr/include/bits/select.h
chipinterf.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
chipinterf.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
chipinterf.o: /usr/include/alloca.h /usr/include/unistd.h
chipinterf.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
chipinterf.o: /usr/include/getopt.h /usr/include/netdb.h
chipinterf.o: /usr/include/netinet/in.h /usr/include/stdint.h
chipinterf.o: /usr/include/bits/wchar.h /usr/include/sys/socket.h
chipinterf.o: /usr/include/sys/uio.h /usr/include/bits/uio.h
chipinterf.o: /usr/include/bits/socket.h /usr/include/limits.h
chipinterf.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
chipinterf.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
chipinterf.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
chipinterf.o: /usr/include/asm/sockios.h /usr/include/bits/in.h
chipinterf.o: /usr/include/bits/byteswap.h /usr/include/rpc/netdb.h
chipinterf.o: /usr/include/bits/netdb.h /usr/include/sys/time.h
chipinterf.o: /usr/include/sys/wait.h /usr/include/signal.h
chipinterf.o: /usr/include/bits/signum.h /usr/include/bits/siginfo.h
chipinterf.o: /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h
chipinterf.o: /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h
chipinterf.o: /usr/include/sys/resource.h /usr/include/bits/resource.h
chipinterf.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
chipinterf.o: /usr/include/arpa/inet.h /usr/include/popt.h daemon.h
chipinterf.o: chipinterf.h chipdb.h interface.h version.h msglog.h request.h
chipinterf.o: simulator.h
daemon.o: /usr/include/unistd.h /usr/include/features.h
daemon.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
daemon.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
daemon.o: /usr/include/bits/posix_opt.h /usr/include/bits/types.h
daemon.o: /usr/include/bits/typesizes.h /usr/include/bits/confname.h
daemon.o: /usr/include/getopt.h /usr/include/stdlib.h
daemon.o: /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h
daemon.o: /usr/include/bits/endian.h /usr/include/sys/select.h
daemon.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
daemon.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
daemon.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
daemon.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
daemon.o: /usr/include/signal.h /usr/include/bits/signum.h
daemon.o: /usr/include/bits/siginfo.h /usr/include/bits/sigaction.h
daemon.o: /usr/include/bits/sigcontext.h /usr/include/bits/sigstack.h
daemon.o: /usr/include/bits/sigthread.h /usr/include/sys/wait.h
daemon.o: /usr/include/sys/resource.h /usr/include/bits/resource.h
daemon.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
daemon.o: /usr/include/errno.h /usr/include/bits/errno.h
daemon.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
daemon.o: /usr/include/asm-generic/errno.h
daemon.o: /usr/include/asm-generic/errno-base.h daemon.h
dbgimplementation.o: /usr/include/stdio.h /usr/include/features.h
dbgimplementation.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
dbgimplementation.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
dbgimplementation.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
dbgimplementation.o: /usr/include/libio.h /usr/include/_G_config.h
dbgimplementation.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
dbgimplementation.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
dbgimplementation.o: /usr/include/sys/types.h /usr/include/time.h
dbgimplementation.o: /usr/include/endian.h /usr/include/bits/endian.h
dbgimplementation.o: /usr/include/sys/select.h /usr/include/bits/select.h
dbgimplementation.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
dbgimplementation.o: /usr/include/sys/sysmacros.h
dbgimplementation.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
dbgimplementation.o: /usr/include/string.h
interface.o: StdAfx.h /usr/include/stdio.h /usr/include/features.h
interface.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
interface.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
interface.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
interface.o: /usr/include/libio.h /usr/include/_G_config.h
interface.o: /usr/include/wchar.h /usr/include/bits/stdio_lim.h
interface.o: /usr/include/bits/sys_errlist.h /usr/include/stdlib.h
interface.o: /usr/include/sys/types.h /usr/include/time.h
interface.o: /usr/include/endian.h /usr/include/bits/endian.h
interface.o: /usr/include/sys/select.h /usr/include/bits/select.h
interface.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
interface.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
interface.o: /usr/include/alloca.h /usr/include/unistd.h
interface.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
interface.o: /usr/include/getopt.h /usr/include/netdb.h
interface.o: /usr/include/netinet/in.h /usr/include/stdint.h
interface.o: /usr/include/bits/wchar.h /usr/include/sys/socket.h
interface.o: /usr/include/sys/uio.h /usr/include/bits/uio.h
interface.o: /usr/include/bits/socket.h /usr/include/limits.h
interface.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
interface.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
interface.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
interface.o: /usr/include/asm/sockios.h /usr/include/bits/in.h
interface.o: /usr/include/bits/byteswap.h /usr/include/rpc/netdb.h
interface.o: /usr/include/bits/netdb.h /usr/include/sys/time.h
interface.o: /usr/include/sys/wait.h /usr/include/signal.h
interface.o: /usr/include/bits/signum.h /usr/include/bits/siginfo.h
interface.o: /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h
interface.o: /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h
interface.o: /usr/include/sys/resource.h /usr/include/bits/resource.h
interface.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
interface.o: /usr/include/arpa/inet.h /usr/include/popt.h daemon.h
interface.o: chipinterf.h chipdb.h interface.h version.h msglog.h
interface.o: /usr/include/string.h /usr/include/sys/stat.h
interface.o: /usr/include/bits/stat.h /usr/include/fcntl.h
interface.o: /usr/include/bits/fcntl.h /usr/include/errno.h
interface.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
interface.o: /usr/include/asm/errno.h /usr/include/asm-generic/errno.h
interface.o: /usr/include/asm-generic/errno-base.h simulator.h request.h
interface.o: utils.h ../../debugger/src/broadcast_pkt.hh
interface.o: /usr/include/sys/un.h
msglog.o: StdAfx.h /usr/include/stdio.h /usr/include/features.h
msglog.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
msglog.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
msglog.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
msglog.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
msglog.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
msglog.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
msglog.o: /usr/include/endian.h /usr/include/bits/endian.h
msglog.o: /usr/include/sys/select.h /usr/include/bits/select.h
msglog.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
msglog.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
msglog.o: /usr/include/alloca.h /usr/include/unistd.h
msglog.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
msglog.o: /usr/include/getopt.h /usr/include/netdb.h
msglog.o: /usr/include/netinet/in.h /usr/include/stdint.h
msglog.o: /usr/include/bits/wchar.h /usr/include/sys/socket.h
msglog.o: /usr/include/sys/uio.h /usr/include/bits/uio.h
msglog.o: /usr/include/bits/socket.h /usr/include/limits.h
msglog.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
msglog.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
msglog.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
msglog.o: /usr/include/asm/sockios.h /usr/include/bits/in.h
msglog.o: /usr/include/bits/byteswap.h /usr/include/rpc/netdb.h
msglog.o: /usr/include/bits/netdb.h /usr/include/sys/time.h
msglog.o: /usr/include/sys/wait.h /usr/include/signal.h
msglog.o: /usr/include/bits/signum.h /usr/include/bits/siginfo.h
msglog.o: /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h
msglog.o: /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h
msglog.o: /usr/include/sys/resource.h /usr/include/bits/resource.h
msglog.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
msglog.o: /usr/include/arpa/inet.h /usr/include/popt.h daemon.h chipinterf.h
msglog.o: chipdb.h interface.h version.h msglog.h /usr/include/string.h
msglog.o: /usr/include/syslog.h /usr/include/sys/syslog.h
msglog.o: /usr/include/bits/syslog-path.h
request.o: StdAfx.h /usr/include/stdio.h /usr/include/features.h
request.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
request.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
request.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
request.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
request.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
request.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
request.o: /usr/include/endian.h /usr/include/bits/endian.h
request.o: /usr/include/sys/select.h /usr/include/bits/select.h
request.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
request.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
request.o: /usr/include/alloca.h /usr/include/unistd.h
request.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
request.o: /usr/include/getopt.h /usr/include/netdb.h
request.o: /usr/include/netinet/in.h /usr/include/stdint.h
request.o: /usr/include/bits/wchar.h /usr/include/sys/socket.h
request.o: /usr/include/sys/uio.h /usr/include/bits/uio.h
request.o: /usr/include/bits/socket.h /usr/include/limits.h
request.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
request.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
request.o: /usr/include/bits/sockaddr.h /usr/include/asm/socket.h
request.o: /usr/include/asm/sockios.h /usr/include/bits/in.h
request.o: /usr/include/bits/byteswap.h /usr/include/rpc/netdb.h
request.o: /usr/include/bits/netdb.h /usr/include/sys/time.h
request.o: /usr/include/sys/wait.h /usr/include/signal.h
request.o: /usr/include/bits/signum.h /usr/include/bits/siginfo.h
request.o: /usr/include/bits/sigaction.h /usr/include/bits/sigcontext.h
request.o: /usr/include/bits/sigstack.h /usr/include/bits/sigthread.h
request.o: /usr/include/sys/resource.h /usr/include/bits/resource.h
request.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
request.o: /usr/include/arpa/inet.h /usr/include/popt.h daemon.h chipinterf.h
request.o: chipdb.h interface.h version.h msglog.h /usr/include/string.h
request.o: /usr/include/sys/stat.h /usr/include/bits/stat.h
request.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
request.o: /usr/include/errno.h /usr/include/bits/errno.h
request.o: /usr/include/linux/errno.h /usr/include/asm/errno.h
request.o: /usr/include/asm-generic/errno.h
request.o: /usr/include/asm-generic/errno-base.h /usr/include/assert.h
request.o: simulator.h request.h utils.h
utils.o: StdAfx.h /usr/include/stdio.h /usr/include/features.h
utils.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
utils.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
utils.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
utils.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
utils.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
utils.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
utils.o: /usr/include/endian.h /usr/include/bits/endian.h
utils.o: /usr/include/sys/select.h /usr/include/bits/select.h
utils.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
utils.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
utils.o: /usr/include/alloca.h /usr/include/unistd.h
utils.o: /usr/include/bits/posix_opt.h /usr/include/bits/confname.h
utils.o: /usr/include/getopt.h /usr/include/netdb.h /usr/include/netinet/in.h
utils.o: /usr/include/stdint.h /usr/include/bits/wchar.h
utils.o: /usr/include/sys/socket.h /usr/include/sys/uio.h
utils.o: /usr/include/bits/uio.h /usr/include/bits/socket.h
utils.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
utils.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
utils.o: /usr/include/bits/posix2_lim.h /usr/include/bits/sockaddr.h
utils.o: /usr/include/asm/socket.h /usr/include/asm/sockios.h
utils.o: /usr/include/bits/in.h /usr/include/bits/byteswap.h
utils.o: /usr/include/rpc/netdb.h /usr/include/bits/netdb.h
utils.o: /usr/include/sys/time.h /usr/include/sys/wait.h
utils.o: /usr/include/signal.h /usr/include/bits/signum.h
utils.o: /usr/include/bits/siginfo.h /usr/include/bits/sigaction.h
utils.o: /usr/include/bits/sigcontext.h /usr/include/bits/sigstack.h
utils.o: /usr/include/bits/sigthread.h /usr/include/sys/resource.h
utils.o: /usr/include/bits/resource.h /usr/include/bits/waitflags.h
utils.o: /usr/include/bits/waitstatus.h /usr/include/arpa/inet.h
utils.o: /usr/include/popt.h daemon.h chipinterf.h chipdb.h interface.h
utils.o: version.h msglog.h /usr/include/string.h /usr/include/sys/stat.h
utils.o: /usr/include/bits/stat.h /usr/include/fcntl.h
utils.o: /usr/include/bits/fcntl.h /usr/include/errno.h
utils.o: /usr/include/bits/errno.h /usr/include/linux/errno.h
utils.o: /usr/include/asm/errno.h /usr/include/asm-generic/errno.h
utils.o: /usr/include/asm-generic/errno-base.h simulator.h utils.h
