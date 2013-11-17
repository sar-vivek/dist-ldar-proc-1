/*JWHGPLHERE*/
/*
 * DistributeM.c
 *
 * Author: James W Hegeman
 *
 */

#include "DLPstd.h"
#include "LdarReader.h"
#include "DistributeM.h"

/*
#define MY_INDEX 0

char NodeIPs[20][16];

LidarPointNode_t *BinTbl[NUM_BINS_X][NUM_BINS_Y];
LidarPointNode_t *CurTbl[NUM_BINS_X][NUM_BINS_Y];
double BinU1[NUM_BINS_X][NUM_BINS_Y];
double BinU2[NUM_BINS_X][NUM_BINS_Y];
uint32_t BinCnt[NUM_BINS_X][NUM_BINS_Y];
LidarPointNode_t *PntTbl;
double *Z2;
int NodeID;
int8_t *FiltTbl;

int main(int argc, char *argv[]) {
    struct timeval t_start;
    struct timeval t_end;
    double t_diff;
    double Xint;
    double Yint;
    double X_c;
    double Y_c;
    double Z_c;
    double u1;
    double u2;
    LidarPointNode_t *current;
    double *current2;
    long int ix;
    long int iy;
    int32_t X;
    int32_t Y;
    int32_t Z;
    uint32_t count;
    uint32_t n;
*/

void Send(int sd, const void *buffer, size_t len) {

    ssize_t ret;
    ssize_t sent = 0;

    while (sent < len) {
	ret = send(sd, buffer + sent, len - sent, 0);
	if (ret == -1) perror("send()");
	sent += ret;
    }
}

void DistributeM() {

    long int ix;
    long int iy;
    int i;

    for (i = 1; i < NUM_NODES; ++i) {
	memset(&svr_addr[i], 0, sizeof (struct sockaddr_in));

	msock[i] = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (msock[i] == -1) perror("socket()");

	svr_addr[i].sin_family = AF_INET;
	svr_addr[i].sin_port = htons(PORT_BASE);
	inet_pton(AF_INET, NodeIPs[i], &svr_addr[i].sin_addr.s_addr);
	if (connect(msock[i], (struct sockaddr *) &svr_addr[i], sizeof (struct sockaddr_in)) == -1) perror("connect()");
    }

    for (i = 1; i < NUM_NODES; ++i) {
	Send(msock[i], &Xscale, DOUBLE_SIZE);
	Send(msock[i], &Yscale, DOUBLE_SIZE);
	Send(msock[i], &Zscale, DOUBLE_SIZE);
	Send(msock[i], &Xoffset, DOUBLE_SIZE);
	Send(msock[i], &Yoffset, DOUBLE_SIZE);
	Send(msock[i], &Zoffset, DOUBLE_SIZE);
	Send(msock[i], &MaxX, DOUBLE_SIZE);
	Send(msock[i], &MinX, DOUBLE_SIZE);
	Send(msock[i], &MaxY, DOUBLE_SIZE);
	Send(msock[i], &MinY, DOUBLE_SIZE);
	Send(msock[i], &MaxZ, DOUBLE_SIZE);
	Send(msock[i], &MinZ, DOUBLE_SIZE);
    }

    while (count-- > 0) {
	fread(X_b, XYZ_SIZE, 1, las_file_in);
	fseek(las_file_in, POINT_DATA_SKIP, SEEK_CUR);

	ix = lround(floor(Xratio * (int32_t *) X_b + Xdiff));
	iy = lround(floor(Yratio * (int32_t *) Y_b + Ydiff));

	Send(msock[iy * NUM_NODES_X + ix], X_b, 3 * INT32_SIZE);
    }

}
