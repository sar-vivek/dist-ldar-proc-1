/*JWHGPLHERE*/
/*
 * RMSECalc.h
 *
 * Author: James W Hegeman
 *
 */

#ifndef __RMSE_CALC_H__
#define __RMSE_CALC_H__

double RMSEFindZ(LidarPointNode_t *A, LidarPointNode_t *B, LidarPointNode_t *C, LidarPointNode_t *X);
double ComputeLocalMSE();
void RMSECalcMaster();
void RMSECalcSlave();

#endif
