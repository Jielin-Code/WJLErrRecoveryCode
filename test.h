#ifndef _TEST_H
#define _TEST_H

#ifdef	__cplusplus
extern "C" {
#endif
	// 生成随机数
	int randEx();

	int QPSK_AWGN_ERRRECOVERY(unsigned int Length, double snr, int printIt);

#ifdef	__cplusplus
}
#endif

#endif