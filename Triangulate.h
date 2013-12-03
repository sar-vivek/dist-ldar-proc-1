/*
 * $Id$
 */
/* Author : Vivek Sardeshmukh*/
#ifndef TRIANGULATE_H
#  define TRIANGULATE_H

extern LidarPointNode_t* **TriVertex[NUM_CELLS]; 
extern INT **TriEdge[NUM_CELLS];
extern INT NumTri[NUM_CELLS]; 
extern INT *estack[NUM_CELLS];/*[CellCnt[cell]] - malloc it*/
extern INT topstk[NUM_CELLS];

void Delaunay(int cell); 



#endif /* ifndef TRIANGULATE_H */

