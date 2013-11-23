/*JWHGPLHERE*/
/*
 * LdarReader.h
 *
 * Author: James W Hegeman
 *
 */

#ifndef __LDAR_READER_H__
#define __LDAR_READER_H__

struct LidarPointNode {
    double X_c;
    double Y_c;
    double Z_c;
    struct LidarPointNode *next;
} __attribute__ ((packed));

typedef struct LidarPointNode LidarPointNode_t;

extern char SysID[32];
extern char GenSoft[32];
extern double Xscale;
extern double Yscale;
extern double Zscale;
extern double Xoffset;
extern double Yoffset;
extern double Zoffset;
extern double MaxX;
extern double MinX;
extern double MaxY;
extern double MinY;
extern double MaxZ;
extern double MinZ;
extern unsigned char Guid4[8];
extern FILE *las_file_in;
extern uint32_t NumPointsByRet[5];
extern uint32_t Resrvd;
extern uint32_t Guid1;
extern uint32_t DataOffset;
extern uint32_t NumVarLenRec;
extern uint32_t NumPointRec;
extern PublicHeaderBlock_t las_header;
extern char FileSig[4];
extern uint16_t Guid2;
extern uint16_t Guid3;
extern uint16_t DateJulian;
extern uint16_t Year;
extern uint16_t HeaderSize;
extern uint16_t PointDataRecLen;
extern unsigned char MajorVers;
extern unsigned char MinorVers;
extern unsigned char PointDataFormatID;

extern void LasFileInit(const char *filename);

#endif
