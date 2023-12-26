#include "test.h"
#include "WJLErrRecoveryCore.h"
#include "QPSK_AWGN.h"
#include "BPSK_AWGN.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
// Q����
double Q(double EbN0)
{
	return 0.5 * erfc(sqrt(2 * EbN0) / sqrt(2.0));
}
//------------------------------------------------------------------------------------------------------
// ����ļ���������Ҫ����Բ�ͬ��ϵͳ���з��棬��Ҫ�ǻ��ڲ�ͬ���ŵ����ͣ����벻ͬ������ȼ������Ӧ��BER
// BPSK��һ����ʽ
double AWGN_BPSK_BER(double EbN0_dB)
{
	// �ο����ף�����. AWGN�ŵ���BPSK�����ʷ������[J]. ΢�����,2021,42(3):23-26. DOI:10.3969/j.issn.1002-2279.2021.03.006.
	double EbN0 = pow(10, EbN0_dB / 10);
	// �����������
	return 0.5 * erfc(sqrt(EbN0));
	//return Q(EbN0);
}
// ���ڴ�������ݱ�����
int AWGN_ERRRECOVERY(int type, int BlockSize, int BytesForEachBlock, double snr, int printIt, double* BERSum, double* BLERSum)
{
	int i = 0, j = 0, k = 0, ber_count = 0, bler_count = 0, EncodedSize = 0;
	unsigned char tmpByte = 0x00;
	double R, realber = 0.0;
	PARAMETERS* par = NULL;
	WJL_ERRRECOVERY_ENCODER* encoder = NULL;
	WJL_ERRRECOVERY_DECODER* decoder = NULL;
	WJL_ERRRECOVERY_ENCODER** list = NULL;
	unsigned char* BytesForEncode = NULL, * BytesForDecode = NULL;
	unsigned char* SendBuff = NULL, * ReceiveBuff = NULL;
	// ���������������㷨�ĺ���
	par = (PARAMETERS*)malloc(sizeof(PARAMETERS));
	if (par == NULL) {
		goto Err;
	}
	// Ȼ�������ò���
	par->DECODER_LIST_SIZE = 64;
	par->MAX_NUMBER_OF_0xFF = 3;
	par->START_LIMIT = 10;
	par->END_LIMIT = 8;
	par->FIRST_ERR_COMPARE_LIMIT = 3;
	par->BLOCK_ERR_COMPARE_LIMIT = 4;
	par->ERRBITS_LIMIT = 5;
	par->MAXIMUM_TRAVERSAL_TIMES = 8;

	// ���ٻ���
	encoder = (WJL_ERRRECOVERY_ENCODER*)malloc(sizeof(WJL_ERRRECOVERY_ENCODER));
	decoder = (WJL_ERRRECOVERY_DECODER*)malloc(sizeof(WJL_ERRRECOVERY_DECODER));
	list = (WJL_ERRRECOVERY_DECODER**)malloc(par->DECODER_LIST_SIZE * sizeof(WJL_ERRRECOVERY_DECODER*));
	if (encoder == NULL || decoder == NULL || list == NULL) {
		goto Err;
	}
	// ֱ�ӿ���DECODER_LIST_SIZE��list
	for (i = 0; i < par->DECODER_LIST_SIZE; ++i) {
		list[i] = (WJL_ERRRECOVERY_DECODER*)malloc(sizeof(WJL_ERRRECOVERY_DECODER));
		if (list[i] == NULL) {
			goto Err;
		}
	}
	// ����������ݵĻ���
	BytesForEncode = (unsigned char*)malloc(BytesForEachBlock * BlockSize);
	BytesForDecode = (unsigned char*)malloc(BytesForEachBlock * BlockSize);
	// �����㹻��ķ��ͻ��棬�����ÿ֡�����洢��λ�ã�һ�㿪���㹻��Ŀռ�
	SendBuff = (unsigned char*)malloc(BytesForEachBlock * BlockSize * 16);
	// ����SendBuff��ͬ��С�Ļ��棬ͨ������-AWGN����-�����Ļ���
	ReceiveBuff = (unsigned char*)malloc(BytesForEachBlock * BlockSize * 16);
	// �ų��쳣
	if (BytesForEncode == NULL || BytesForDecode == NULL || SendBuff == NULL || ReceiveBuff == NULL) goto Err;

	// ָ��ϵ��������������MAX_NUMBER_OF_0xFFϵ����������ͬ���ڱ���ʱ����MAX_NUMBER_OF_0xFF��������ϵ������Ч
	encoder->par = par;
	decoder->par = par;

	// Ĭ�ϻ��ڵ�ǰ���ݱ���ǰ���ȱ�������0x00��������ʵ��Ӧ��������������2
	encoder->InBytesLength = BytesForEachBlock;
	decoder->OutBytesLength = BytesForEachBlock;
	// ������澡���ŵĴ�һЩ����Ϊ���ݲ�ͬ��SUBSECTION�����ǲ�һ���ģ�Ϊ��֧��0.25�����ʣ���Ҫ�����ı�����
	encoder->OutBytesLength = (unsigned int)(encoder->InBytesLength * 16);
	// ����Ļ���
	encoder->InBytesArray = (unsigned char*)malloc(encoder->InBytesLength);
	encoder->OutBytesArray = (unsigned char*)malloc(encoder->OutBytesLength);
	// ����Ļ���
	decoder->InBytesArray = (unsigned char*)malloc(encoder->OutBytesLength);
	decoder->OutBytesArray = (unsigned char*)malloc(encoder->InBytesLength);
	decoder->BytesArray = (unsigned char*)malloc(decoder->par->DECODER_LIST_SIZE);
	decoder->InBytesArraySection = (unsigned char*)malloc(decoder->par->DECODER_LIST_SIZE);
	if (encoder->InBytesArray == NULL || encoder->OutBytesArray == NULL || decoder->InBytesArray == NULL || decoder->OutBytesArray == NULL || decoder->BytesArray == NULL || decoder->InBytesArraySection == NULL) {
		goto Err;
	}

	srand((unsigned)time(NULL));
	if (printIt) printf("������ɴ���������(Random bytes for waiting encoding)��%d\n", BytesForEachBlock * BlockSize);
	// �������BytesForEachBlock * BlockSize���ֽ�
	for (i = 0; i < BytesForEachBlock * BlockSize; ++i) {
		BytesForEncode[i] = rand() % 256;
		if (printIt) printf("%02X,", BytesForEncode[i]);
	}
	if (printIt)printf("\n");

	/********************�������벿��**********************/
	for (i = 0; i < BlockSize; ++i) {
		// �����ǰ�ÿ֡�����ݴ洢��encoder->InBytesArray
		for (j = 0; j < BytesForEachBlock; ++j) {
			encoder->InBytesArray[j] = BytesForEncode[i * BytesForEachBlock + j];
		}
		// ���뵱ǰ��֡
		WJLErrRecoveryEncoder(encoder);
		// ����ÿ֡����Ľ�������볤����һ�µģ����Է���EncodedSize
		if (EncodedSize == 0) EncodedSize = encoder->OutBytesIndex;
		// Ϊ�˷�ֹ���ܳ��ֲ�ͬ���ȵı�������������һ���ж�
		else if (EncodedSize != encoder->OutBytesIndex) {
			printf("�������ÿ֡���볤�Ȳ�һ�£�����ʱ����ֲ����ֹ���룡\nRandom data has inconsistent encoding length per frame! Decoding occurs with an error! End-coding!\n");
			goto Err;
		}
		// ��ÿ֡�ı�����encoder->OutBytesArray�洢��SendBuff��
		for (j = 0; j < encoder->OutBytesIndex; ++j) {
			SendBuff[k] = encoder->OutBytesArray[j];
			k++;
		}
	}

	// ʵ�ʱ�������
	R = ((double)BytesForEachBlock * (double)BlockSize) / (double)k;
	if (printIt) {
		printf("�������ֽ�(Encoded bytes)��%d\n", k);
		for (i = 0; i < k; ++i) {
			printf("%02X,", SendBuff[i]);
		}
		printf("\n");
	}
	printf("����ǰ��%d, �����%d��ʵ�ʱ�������R = %1.6f\n���۱�������R = -1/log2(1/3) = 1/1.5849625 = 0.63092975\n", BytesForEachBlock * BlockSize, k, R);
	printf("Before coding:%d  After coding:%d Actual code rate:R = %1.6f\nTheoretical code rate:R = -1/log2(1/3) = 1/1.5849625 = 0.63092975\n", BytesForEachBlock * BlockSize, k, R);
	printf("\n");

	// ������ͨ������-awgn����-������棬�����㹻����������ʲ����б���
	if (type == QPSK) Qpsk_awgn(SendBuff, ReceiveBuff, k, snr, &ber_count);
	if (type == BPSK) Bpsk_awgn(SendBuff, ReceiveBuff, k, snr, &ber_count);
	// �ۼ�ʵ���ϵ��������������ó�ƽ��������awgn��Ӧ���������
	*BERSum = *BERSum + ((double)ber_count / ((double)k * 8.0));
	printf("ʵ���������Ϊ(The actual bit error rate is)��ber = %1.6f�������������(Theoretical bit error rate)��ber = %1.6f\n\n", (double)ber_count / ((double)k * 8), AWGN_BPSK_BER(snr));
	if (printIt) {
		printf("����-AWGN����-����������ֽ�(Error bytes after Modulation-AWGN transmission-demodulation)��\n");
		for (i = 0; i < k; ++i) {
			if (ReceiveBuff[i] != SendBuff[i]) {
				printf("%d:(S)%02X->(R)%02X,", i, SendBuff[i], ReceiveBuff[i]);
			}

		}
		printf("\n");
	}
	/********************�������벿��**********************/
	k = 0;
	for (i = 0; i < BlockSize; ++i) {
		// ����EncodedSize��ȡһ֡����
		for (j = 0; j < EncodedSize; ++j) {
			decoder->InBytesArray[j] = ReceiveBuff[k];
			k++;
		}
		// һ��Ҫ��ʼ������������ĳ���
		decoder->InBytesLength = EncodedSize;
		// ���뵱ǰ��֡
		WJLErrRecoveryDecoder(decoder, list);
		// �ѽ����Ľ��decoder->OutBytesArray�洢��BytesForDecode��
		for (j = 0; j < BytesForEachBlock; ++j) {
			BytesForDecode[i * BytesForEachBlock + j] = decoder->OutBytesArray[j];
		}
		// �жϵ�ǰ�Ŀ��Ƿ���ڴ���
		for (j = 0; j < BytesForEachBlock; ++j) {
			if (BytesForDecode[i * BytesForEachBlock + j] != BytesForEncode[i * BytesForEachBlock + j]) {
				// �ۼƴ���Ŀ�������������������
				bler_count++;
				if (printIt) printf("��%d�����ʧ�ܣ��������\nThe %d block error correction failure, decoding error!\n", i, i);
				// ���ٴ洢��ǰ�Ŀ�
				break;
			}
		}
	}
	// �ۼ��ϵ�ǰ�������
	*BLERSum = *BLERSum + (double)bler_count / (double)BlockSize;
	if (printIt) {
		printf("��ǰ��������%d��������������������������%d, �����Ϊ��BLER = %1.6f\n", BlockSize, bler_count, (double)bler_count / (double)BlockSize);
		printf("A total of %d block random numbers is currently tested, Number of error blocks after error correction and decoding:%d, BLER = %1.6f\n", BlockSize, bler_count, (double)bler_count / (double)BlockSize);
	}

	if (printIt) {
		printf("�������ֽ�(Decoded bytes)��\n");
		for (i = 0; i < BytesForEachBlock * BlockSize; ++i) {
			printf("%02X,", BytesForDecode[i]);
		}
		printf("\n");
	}

	// �ͷ���Դ
	if (BytesForEncode) free(BytesForEncode);
	if (BytesForDecode) free(BytesForDecode);
	if (SendBuff) free(SendBuff);
	if (ReceiveBuff) free(ReceiveBuff);
	if (encoder) {
		if (encoder->InBytesArray)free(encoder->InBytesArray);
		if (encoder->OutBytesArray)free(encoder->OutBytesArray);
		free(encoder);
	}
	if (decoder) {
		if (list) {
			for (i = 0; i < decoder->par->DECODER_LIST_SIZE; ++i) if (list[i]) free(list[i]);
			free(list);
		}
		if (decoder->InBytesArray)free(decoder->InBytesArray);
		if (decoder->OutBytesArray)free(decoder->OutBytesArray);
		if (decoder->BytesArray)free(decoder->BytesArray);
		if (decoder->InBytesArraySection)free(decoder->InBytesArraySection);
		free(decoder);
	}
	if (par) free(par);
	// ����bler_count�����ж���֡��
	if (bler_count > 0) {
		printf("�������Decoding error!\n");
		return 0;
	}
	else {
		printf("���д����Ѿ�����������������ȷ��All errors have been corrected and are decoded correctly!\n");
		return 1;
	}
Err:
	// �ͷ���Դ
	if (BytesForEncode) free(BytesForEncode);
	if (BytesForDecode) free(BytesForDecode);
	if (SendBuff) free(SendBuff);
	if (ReceiveBuff) free(ReceiveBuff);
	if (encoder) {
		if (encoder->InBytesArray)free(encoder->InBytesArray);
		if (encoder->OutBytesArray)free(encoder->OutBytesArray);
		free(encoder);
	}
	if (decoder) {
		if (list) {
			for (i = 0; i < decoder->par->DECODER_LIST_SIZE; ++i) if (list[i]) free(list[i]);
			free(list);
		}
		if (decoder->InBytesArray)free(decoder->InBytesArray);
		if (decoder->OutBytesArray)free(decoder->OutBytesArray);
		if (decoder->BytesArray)free(decoder->BytesArray);
		if (decoder->InBytesArraySection)free(decoder->InBytesArraySection);
		free(decoder);
	}
	if (par) free(par);
	printf("�������Decoding error!\n");
	return 0;
}