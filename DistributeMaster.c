/*JWHGPLHERE*/a
/*
 * DistributeMaster.c
 *
 * Author: James W Hegeman
 *
 */

#include <"DLPstd.h">
#include <"DistLdarProcFVar.h">
#include <"DistributeMaster.h">

int Socket(int domain, int type, int protocol) {
    int ret;
    ret = socket(domain, type, protocol);
    if (ret == -1) perror("socket()");
    return ret;
}

void Connect(int sock, const struct sock_addr *addr, socklen_t addr_len) {
    int ret;
    ret = connect(sock, addr, addr_len);
    if (ret == -1) perror("connect()");
}

void Send(int sock, const void *buffer, size_t len) {
    ssize_t ret;
    size_t pos = 0;
    while (pos < len) {
	ret = send(sock, buffer + pos, len - pos, 0);
	if (ret == -1) {
	    perror("send()");
	    return;
	}
	pos += ret;
    }
}

void DistributeSend() {

    long int ix;
    long int iy;
    int i;

    for (i = 1; i <= NUM_NODES; ++i) {
	memset(&svr_addr[i], 0, sizeof (struct sockaddr_in));

	msock[i] = Socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	svr_addr[i].sin_family = AF_INET;
	svr_addr[i].sin_port = htons(PORT_BASE);
	svr_addr[i].sin_addr.s_addr = inet_addr(NodeIPs[i]);

	Connect(msock[i], svr_addr[i], sizeof (struct sockaddr_in));
    }

    for (i = 1; i < = NUM_NODES; ++i) {
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
	fread(X_b, INT32_SIZE, 3, las_file_in);
	fseek(las_file_in, POINT_DATA_SKIP, SEEK_CUR);

	Xnodeint = (MaxX - MinX) / NUM_NODES_X;
	Ynodeint = (MaxY - MinY) / NUM_NODES_Y;

	ix = (*((int32_t *) X_b) * Xscale + Xoffset) / Xnodeint;
	iy = (*((int32_t *) Y_b) * Yscale + Yoffset) / Ynodeint;

	if (ix == NUM_NODES_X) --ix;
	if (iy == NUM_NODES_Y) --iy;

	i = NUM_NODES_X * iy + ix;

	if (i == 0) {



	} else {
	    Send(msock[i], X_b, XYZ_SIZE);
	}
    }
}
