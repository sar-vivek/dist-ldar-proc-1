/*JWHGPLHERE*/
/*
 * DLPstd.h
 *
 * Author: James W Hegeman
 *
 */

#ifndef __DLP_STD_H__
#define __DLP_STD_H__

#include <arpa/inet.h>
#include <assert.h>
#include <math.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define DEBUG 1 
/*interpret double zero to be in between -DZERO to +DZERO*/
#define DZERO 1.0e-8 
#define NODE_POINTS_MAX 16777216

/* NUM_BINS_X and NUM_BINS_Y should be even */
#define NUM_BINS_X 4 
#define NUM_BINS_Y 2

#define NUM_CELLS 1 
#define NUM_CELLS_X 1 
#define NUM_CELLS_Y 1

#define NUM_NODES 1
#define NUM_NODES_X 1
#define NUM_NODES_Y 1

#define NUM_WORKERS (NUM_CELLS - 1)

#define POINT_DATA_SKIP 16

#define PORT_BASE 57394

#define VAR_THRESHOLD 0.01

#define CHAR_SIZE 1
#define UCHAR_SIZE 1
#define INT8_SIZE 1
#define UINT16_SIZE 2
#define INT32_SIZE 4
#define UINT32_SIZE 4
#define DOUBLE_SIZE 8
#define XYZ_SIZE 12

#define BOUNDARY 0xFFFFFFFF
#define INT uint32_t

#define PERROR(X) do {           \
fprintf(stderr, X);              \
fprintf(stderr, ". Exiting.\n"); \
fflush(stderr);                  \
fclose(las_file_in);             \
exit(-1);                        \
} while (0)

#endif
