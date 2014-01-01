/*JWHGPLHERE*/
/*
 * MergeMaster.c
 *
 * Author: James W Hegeman
 *
 */

#include "DLPstd.h"
#include "LdarReader.h"
#include "DistLdarProcFVar.h"
#include "Triangulate.h"
#include "DistributeSlave.h"
#include "MergeMaster.h"

void MergeReceive() {

    int32_t recvcnt[NUM_NODES];
    int32_t *bufp;
    size_t blockcnt;
    int32_t tcnt;
    uint32_t t;
    int c;
    int i;
    int j;
    int ret;
    int readsock;
    int socketcount = NUM_NODES - 1;

    gettimeofday(&t_merge_begin, NULL);

    if ((proc_file_out = fopen("../Pout/processed.out", "w")) == NULL) {
	fprintf(stderr, "Could not open file %s for writing. Exiting.\n", "processed.out");
	fflush(stderr);
	exit(-1);
    }

    if (NUM_NODES > 1) {
	newevents = (struct epoll_event *) Malloc(NUM_NODES * sizeof (struct epoll_event));

	polldesc = epoll_create(NUM_NODES - 1);
	if (polldesc == -1) perror("epoll_create()");

	NetBufCounter[0] = 0;
	for (i = 1; i < NUM_NODES; ++i) {
	    msockevents[i].events = EPOLLIN;
	    msockevents[i].data.fd = msock[i];
	    if (epoll_ctl(polldesc, EPOLL_CTL_ADD, msock[i], &msockevents[i]) == -1) perror("epoll_ctl()");
	    recvcnt[i] = -1;
	    NetBufCounter[i] = 0;
	}

	while (1) {
	    ret = epoll_wait(polldesc, newevents, NUM_NODES - 1, -1);
	    if (ret == -1) perror("epoll_wait()");

	    for (i = 0; i < ret; ++i) {
		readsock = newevents[i].data.fd;
		j = NodeSockIndex[readsock];

		if (recvcnt[j] == -1) {
		    Receive(readsock, &NetBufCounter[j], UINT32_SIZE);
		    recvcnt[j] = 0;
		} else {
		    Receive(readsock, NetworkBuffers[j], TRI_PACKET_LEN);

		    if (NetBufCounter[j] - recvcnt[j] <= TRI_PER_PACKET) {
#if DEBUG >= 1
			fprintf(stderr, "Received %d triangles from node %d\n", recvcnt[j], j);
			fflush(stderr);
#endif
			fwrite(NetworkBuffers[j], (NetBufCounter[j] - recvcnt[j]) * TRI_SIZE, 1, proc_file_out);

			if (epoll_ctl(polldesc, EPOLL_CTL_DEL, readsock, &msockevents[j]) == -1) {
			    perror("epoll_ctl()");
			}
			--socketcount;
		    } else {
			fwrite(NetworkBuffers[j], TRI_PACKET_LEN, 1, proc_file_out);
			recvcnt[j] += TRI_PER_PACKET;
		    }
		}
#if DEBUG >= 2
		if (recvcnt[j] % (TRI_PER_PACKET * 5000) == 0) {
		    fprintf(stderr, "Received %d triangles from node %d\n", recvcnt[j], j);
		    fflush(stderr);
		}
#endif
	    }

	    if (socketcount == 0) break;
	}

	if (close(polldesc) == -1) perror("close()");

	free(newevents);
    }

#if DEBUG >= 1
    for (i = 1; i < NUM_NODES; ++i) {
	fprintf(stderr, "Expected and received %u triangles from node %d\n", NetBufCounter[i], i);
    }
    fflush(stderr);
#endif

    blockcnt = NET_BUF_LEN * XYZ_SIZE;
    tcnt = 0;
    blockcnt /= TRI_SIZE;
    bufp = (int32_t *) NetworkBuffers[0];
    for (c = 0; c < NUM_CELLS; ++c) {
	for (t = 0; t <= NumTri[c]; ++t) {
	    for (i = 0; i < 3; ++i) {
		*bufp++ = lround((TriVertex[c][t][i]->X_c - Xoffset) / Xscale);
		*bufp++ = lround((TriVertex[c][t][i]->Y_c - Yoffset) / Yscale);
		*bufp++ = lround((TriVertex[c][t][i]->Z_c - Zoffset) / Zscale);
	    }
	    ++tcnt;

	    if (tcnt == blockcnt) {
		fwrite(NetworkBuffers[0], tcnt * TRI_SIZE, 1, proc_file_out);
		bufp = (int32_t *) NetworkBuffers[0];
		tcnt = 0;
	    }
	}
    }

    if (tcnt > 0) fwrite(NetworkBuffers[0], tcnt * TRI_SIZE, 1, proc_file_out);

    fflush(proc_file_out);

    if (fclose(proc_file_out)) perror("proc_file_out close");

}
