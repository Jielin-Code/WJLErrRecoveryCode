#include "QPSK_AWGN.h"
#include <stdlib.h>
#include <math.h>
const double pi = 3.1415926;
// ���ֽ�ת�ɱ���
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
// �ѱ���תΪ�ֽ�
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
		// �����������
		r1 = (double)rand() / RAND_MAX;
		r2 = (double)rand() / RAND_MAX;
		if (r1 <= 1.0e-8) {
			r1 = 1.0e-8; //��ֹ����log0�Ĳ���
		}

		amp = sqrt(-log(r1) / gamma);
		phase = 2.0 * pi * r2;
		noise[0] = amp * cos(phase);
		noise[1] = amp * sin(phase);
		// ��QPSK�ź����ۼ�����
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
// qpsk�ź�awgn�ŵ���Ϊ����Ͻ���������㷨�������������������ֽ�
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
	// ���ٶ�ά����
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
	// ����3dB���������Ըպú������������ͬ
	snr += 3.0;
	// ���ֽ�ת��Ϊ����
	Bytes2Bits(InBuff, length, InBuff_bits);
	QPSK_modulator(InBuff_bits, symbolSize, sended_signal);
	awgn(sended_signal, received_signal, snr, symbolSize);
	QPSK_demodulator(received_signal, OutBuff_bits, symbolSize);
	// Ȼ��ѱ���ת��Ϊ�ֽ�
	Bits2Bytes(OutBuff_bits, length, OutBuff);
	// ͳ��OutBuff_bits��InBuff_bits�б��ز���
	for (i = 0; i < bitsLength; ++i) {
		if (OutBuff_bits[i] != InBuff_bits[i]) count++;
	}
	// �ۼ�
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