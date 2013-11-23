/*JWHGPLHERE*/
/*
 * DistributeS.c
 *
 * Author: James W Hegeman
 *
 */

#include "DLPstd.h"
#include "LdarReader.h"
#include "DistributeS.h"

void Receive(int sd, void *buffer, size_t len) {
    ssize_t ret;
    ret = recv(sd, buffer, len, 0);
    if (ret != len) perror("recv()");
}

void DistributeS() {

    struct sockaddr_in cli_addr;
    long int ix;
    long int iy;
    int lsock;
    int cli_addr_len;

    lsock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (lsock == -1) perror("socket()");

    memset(svr_addr, 0, sizeof (struct sockaddr_in));
    svr_addr->sin_family = AF_INET;
    svr_addr->sin_port = htons(PORT_BASE);
    svr_addr->sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(lsock, (struct sockaddr *) &svr_addr[0], sizeof (struct sockaddr_in)) == -1) perror("bind()");

    if (listen(lsock, 1) == -1) perror("listen()");
    ssock = accept(lsock, (struct sockaddr *) &cli_addr, (socklen_t *) &cli_addr_len);
    if (ssock == -1) perror("accept()");

    Receive(ssock, &Xscale, DOUBLE_SIZE);
    Receive(ssock, &Yscale, DOUBLE_SIZE);
    Receive(ssock, &Zscale, DOUBLE_SIZE);
    Receive(ssock, &Xoffset, DOUBLE_SIZE);
    Receive(ssock, &Yoffset, DOUBLE_SIZE);
    Receive(ssock, &Zoffset, DOUBLE_SIZE);
    Receive(ssock, &MaxX, DOUBLE_SIZE);
    Receive(ssock, &MinX, DOUBLE_SIZE);
    Receive(ssock, &MaxY, DOUBLE_SIZE);
    Receive(ssock, &MinY, DOUBLE_SIZE);
    Receive(ssock, &MaxZ, DOUBLE_SIZE);
    Receive(ssock, &MinZ, DOUBLE_SIZE);

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

    while (1) {
	Receive(ssock, X_b, XYZ_SIZE);
	if (*X_b == 0 && *Y_b == 0 && *Z_b == 0) break;

	X_c = (*X_b) * Xscale + Xoffset;
	Y_c = (*Y_b) * Yscale + Yoffset;
	Z_c = (*Z_b) * Zscale + Zoffset;
	current->Next = NULL;
	current->X_c = X_c;
	current->Y_c = Y_c;
	current->Z_c = Z_c;


	ix = lround(floor((X_c - MinX) / Xint));
	iy = lround(floor((Y_c - MinY) / Yint));



	if (BinTbl[ix][iy] == NULL
    }

    if (close(lsock) == -1) perror("close()");
}
