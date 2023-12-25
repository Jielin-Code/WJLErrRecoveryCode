#include "BPSK_AWGN.h"
#include <stdlib.h>
#include <math.h>
const double PI = 3.1415926;
// 把字节转成比特
void BPSK_Bytes2Bits(unsigned char* bytes, int length, unsigned char* bits)
{
	int i, j, k = 0;
	for (i = 0; i < length; ++i) {
		for (j = 7; j >= 0; --j) {
			bits[k] = (bytes[i] >> j) & 0x01;
			k++;
		}
	}
}
// 把比特转为字节
void BPSK_Bits2Bytes(unsigned char* bits, int length, unsigned char* bytes)
{
	int i, j, k = 0;
	for (i = 0; i < length; ++i) {
		bytes[i] = 0x00;
		for (j = 7; j >= 0; --j) {
			bytes[i] <<= 1;
			bytes[i] |= bits[k];
			k++;
		}
	}
}
void BPSK_modulator(unsigned char* bits, int bitsSize, double* signal) {
	int n;
	for (n = 0; n < bitsSize; n++) {
		if (bits[n] == 0x00) {
			signal[n] = 1.0;
		}
		else {
			signal[n] = -1.0;
		}
	}
}
void BPSK_awgn(double* sended_signal, double* received_signal, double snr, int bitsSize) {
	int n;
	double X, Y;
	double amp, phase;
	double gamma, noise;

	gamma = pow(10.0, snr / 10.0);

	srand((unsigned)time(NULL));

	for (n = 0; n < bitsSize; n++) {
		// 产生随机噪声
		X = (double)rand() / RAND_MAX;
		Y = (double)rand() / RAND_MAX;
		if (X <= 1.0e-8) {
			X = 1.0e-8; //防止出现log0的操作
		}
		amp = sqrt(-log(X) / gamma);
		phase = 2.0 * PI * Y;
		noise = amp * sin(phase);
		// 在BPSK信号上累加噪声
		received_signal[n] = sended_signal[n] + noise;
	}
}
void BPSK_demodulator(double* signal, unsigned char* bits, int bitsSize) {
	int n;
	for (n = 0; n < bitsSize; n++) {
		if (signal[n] < 0) {
			bits[n] = 0x01;
		}
		else {
			bits[n] = 0x00;
		}
	}
}
void Bpsk_awgn(unsigned char* InBuff, unsigned char* OutBuff, int length, double snr, long* ber_count)
{
	int i, count = 0;
	unsigned char* InBuff_bits = NULL, * OutBuff_bits = NULL;
	double** sended_signal, ** received_signal;
	int bitsLength = length << 3;

	if (bitsLength <= 0) return;

	InBuff_bits = (unsigned char*)malloc(bitsLength);
	OutBuff_bits = (unsigned char*)malloc(bitsLength);
	sended_signal = (double*)malloc(sizeof(double) * bitsLength);
	received_signal = (double*)malloc(sizeof(double) * bitsLength);
	if (InBuff_bits == NULL || OutBuff_bits == NULL || sended_signal == NULL || received_signal == NULL) goto EndAndFree;

	// 将字节转换为比特
	BPSK_Bytes2Bits(InBuff, length, InBuff_bits);
	BPSK_modulator(InBuff_bits, bitsLength, sended_signal);
	BPSK_awgn(sended_signal, received_signal, snr, bitsLength);
	BPSK_demodulator(received_signal, OutBuff_bits, bitsLength);
	// 然后把比特转换为字节
	BPSK_Bits2Bytes(OutBuff_bits, length, OutBuff);
	// 统计OutBuff_bits与InBuff_bits中比特差异
	for (i = 0; i < bitsLength; ++i) {
		if (OutBuff_bits[i] != InBuff_bits[i]) count++;
	}
	// 返回误比特个数
	*ber_count = count;

EndAndFree:
	// 释放资源
	if (InBuff_bits) free(InBuff_bits);
	if (OutBuff_bits) free(OutBuff_bits);
	if (sended_signal) free(sended_signal);
	if (received_signal) free(received_signal);
}