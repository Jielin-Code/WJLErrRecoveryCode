#include "QPSK_AWGN.h"
#include <stdlib.h>
#include <math.h>
const double pi = 3.1415926;
// 把字节转成比特
void Bytes2Bits(unsigned char* bytes, int length, unsigned char* bits)
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
void Bits2Bytes(unsigned char* bits, int length, unsigned char* bytes)
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
void QPSK_modulator(unsigned char* bits, int symbolSize, double** signal) {
	int n, bit1, bit2, symbol;
	int bin2sym[2][2] = {
		{ 0, 1},
		{ 3, 2}
	};
	double sym2sgnl[4][2] = {
		{ 1 / sqrt(2), 1 / sqrt(2)},
		{-1 / sqrt(2), 1 / sqrt(2)},
		{-1 / sqrt(2),-1 / sqrt(2)},
		{ 1 / sqrt(2),-1 / sqrt(2)}
	};

	for (n = 0; n < symbolSize; n++) {
		bit1 = bits[n * 2];
		bit2 = bits[n * 2 + 1];
		symbol = bin2sym[bit1][bit2];
		signal[n][0] = sym2sgnl[symbol][0];
		signal[n][1] = sym2sgnl[symbol][1];
	}
}
void awgn(double** sended_signal, double** received_signal, double snr, int symbolSize) {
	int n;
	double r1, r2;
	double amp, phase;
	double gamma, noise[2];

	gamma = pow(10.0, snr / 10.0);

	srand((unsigned)time(NULL));

	for (n = 0; n < symbolSize; n++) {
		// 产生随机噪声
		r1 = (double)rand() / RAND_MAX;
		r2 = (double)rand() / RAND_MAX;
		if (r1 <= 1.0e-8) {
			r1 = 1.0e-8; //防止出现log0的操作
		}

		amp = sqrt(-log(r1) / gamma);
		phase = 2.0 * pi * r2;
		noise[0] = amp * cos(phase);
		noise[1] = amp * sin(phase);
		// 在QPSK信号上累加噪声
		received_signal[n][0] = sended_signal[n][0] + noise[0];
		received_signal[n][1] = sended_signal[n][1] + noise[1];
	}
}
void QPSK_demodulator(double** signal, unsigned char* bits, int symbolSize) {
	int n, symbol;
	int sym2bin[4][2] = {
		{0,0},
		{0,1},
		{1,1},
		{1,0}
	};

	for (n = 0; n < symbolSize; n++) {
		if (signal[n][0] >= 0.0) {
			symbol = (signal[n][1] >= 0.0) ? 0 : 3;
		}
		else {
			symbol = (signal[n][1] >= 0.0) ? 1 : 2;
		}

		bits[n * 2] = sym2bin[symbol][0];
		bits[n * 2 + 1] = sym2bin[symbol][1];
	}
}
// qpsk信号awgn信道，为了配合杰林码纠错算法，这里输入和输出都是字节
void Qpsk_awgn(unsigned char* InBuff, unsigned char* OutBuff, int length, double snr, long* ber_count)
{
	int i, count = 0;
	unsigned char* InBuff_bits = NULL, * OutBuff_bits = NULL;
	double** sended_signal, ** received_signal;
	int bitsLength = length << 3;
	int symbolSize = bitsLength >> 1;

	if (bitsLength <= 0 || symbolSize <= 0) return;

	InBuff_bits = (unsigned char*)malloc(bitsLength);
	OutBuff_bits = (unsigned char*)malloc(bitsLength);
	sended_signal = (double**)malloc(sizeof(double*) * symbolSize);
	received_signal = (double**)malloc(sizeof(double*) * symbolSize);

	if (InBuff_bits == NULL || OutBuff_bits == NULL || sended_signal == NULL || received_signal == NULL) return;
	// 开辟二维数组
	for (i = 0; i < symbolSize; ++i) {
		sended_signal[i] = (double*)malloc(sizeof(double) * 2);
		if (sended_signal[i]) {
			sended_signal[i][0] = 0.0;
			sended_signal[i][1] = 0.0;
		}
		received_signal[i] = (double*)malloc(sizeof(double) * 2);
		if (received_signal[i]) {
			received_signal[i][0] = 0.0;
			received_signal[i][1] = 0.0;
		}
	}
	// 补齐3dB，经过测试刚好和理论误比特相同
	snr += 3.0;
	// 将字节转换为比特
	Bytes2Bits(InBuff, length, InBuff_bits);
	QPSK_modulator(InBuff_bits, symbolSize, sended_signal);
	awgn(sended_signal, received_signal, snr, symbolSize);
	QPSK_demodulator(received_signal, OutBuff_bits, symbolSize);
	// 然后把比特转换为字节
	Bits2Bytes(OutBuff_bits, length, OutBuff);
	// 统计OutBuff_bits与InBuff_bits中比特差异
	for (i = 0; i < bitsLength; ++i) {
		if (OutBuff_bits[i] != InBuff_bits[i]) count++;
	}
	// 累加
	*ber_count = count;

	if (InBuff_bits) free(InBuff_bits);
	if (OutBuff_bits) free(OutBuff_bits);
	if (sended_signal) {
		for (i = 0; i < symbolSize; ++i) if (sended_signal[i]) free(sended_signal[i]);
		free(sended_signal);
	}
	if (received_signal) {
		for (i = 0; i < symbolSize; ++i) if (received_signal[i]) free(received_signal[i]);
		free(received_signal);
	}
}