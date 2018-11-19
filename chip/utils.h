#ifndef	__UTILS_H__
#define __UTILS_H__

int sendall (int s, const void *msg, size_t len, int flags);
int recvall (int s, void *buf, size_t len, int flags);

#endif
