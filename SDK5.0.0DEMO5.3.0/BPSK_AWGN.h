#pragma once
#ifndef _BPSK_AWGN_H
#define _BPSK_AWGN_H

#ifdef	__cplusplus
extern "C" {
#endif

	void Bpsk_awgn(unsigned char* InBuff, unsigned char* OutBuff, int length, double snr, long* ber_count);

#ifdef	__cplusplus
}
#endif
#endif