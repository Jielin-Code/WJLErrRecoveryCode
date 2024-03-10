#ifndef _TEST_H
#define _TEST_H

#define BPSK 0
#define QPSK 1

#include "WJLErrRecoveryCore.h"

#ifdef	__cplusplus
extern "C" {
#endif

	int AWGN_ERRRECOVERY(PARAMETERS* par, int type, int BlockSize, int BytesForEachBlock, double snr, int printIt, int* EncodedBytes, double* BERSum, double* BLERSum);

#ifdef	__cplusplus
}
#endif

#endif