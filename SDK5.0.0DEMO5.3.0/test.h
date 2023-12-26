#ifndef _TEST_H
#define _TEST_H

#define BPSK 0
#define QPSK 1

#ifdef	__cplusplus
extern "C" {
#endif

	int AWGN_ERRRECOVERY(int type, int BlockSize, int BytesForEachBlock, double snr, int printIt, double* BERSum, double* BLERSum);

#ifdef	__cplusplus
}
#endif

#endif