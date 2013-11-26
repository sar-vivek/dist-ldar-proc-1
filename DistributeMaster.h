/*JWHGPLHERE*/
/*
 * DistributeMaster.h
 *
 * Author: James W Hegeman
 *
 */

#ifndef __DISTRIBUTE_MASTER_H__
#define __DISTRIBUTE_MASTER_H__

int Socket(int domain, int type, int protocol);
void Connect(int sock, const struct sock_addr *addr, socklen_t addr_len);
void Send(int sock, const void *buffer, size_t len);
void DistributeSend();

#endif
