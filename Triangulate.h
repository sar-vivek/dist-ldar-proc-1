/*
 * Triangulate.h
 *
 * Author: Vivek B Sardeshmukh   
 *
 * Derived from 'A fast algorithm for constructing Delaunay triangulations in
 * the plane' by S. W. Sloan, Adv. Eng. Software, 1987, Vol. 9, No. 1.
 *
 */

#ifndef __TRIANGULATE_H__
#define __TRIANGULATE_H__

extern LidarPointNode_t ***TriVertex[NUM_CELLS];
extern INT **TriEdge[NUM_CELLS];
extern INT *estack[NUM_CELLS];
extern INT NumTri[NUM_CELLS];
extern INT topstk[NUM_CELLS];
extern LidarPointNode_t BigTriangle[NUM_CELLS][4];

INT triLoc(int cell, LidarPointNode_t *point, int *bfp, int *dfp);
void Delaunay(int cell); 

#endif
