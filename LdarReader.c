/*JWHGPLHERE*/
/*
 * LdarReader.c
 *
 * Author: James W Hegeman
 *
 */

#include "DLPstd.h"
#include "LdarReader.h"

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
FILE *las_file_in;
uint32_t NumPointsByRet[5];
uint32_t Resrvd;
uint32_t Guid1;
uint32_t DataOffset;
uint32_t NumVarLenRec;
uint32_t NumPointRec;
PublicHeaderBlock_t las_header;
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

void LasFileInit(const char *filename) {
    uint32_t i;
    uint16_t signature;
    uint16_t reclen;

    if ((las_file_in = fopen(filename, "rb")) == NULL) {
	fprintf(stderr, "Could not open file %s for reading. Exiting.\n", filename);
	fflush(stderr);
	exit(-1);
    }

    fread(&FileSig[0], CHAR_SIZE, 4, las_file_in);
    fread(&Resrvd, UINT32_SIZE, 1, las_file_in);
    fread(&Guid1, UINT32_SIZE, 1, las_file_in);
    fread(&Guid2, UINT16_SIZE, 1, las_file_in);
    fread(&Guid3, UINT16_SIZE, 1, las_file_in);
    fread(&Guid4[0], UCHAR_SIZE, 8, las_file_in);
    fread(&MajorVers, UCHAR_SIZE, 1, las_file_in);
    fread(&MinorVers, UCHAR_SIZE, 1, las_file_in);
    fread(&SysID[0], CHAR_SIZE, 32, las_file_in);
    fread(&GenSoft[0], CHAR_SIZE, 32, las_file_in);
    fread(&DateJulian, UINT16_SIZE, 1, las_file_in);
    fread(&Year, UINT16_SIZE, 1, las_file_in);
    fread(&HeaderSize, UINT16_SIZE, 1, las_file_in);
    fread(&DataOffset, UINT32_SIZE, 1, las_file_in);
    fread(&NumVarLenRec, UINT32_SIZE, 1, las_file_in);
    fread(&PointDataFormatID, UCHAR_SIZE, 1, las_file_in);
    fread(&PointDataRecLen, UINT16_SIZE, 1, las_file_in);
    fread(&NumPointRec, UINT32_SIZE, 1, las_file_in);
    fread(&NumPointsByRet[0], UINT32_SIZE, 5, las_file_in);
    fread(&Xscale, DOUBLE_SIZE, 1, las_file_in);
    fread(&Yscale, DOUBLE_SIZE, 1, las_file_in);
    fread(&Zscale, DOUBLE_SIZE, 1, las_file_in);
    fread(&Xoffset, DOUBLE_SIZE, 1, las_file_in);
    fread(&Yoffset, DOUBLE_SIZE, 1, las_file_in);
    fread(&Zoffset, DOUBLE_SIZE, 1, las_file_in);
    fread(&MaxX, DOUBLE_SIZE, 1, las_file_in);
    fread(&MinX, DOUBLE_SIZE, 1, las_file_in);
    fread(&MaxY, DOUBLE_SIZE, 1, las_file_in);
    fread(&MinY, DOUBLE_SIZE, 1, las_file_in);
    fread(&MaxZ, DOUBLE_SIZE, 1, las_file_in);
    fread(&MinZ, DOUBLE_SIZE, 1, las_file_in);

#if DEBUG == 1
    printf("-------------------------------------\n");
    printf("NumPointRec = %u\n", NumPointRec);
    printf("Xscale = %lg\n", Xscale);
    printf("Yscale = %lg\n", Yscale);
    printf("Zscale = %lg\n", Zscale);
    printf("Xoffset = %lg\n", Xoffset);
    printf("Yoffset = %lg\n", Yoffset);
    printf("Zoffset = %lg\n", Zoffset);
    printf("MaxX = %lg\n", MaxX);
    printf("MinX = %lg\n", MinX);
    printf("MaxY = %lg\n", MaxY);
    printf("MinY = %lg\n", MinY);
    printf("MaxZ = %lg\n", MaxZ);
    printf("MinZ = %lg\n", MinZ);
    printf("-------------------------------------\n\n");
    fflush(stdout);
#endif

    if (NumVarLenRec == 0) {
	if (DataOffset < 2) PERROR("Data Offset error");
	if (fseek(las_file_in, DataOffset - 2, SEEK_SET) != 0)
	    PERROR("Error while preparing to read point data");
    } else {
	if (fseek(las_file_in, DataOffset, SEEK_SET) != 0)
	    PERROR("Error while preparing to read variable length data");
	for (i = 0; i < NumVarLenRec; ++i) {
	    fread(&signature, UINT16_SIZE, 1, las_file_in);
	    if (signature != 0xAABB)
		PERROR("Variable Length Record signature error");
	    fseek(las_file_in, 16 * CHAR_SIZE + UINT16_SIZE, SEEK_CUR);
	    fread(&reclen, UINT16_SIZE, 1, las_file_in);
	    fseek(las_file_in, 32 * CHAR_SIZE + reclen, SEEK_CUR);
	}
    }

    fread(&signature, UINT16_SIZE, 1, las_file_in);
    if (signature != 0xCCDD) PERROR("Point Data signature error");

    return;
}
