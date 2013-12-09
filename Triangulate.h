/*
 * Triangulate.h
 *
 * Author: Vivek B Sardeshmukh   
 *
 * Derived from 'A fast algorithm for constructing Delaunay triangulations in
 * the plane' by S. W. Sloan, Adv. Eng. Software, 1987, Vol. 9, No. 1.
 *
 */

#ifndef TRIANGULATE_H
#define TRIANGULATE_H

extern LidarPointNode_t ***TriVertex[NUM_CELLS]; 
extern INT **TriEdge[NUM_CELLS];
extern INT NumTri[NUM_CELLS]; 
extern INT *estack[NUM_CELLS];/*[CellCnt[cell]] - malloc it*/
extern INT topstk[NUM_CELLS];

void Delaunay(int cell); 

#endif

