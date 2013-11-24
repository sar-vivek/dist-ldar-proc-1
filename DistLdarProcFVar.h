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
extern LidarPointNode_t *CurTbl[NUM_BINS_X][NUM_BINS_Y];
extern double BinU1[NUM_CELLS][NUM_BINS_X][NUM_BINS_Y];
extern double BinU2[NUM_CELLS][NUM_BINS_X][NUM_BINS_Y];
extern uint32_t BinCnt[NUM_CELLS][NUM_BINS_X][NUM_BINS_Y];
extern LidarPointNode_t CellMin[NUM_CELLS];
extern LidarPointNode_t CellMax[NUM_CELLS];

extern char NodeIPs[NUM_NODES][16];

extern struct sockaddr_in svr_addr[NUM_NODES];
extern int msock[NUM_NODES];

extern struct timeval t_start;
extern struct timeval t_end;

extern double t_diff;
extern double Xint;
extern double Yint;
extern double Xdiff;
extern double Ydiff;
extern double X_c;
extern double Y_c;
extern double Z_c;
extern double u1;
extern double u2;

extern LidarPointNode_t *PntTbl;
extern LidarPointNode_t *current;
extern double *Z2;
extern double *current2;
extern int8_t *FiltTbl;
extern void *X;
extern void *Y;
extern void *Z;

extern int NodeID;
extern int ssock;
extern uint32_t count;
extern uint32_t n;

#endif
