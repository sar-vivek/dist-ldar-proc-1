/*JWHGPLHERE*/
/*
 * LdarReader.h
 *
 * Author: James W Hegeman
 *
 */

#ifndef __LDAR_READER_H__
#define __LDAR_READER_H__

typedef struct PublicHeaderBlock {
    char SysID[32];
    char GenSoft[32];
    double Xscale;
    double Yscale;
    double Zscale;
    double Xoffset;
    double Yoffset;
    double Zoffset;
    double MaxX;
    double MinX;
    double MaxY;
    double MinY;
    double MaxZ;
    double MinZ;
    unsigned char Guid4[8];
    uint32_t NumPointsByRet[5];
    uint32_t Resrvd;
    uint32_t Guid1;
    uint32_t DataOffset;
    uint32_t NumVarLenRec;
    uint32_t NumPointRec;
    char FileSig[4];
    uint16_t Guid2;
    uint16_t Guid3;
    uint16_t DateJulian;
    uint16_t Year;
    uint16_t HeaderSize;
    uint16_t PointDataRecLen;
    unsigned char MajorVers;
    unsigned char MinorVers;
    unsigned char PointDataFormatID;
} PublicHeaderBlock_t;

struct LidarPoint {
    double GpsTime;
    int32_t X;
    int32_t Y;
    int32_t Z;
    int32_t StopFlag;
    uint16_t Intensity;
    uint16_t UserBitField;
    unsigned char Classification;
    unsigned char ScanAngleRankLeft;
    unsigned char FileMarker;
    unsigned char RetNum : 3;
    unsigned char NumReturns : 3;
    unsigned char ScanDirFlag : 1;
    unsigned char EdgeOfFlightLine : 1;
} __attribute__ ((packed));

typedef struct LidarPoint LidarPoint_t;

struct LidarPointNode {
    struct LidarPointNode *next;
    double X_c;
    double Y_c;
    double Z_c;
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
