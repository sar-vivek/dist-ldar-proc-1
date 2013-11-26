/*
 * $Id$
 */
/* Author : Vivek Sardeshmukh*/
#ifndef TRIANGLE_H
#  define TRIANGLE_H

extern LidarPointNode_t* **TriVertex[NUM_CELLS]; 
extern INT **TriEdge[NUM_CELLS];
extern INT NumTri[NUM_CELLS]; 
extern INT *estack[NUM_CELLS];/*[CellCnt[cell]] - malloc it*/
extern INT topstk[NUM_CELLS];

extern void Delaunay(int cell); 



#endif /* ifndef TRIANGLE_H */

