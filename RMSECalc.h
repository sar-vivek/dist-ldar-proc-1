/*JWHGPLHERE*/
/*
 * RMSECalc.h
 *
 * Author: James W Hegeman
 *
 */

#ifndef __RMSE_CALC_H__
#define __RMSE_CALC_H__

void RMSECalcMaster();
void RMSECalcSlave();
double FindZ(LidarPointNode_t *A, LidarPointNode_t *B, LidarPointNode_t *C, LidarPointNode_t *X);

#endif
