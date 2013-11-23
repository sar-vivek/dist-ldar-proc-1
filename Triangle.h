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

/*return triangle number of the triangle which contains pt point*/
INT triLoc(int cellnum, LidarPointNode_t *point); 

#endif /* ifndef TRIANGLE_H */

