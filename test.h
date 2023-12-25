#ifndef _TEST_H
#define _TEST_H

#define BPSK 0
#define QPSK 1

#ifdef	__cplusplus
extern "C" {
#endif
	// 生成随机数
	int randEx();

	int AWGN_ERRRECOVERY(int type, unsigned int Length, double snr, int printIt);

#ifdef	__cplusplus
}
#endif

#endif