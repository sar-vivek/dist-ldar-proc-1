/*JWHGPLHERE*/
/*
 * DistributeSlave.h
 *
 * Author: James W Hegeman
 *
 */

#ifndef __DISTRIBUTE_SLAVE_H__
#define __DISTRIBUTE_SLAVE_H__

void Receive(int sd, void *buffer, size_t len);
void DistributeReceive();

#endif
