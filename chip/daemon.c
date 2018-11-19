#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "daemon.h"

/* closeall() -- close all FDs >= a specified value */

void closeall(int fd) {
    int fdlimit = sysconf(_SC_OPEN_MAX);

    while (fd < fdlimit)
        close(fd++);
}

/* daemon() - detach process from user and disappear into the background
 * returns -1 on failure, but you can't do much except exit in that case
 * since we may already have forked. This is based on the BSD version,
 * so the caller is responsible for things like the umask, etc.
 */

/* believed to work on all Posix systems */

int daemon(int nochdir, int noclose) {
    switch (fork()) {
    case 0:
        break;
    case -1:
        return -1;
    default:
        _exit(0);          /* exit the original process */
    }

    if (setsid() < 0)               /* shoudn't fail */
        return -1;

    /* dyke out this switch if you want to acquire a control tty in */
    /* the future -- not normally advisable for daemons */

    switch (fork()) {
    case 0:
        break;
    case -1:
        return -1;
    default:
        _exit(0);
    }

    if (!nochdir)
        chdir("/");

    if (!noclose) {
        closeall(0);
        open("/dev/null",O_RDWR);
        dup(0);
        dup(0);
    }

    return 0;
}

/* fork2() -- like fork, but the new process is immediately orphaned
 *            (won't leave a zombie when it exits)                 
 * Returns 1 to the parent, not any meaningful pid.               
 * The parent cannot wait() for the new process (it's unrelated).
 */

/* This version assumes that you *haven't* caught or ignored SIGCHLD. */
/* If you have, then you should just be using fork() instead anyway.  */

int fork2() {
    pid_t pid;
    //  int rc;
    int status;

    if (!(pid = fork())) {
        switch (fork()) {
        case 0:
            return 0;
        case -1:
            _exit(errno);    /* assumes all errnos are <256 */
        default:
            _exit(0);
        }
    }

    if (pid < 0 || waitpid(pid,&status,0) < 0)
        return -1;

    if (WIFEXITED(status))
        if (WEXITSTATUS(status) == 0)
            return 1;
        else
            errno = WEXITSTATUS(status);
    else
        errno = EINTR;  /* well, sort of :-) */

    return -1;
}
