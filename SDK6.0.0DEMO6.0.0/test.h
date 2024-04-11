#ifndef _TEST_H
#define _TEST_H

#define BPSK 0
#define QPSK 1

#include "WJLErrRecoveryCore.h"

#ifdef	__cplusplus
extern "C" {
#endif
	// 根据参数同步字节，非常重要，可以设定固定的几个CODE_LENGTH和Q_LENGTH的值查表，也可以通过下面的函数适应所有CODE_LENGTH和Q_LENGTH
	void SynchronizationBytes(WJL_ALGORITHM_PARAMETERS* par);

	// 没有误比特，可以用来测试无损编解码
	int NoErrByBytesArrayLength(WJL_ALGORITHM_PARAMETERS* par);

	int AWGN_ERRRECOVERY(WJL_ALGORITHM_PARAMETERS* par, int model, int BlockSize, double snr, int printIt, int* EncodedBytesCount, double* BERSum, double* BLERSum);
	
	// 检验某些二进制序列为什么出错的原因
	int CHECK_ERRRECOVERY(WJL_ALGORITHM_PARAMETERS* par);
#ifdef	__cplusplus
}
#endif

#endif