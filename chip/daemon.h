#ifndef	__DAEMON_H__
#define __DAEMON_H__
/* daemon.c:14:NF */ extern void closeall (int fd); /* (fd) int fd; */
/* daemon.c:30:NF */ extern int daemon (int nochdir, int noclose); /* (nochdir, noclose) int nochdir; int noclose; */
/* daemon.c:74:OF */ extern int fork2 (void); /* () */
#endif
