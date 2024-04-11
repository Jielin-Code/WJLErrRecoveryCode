#include "BPSK_AWGN.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
const double PI = 3.1415926;
// ���ֽ�ת�ɱ���
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
// �ѱ���תΪ�ֽ�
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
			signal[n] = 1.00;
		}
		else {
			signal[n] = -1.00;
		}
	}
}
void BPSK_awgn(double* sended_signal, double* received_signal, double snr, int bitsSize) {
	int n;
	double X, Y;
	double amp, phase;
	double gamma, noise;

	//srand((unsigned)time(NULL));
	// ����һ
	gamma = pow(10.0, snr / 10.0);
	for (n = 0; n < bitsSize; n++) {
		// ������̬�ֲ����������
		X = ((double)rand() + 1.0) / (RAND_MAX + 1.0);
		Y = ((double)rand() + 1.0) / (RAND_MAX + 1.0);
		amp = sqrt(-log(X) / gamma);
		phase = 2.0 * PI * Y;
		noise = amp * cos(phase);
		// ��BPSK�ź����ۼ�����
		received_signal[n] = sended_signal[n] + noise;
	    //printf("%1.2f->%1.2f,", sended_signal[n], received_signal[n]);
	}
	// ������
	//noise = (pow(10.0, snr / 10.0) / 2.0);
	//for (n = 0; n < bitsSize; n++) {
	//	received_signal[n] = sended_signal[n] + sqrt(noise) * (double)rand() / RAND_MAX;
	//	//printf("%1.2f->%1.2f,", sended_signal[n], received_signal[n]);
	//}
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
void bpsk_awgn(unsigned char* InBuff, unsigned char* OutBuff, int length, double snr, long* ber_count)
{
	int i, count = 0;
	unsigned char* InBuff_bits = NULL, * OutBuff_bits = NULL;
	double* sended_signal, * received_signal;
	int bitsLength = length << 3;

	if (bitsLength <= 0) return;

	InBuff_bits = (unsigned char*)malloc(bitsLength);
	OutBuff_bits = (unsigned char*)malloc(bitsLength);
	sended_signal = (double*)malloc(sizeof(double) * bitsLength);
	received_signal = (double*)malloc(sizeof(double) * bitsLength);
	if (InBuff_bits == NULL || OutBuff_bits == NULL || sended_signal == NULL || received_signal == NULL) goto EndAndFree;

	// ���ֽ�ת��Ϊ����
	BPSK_Bytes2Bits(InBuff, length, InBuff_bits);
	BPSK_modulator(InBuff_bits, bitsLength, sended_signal);
	BPSK_awgn(sended_signal, received_signal, snr, bitsLength);
	BPSK_demodulator(received_signal, OutBuff_bits, bitsLength);
	// Ȼ��ѱ���ת��Ϊ�ֽ�
	BPSK_Bits2Bytes(OutBuff_bits, length, OutBuff);
	// ͳ��OutBuff_bits��InBuff_bits�б��ز���
	for (i = 0; i < bitsLength; ++i) {
		if (OutBuff_bits[i] != InBuff_bits[i]) {
			//printf("%d-%d,", OutBuff_bits[i], InBuff_bits[i]);
			count++;
		}
	}
	// ��������ظ���
	*ber_count = count;

EndAndFree:
	// �ͷ���Դ
	if (InBuff_bits) free(InBuff_bits);
	if (OutBuff_bits) free(OutBuff_bits);
	if (sended_signal) free(sended_signal);
	if (received_signal) free(received_signal);
}