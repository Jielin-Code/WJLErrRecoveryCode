#ifndef _TEST_H
#define _TEST_H

#define BPSK 0
#define QPSK 1

#include "WJLErrRecoveryCore.h"

#ifdef	__cplusplus
extern "C" {
#endif
	// ���ݲ���ͬ���ֽڣ��ǳ���Ҫ�������趨�̶��ļ���CODE_LENGTH��Q_LENGTH��ֵ���Ҳ����ͨ������ĺ�����Ӧ����CODE_LENGTH��Q_LENGTH
	void SynchronizationBytes(WJL_ALGORITHM_PARAMETERS* par);

	// û������أ���������������������
	int NoErrByBytesArrayLength(WJL_ALGORITHM_PARAMETERS* par);

	int AWGN_ERRRECOVERY(WJL_ALGORITHM_PARAMETERS* par, int model, int BlockSize, double snr, int printIt, int* EncodedBytesCount, double* BERSum, double* BLERSum);
	
	// ����ĳЩ����������Ϊʲô�����ԭ��
	int CHECK_ERRRECOVERY(WJL_ALGORITHM_PARAMETERS* par);
#ifdef	__cplusplus
}
#endif

#endif