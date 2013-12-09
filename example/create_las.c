/*
 * Author : Vivek B Sardeshmukh
 *          James W Hegeman
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char SysID[32]="0";;
char GenSoft[32]="0";
double Xscale=1.0;
double Yscale=1.0;
double Zscale=1.0;
double Xoffset=0;
double Yoffset=0;
double Zoffset=0;
double MaxX;
double MinX;
double MaxY;
double MinY;
double MaxZ;
double MinZ;
unsigned char Guid4[8]="0";
FILE *las_file_in;
FILE *las_file_out;
uint32_t NumPointsByRet[5]={0, 0, 0, 0, 0};
uint32_t Resrvd=0;
uint32_t Guid1=0;
uint32_t DataOffset=0;
uint32_t NumVarLenRec=0;
uint32_t NumPointRec;
char FileSig[4]="0";
uint16_t Guid2=0;
uint16_t Guid3=0;
uint16_t DateJulian=0;
uint16_t Year=0;
uint16_t HeaderSize;
uint16_t PointDataRecLen;
unsigned char MajorVers;
unsigned char MinorVers;
unsigned char PointDataFormatID;


void WriteHeader(const char *filename) {
    double zero;
    int i;
    uint32_t buffer;
    uint16_t signature;

    if ((las_file_out = fopen(filename, "wb")) == NULL) {
	fprintf(stderr, "Could not open file %s for writing. Exiting.\n", filename);
	fflush(stderr);
	fclose(las_file_in);
	exit(-1);
    }

    fwrite((void *) &FileSig[0], sizeof (char), 4, las_file_out);
    fwrite((void *) &Resrvd, sizeof (uint32_t), 1, las_file_out);
    fwrite((void *) &Guid1, sizeof (uint32_t), 1, las_file_out);
    fwrite((void *) &Guid2, sizeof (uint16_t), 1, las_file_out);
    fwrite((void *) &Guid3, sizeof (uint16_t), 1, las_file_out);
    fwrite((void *) &Guid4[0], sizeof (unsigned char), 8, las_file_out);
    fwrite((void *) &MajorVers, sizeof (unsigned char), 1, las_file_out);
    fwrite((void *) &MinorVers, sizeof (unsigned char), 1, las_file_out);
    fwrite((void *) &SysID[0], sizeof (char), 32, las_file_out);
    fwrite((void *) &GenSoft[0], sizeof (char), 32, las_file_out);
    fwrite((void *) &DateJulian, sizeof (uint16_t), 1, las_file_out);
    fwrite((void *) &Year, sizeof (uint16_t), 1, las_file_out);
    fwrite((void *) &HeaderSize, sizeof (uint16_t), 1, las_file_out);
    buffer = HeaderSize + 2;
    fwrite((void *) &buffer, sizeof (uint32_t), 1, las_file_out);
    buffer = 0;
    fwrite((void *) &buffer, sizeof (uint32_t), 1, las_file_out);
    fwrite((void *) &PointDataFormatID, sizeof (unsigned char), 1, las_file_out);
    fwrite((void *) &PointDataRecLen, sizeof (uint16_t), 1, las_file_out);
    fwrite((void *) &buffer, sizeof (uint32_t), 1, las_file_out);
    for (i = 0; i < 5; ++i) {
	fwrite((void *) &buffer, sizeof (uint32_t), 1, las_file_out);
    }
    fwrite((void *) &Xscale, sizeof (double), 1, las_file_out);
    fwrite((void *) &Yscale, sizeof (double), 1, las_file_out);
    fwrite((void *) &Zscale, sizeof (double), 1, las_file_out);
    fwrite((void *) &Xoffset, sizeof (double), 1, las_file_out);
    fwrite((void *) &Yoffset, sizeof (double), 1, las_file_out);
    fwrite((void *) &Zoffset, sizeof (double), 1, las_file_out);
    zero = 0;
    for (i = 0; i < 6; ++i) {
	fwrite((void *) &zero, sizeof (double), 1, las_file_out);
    }
    signature = 0xCCDD;
    fwrite((void *) &signature, sizeof (uint16_t), 1, las_file_out);

    return;
}

void WritePoint(double x, double y) {

    int32_t px,py,pz;
    uint16_t zero=0;
    unsigned char buf='0';
    px=(int32_t)x;
    py=(int32_t)y;
    pz=rand()%100 + 400;
    fwrite((void *) &px, sizeof (int32_t), 1, las_file_out);
    fwrite((void *) &py, sizeof (int32_t), 1, las_file_out);
    fwrite((void *) &pz, sizeof (int32_t), 1, las_file_out);
    fwrite((void *) &zero, sizeof (uint16_t), 1, las_file_out);
    fwrite((void *) &buf, sizeof (unsigned char), 1, las_file_out);
    fwrite((void *) &buf, sizeof (unsigned char), 1, las_file_out);
    fwrite((void *) &buf, sizeof (unsigned char), 1, las_file_out);
    fwrite((void *) &buf, sizeof (unsigned char), 1, las_file_out);
    fwrite((void *) &zero, sizeof (uint16_t), 1, las_file_out);
    fwrite((void *) &x, sizeof (double), 1, las_file_out);
    
    if(x>MaxX) MaxX=x;
    if(y>MaxY) MaxY=y;
    if(pz>MaxZ) MaxZ=pz;
    if(x<MinX) MinX=x;
    if(y<MinY) MinY=y;
    if(pz<MinZ) MinZ=pz;
    NumPointRec++;
    return;
}

void UpdateHeader(void) {
    long offset;

    offset = 68 * sizeof (char) + 11 * sizeof (unsigned char) +
	     6 * sizeof (uint16_t) + 4 * sizeof (uint32_t);
    if (fseek(las_file_out, offset, SEEK_SET) != 0) {
	fprintf(stderr, "Could not seek to update LAS header. Number of ");
	fprintf(stderr, "point records may be incorrect. MaxX, MinX, MaxY, ");
	fprintf(stderr, "MinY, MaxZ, and MinZ may also be incorrect.\n");
	fflush(stderr);
	fseek(las_file_out, 0, SEEK_END);
	return;
    }

    fwrite((void *) &NumPointRec, sizeof (uint32_t), 1, las_file_out);

    offset = 68 * sizeof (char) + 11 * sizeof (unsigned char) + 6 * sizeof (uint16_t) +
	     10 * sizeof (uint32_t) + 6 * sizeof (double);
    if (fseek(las_file_out, offset, SEEK_SET) != 0) {
	fprintf(stderr, "Could not seek to update LAS header. MaxX, MinX, ");
	fprintf(stderr, "MaxY, MinY, MaxZ, and MinZ may be incorrect.\n");
	fflush(stderr);
	fseek(las_file_out, 0, SEEK_END);
	return;
    }

    fwrite((void *) &MaxX, sizeof (double), 1, las_file_out);
    fwrite((void *) &MinX, sizeof (double), 1, las_file_out);
    fwrite((void *) &MaxY, sizeof (double), 1, las_file_out);
    fwrite((void *) &MinY, sizeof (double), 1, las_file_out);
    fwrite((void *) &MaxZ, sizeof (double), 1, las_file_out);
    fwrite((void *) &MinZ, sizeof (double), 1, las_file_out);

    fseek(las_file_out, 0, SEEK_END);

    return;
}

void LasClose(void) {
    fclose(las_file_out);

    return;
}


int main(int argc, char* argv[]){
    FILE* fin;
    double x,y;

    fin=fopen(argv[1], "r");
    if(fin==NULL){
        perror("input");
        exit(-1);
    }

    WriteHeader(argv[2]);
    /*read first point and init all variables*/
    fscanf(fin, "%f %f\n", &x, &y);
    MinX=x;
    MaxX=x;
    MinY=y;
    MaxY=y;
    NumPointRec=1;
    srand(time(NULL));
    MinZ=500;
    MaxZ=400;
    WritePoint(x,y);
    while(fscanf(fin, "%f %f\n", &x, &y)!=EOF){
        WritePoint(x,y);
    }
    UpdateHeader();
    LasClose();
    return 0;
}

