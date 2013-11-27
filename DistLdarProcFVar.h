/*JWHGPLHERE*/
/*
 * DistLdarProcFVar.h
 *
 * Author: James W Hegeman
 *
 */

#ifndef __DIST_LDAR_PROC_FVAR_H__
#define __DIST_LDAR_PROC_FVAR_H__

extern LidarPointNode_t *BinTbl[NUM_CELLS][NUM_BINS_X][NUM_BINS_Y];
extern double BinU1[NUM_CELLS][NUM_BINS_X][NUM_BINS_Y];
extern double BinU2[NUM_CELLS][NUM_BINS_X][NUM_BINS_Y];
extern uint32_t BinCnt[NUM_CELLS][NUM_BINS_X][NUM_BINS_Y];
extern uint32_t CellCnt[NUM_CELLS];

extern LidarPointNode_t CellMin[NUM_CELLS];
extern LidarPointNode_t CellMax[NUM_CELLS];

extern struct sockaddr_in svr_addr[NUM_NODES];
extern int msock[NUM_NODES];

extern pthread_t Workers[NUM_WORKERS + 1];
extern int WorkerIDs[NUM_WORKERS + 1];

extern char NodeIPs[3][16];

extern LidarPointNode_t NodeMin;
extern LidarPointNode_t NodeMax;

extern struct timeval t_start;
extern struct timeval t_end;

extern double t_diff;
extern double Xdiff;
extern double Ydiff;
extern double Xint;
extern double Yint;
extern double Xint_cell;
extern double Yint_cell;
extern double Xint_bin;
extern double Yint_bin;
extern double X_c;
extern double Y_c;
extern double Z_c;

extern LidarPointNode_t *PntTbl;
extern LidarPointNode_t *current;
extern double *Z2;
extern double *current2;
extern int8_t *FiltTbl;
extern void *X_b;
extern void *Y_b;
extern void *Z_b;

extern int NodeID;
extern int ssock;
extern int32_t X;
extern int32_t Y;
extern int32_t Z;
extern uint32_t count;

void *Malloc(size_t len);

#endif
