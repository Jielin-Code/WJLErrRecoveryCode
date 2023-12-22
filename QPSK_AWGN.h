#pragma once
#ifndef _QPSK_AWGN_H
#define _QPSK_AWGN_H

#ifdef	__cplusplus
extern "C" {
#endif

	void Qpsk_awgn(unsigned char* InBuff, unsigned char* OutBuff, int length, double snr, long* ber_count);

#ifdef	__cplusplus
}
#endif
#endif