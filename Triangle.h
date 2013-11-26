/*
 * $Id$
 */
/* Author : Vivek Sardeshmukh*/
#ifndef TRIANGLE_H
#  define TRIANGLE_H
/*TriVertex[NUM_CELLS][NUM_TRI][3]*/
extern LidarPointNode_t* **TriVertex[NUM_CELLS]; 
extern INT **TriEdge[NUM_CELLS];
extern INT NumTri[NUM_CELLS]; /*number of triangles created so far for a cell*/ 
extern INT *estack[NUM_CELLS];/*[CellCnt[cell]] - malloc it*/
extern INT topstk[NUM_CELLS];

extern void Delaunay(int cell); 



#endif /* ifndef TRIANGLE_H */

