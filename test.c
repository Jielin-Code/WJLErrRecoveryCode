#include "test.h"
#include "WJLErrRecoveryCore.h"
#include "QPSK_AWGN.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
unsigned char bitOfByteTable[256][8] =
{
	{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,1},{0,0,0,0,0,0,1,0},{0,0,0,0,0,0,1,1},{0,0,0,0,0,1,0,0},{0,0,0,0,0,1,0,1},{0,0,0,0,0,1,1,0},{0,0,0,0,0,1,1,1},	//0~7
	{0,0,0,0,1,0,0,0},{0,0,0,0,1,0,0,1},{0,0,0,0,1,0,1,0},{0,0,0,0,1,0,1,1},{0,0,0,0,1,1,0,0},{0,0,0,0,1,1,0,1},{0,0,0,0,1,1,1,0},{0,0,0,0,1,1,1,1},	//8~15	
	{0,0,0,1,0,0,0,0},{0,0,0,1,0,0,0,1},{0,0,0,1,0,0,1,0},{0,0,0,1,0,0,1,1},{0,0,0,1,0,1,0,0},{0,0,0,1,0,1,0,1},{0,0,0,1,0,1,1,0},{0,0,0,1,0,1,1,1},	//16~23
	{0,0,0,1,1,0,0,0},{0,0,0,1,1,0,0,1},{0,0,0,1,1,0,1,0},{0,0,0,1,1,0,1,1},{0,0,0,1,1,1,0,0},{0,0,0,1,1,1,0,1},{0,0,0,1,1,1,1,0},{0,0,0,1,1,1,1,1},	//24~31
	{0,0,1,0,0,0,0,0},{0,0,1,0,0,0,0,1},{0,0,1,0,0,0,1,0},{0,0,1,0,0,0,1,1},{0,0,1,0,0,1,0,0},{0,0,1,0,0,1,0,1},{0,0,1,0,0,1,1,0},{0,0,1,0,0,1,1,1},	//32~39
	{0,0,1,0,1,0,0,0},{0,0,1,0,1,0,0,1},{0,0,1,0,1,0,1,0},{0,0,1,0,1,0,1,1},{0,0,1,0,1,1,0,0},{0,0,1,0,1,1,0,1},{0,0,1,0,1,1,1,0},{0,0,1,0,1,1,1,1},	//40~47
	{0,0,1,1,0,0,0,0},{0,0,1,1,0,0,0,1},{0,0,1,1,0,0,1,0},{0,0,1,1,0,0,1,1},{0,0,1,1,0,1,0,0},{0,0,1,1,0,1,0,1},{0,0,1,1,0,1,1,0},{0,0,1,1,0,1,1,1},	//48~55
	{0,0,1,1,1,0,0,0},{0,0,1,1,1,0,0,1},{0,0,1,1,1,0,1,0},{0,0,1,1,1,0,1,1},{0,0,1,1,1,1,0,0},{0,0,1,1,1,1,0,1},{0,0,1,1,1,1,1,0},{0,0,1,1,1,1,1,1},	//56~63
	{0,1,0,0,0,0,0,0},{0,1,0,0,0,0,0,1},{0,1,0,0,0,0,1,0},{0,1,0,0,0,0,1,1},{0,1,0,0,0,1,0,0},{0,1,0,0,0,1,0,1},{0,1,0,0,0,1,1,0},{0,1,0,0,0,1,1,1},	//64~71
	{0,1,0,0,1,0,0,0},{0,1,0,0,1,0,0,1},{0,1,0,0,1,0,1,0},{0,1,0,0,1,0,1,1},{0,1,0,0,1,1,0,0},{0,1,0,0,1,1,0,1},{0,1,0,0,1,1,1,0},{0,1,0,0,1,1,1,1},	//72~79
	{0,1,0,1,0,0,0,0},{0,1,0,1,0,0,0,1},{0,1,0,1,0,0,1,0},{0,1,0,1,0,0,1,1},{0,1,0,1,0,1,0,0},{0,1,0,1,0,1,0,1},{0,1,0,1,0,1,1,0},{0,1,0,1,0,1,1,1},	//80~87
	{0,1,0,1,1,0,0,0},{0,1,0,1,1,0,0,1},{0,1,0,1,1,0,1,0},{0,1,0,1,1,0,1,1},{0,1,0,1,1,1,0,0},{0,1,0,1,1,1,0,1},{0,1,0,1,1,1,1,0},{0,1,0,1,1,1,1,1},	//88~95
	{0,1,1,0,0,0,0,0},{0,1,1,0,0,0,0,1},{0,1,1,0,0,0,1,0},{0,1,1,0,0,0,1,1},{0,1,1,0,0,1,0,0},{0,1,1,0,0,1,0,1},{0,1,1,0,0,1,1,0},{0,1,1,0,0,1,1,1},	//96~103
	{0,1,1,0,1,0,0,0},{0,1,1,0,1,0,0,1},{0,1,1,0,1,0,1,0},{0,1,1,0,1,0,1,1},{0,1,1,0,1,1,0,0},{0,1,1,0,1,1,0,1},{0,1,1,0,1,1,1,0},{0,1,1,0,1,1,1,1},	//104~111
	{0,1,1,1,0,0,0,0},{0,1,1,1,0,0,0,1},{0,1,1,1,0,0,1,0},{0,1,1,1,0,0,1,1},{0,1,1,1,0,1,0,0},{0,1,1,1,0,1,0,1},{0,1,1,1,0,1,1,0},{0,1,1,1,0,1,1,1},	//112~119
	{0,1,1,1,1,0,0,0},{0,1,1,1,1,0,0,1},{0,1,1,1,1,0,1,0},{0,1,1,1,1,0,1,1},{0,1,1,1,1,1,0,0},{0,1,1,1,1,1,0,1},{0,1,1,1,1,1,1,0},{0,1,1,1,1,1,1,1},	//120~127
	{1,0,0,0,0,0,0,0},{1,0,0,0,0,0,0,1},{1,0,0,0,0,0,1,0},{1,0,0,0,0,0,1,1},{1,0,0,0,0,1,0,0},{1,0,0,0,0,1,0,1},{1,0,0,0,0,1,1,0},{1,0,0,0,0,1,1,1},	//128~135
	{1,0,0,0,1,0,0,0},{1,0,0,0,1,0,0,1},{1,0,0,0,1,0,1,0},{1,0,0,0,1,0,1,1},{1,0,0,0,1,1,0,0},{1,0,0,0,1,1,0,1},{1,0,0,0,1,1,1,0},{1,0,0,0,1,1,1,1},	//136~143
	{1,0,0,1,0,0,0,0},{1,0,0,1,0,0,0,1},{1,0,0,1,0,0,1,0},{1,0,0,1,0,0,1,1},{1,0,0,1,0,1,0,0},{1,0,0,1,0,1,0,1},{1,0,0,1,0,1,1,0},{1,0,0,1,0,1,1,1},	//144~151
	{1,0,0,1,1,0,0,0},{1,0,0,1,1,0,0,1},{1,0,0,1,1,0,1,0},{1,0,0,1,1,0,1,1},{1,0,0,1,1,1,0,0},{1,0,0,1,1,1,0,1},{1,0,0,1,1,1,1,0},{1,0,0,1,1,1,1,1},	//152~159
	{1,0,1,0,0,0,0,0},{1,0,1,0,0,0,0,1},{1,0,1,0,0,0,1,0},{1,0,1,0,0,0,1,1},{1,0,1,0,0,1,0,0},{1,0,1,0,0,1,0,1},{1,0,1,0,0,1,1,0},{1,0,1,0,0,1,1,1},	//160~167
	{1,0,1,0,1,0,0,0},{1,0,1,0,1,0,0,1},{1,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,1},{1,0,1,0,1,1,0,0},{1,0,1,0,1,1,0,1},{1,0,1,0,1,1,1,0},{1,0,1,0,1,1,1,1},	//168~175
	{1,0,1,1,0,0,0,0},{1,0,1,1,0,0,0,1},{1,0,1,1,0,0,1,0},{1,0,1,1,0,0,1,1},{1,0,1,1,0,1,0,0},{1,0,1,1,0,1,0,1},{1,0,1,1,0,1,1,0},{1,0,1,1,0,1,1,1},	//176~183
	{1,0,1,1,1,0,0,0},{1,0,1,1,1,0,0,1},{1,0,1,1,1,0,1,0},{1,0,1,1,1,0,1,1},{1,0,1,1,1,1,0,0},{1,0,1,1,1,1,0,1},{1,0,1,1,1,1,1,0},{1,0,1,1,1,1,1,1},	//184~191
	{1,1,0,0,0,0,0,0},{1,1,0,0,0,0,0,1},{1,1,0,0,0,0,1,0},{1,1,0,0,0,0,1,1},{1,1,0,0,0,1,0,0},{1,1,0,0,0,1,0,1},{1,1,0,0,0,1,1,0},{1,1,0,0,0,1,1,1},	//192~199
	{1,1,0,0,1,0,0,0},{1,1,0,0,1,0,0,1},{1,1,0,0,1,0,1,0},{1,1,0,0,1,0,1,1},{1,1,0,0,1,1,0,0},{1,1,0,0,1,1,0,1},{1,1,0,0,1,1,1,0},{1,1,0,0,1,1,1,1},	//200~207
	{1,1,0,1,0,0,0,0},{1,1,0,1,0,0,0,1},{1,1,0,1,0,0,1,0},{1,1,0,1,0,0,1,1},{1,1,0,1,0,1,0,0},{1,1,0,1,0,1,0,1},{1,1,0,1,0,1,1,0},{1,1,0,1,0,1,1,1},	//208~215
	{1,1,0,1,1,0,0,0},{1,1,0,1,1,0,0,1},{1,1,0,1,1,0,1,0},{1,1,0,1,1,0,1,1},{1,1,0,1,1,1,0,0},{1,1,0,1,1,1,0,1},{1,1,0,1,1,1,1,0},{1,1,0,1,1,1,1,1},	//216~223
	{1,1,1,0,0,0,0,0},{1,1,1,0,0,0,0,1},{1,1,1,0,0,0,1,0},{1,1,1,0,0,0,1,1},{1,1,1,0,0,1,0,0},{1,1,1,0,0,1,0,1},{1,1,1,0,0,1,1,0},{1,1,1,0,0,1,1,1},	//224~231
	{1,1,1,0,1,0,0,0},{1,1,1,0,1,0,0,1},{1,1,1,0,1,0,1,0},{1,1,1,0,1,0,1,1},{1,1,1,0,1,1,0,0},{1,1,1,0,1,1,0,1},{1,1,1,0,1,1,1,0},{1,1,1,0,1,1,1,1},	//232~239
	{1,1,1,1,0,0,0,0},{1,1,1,1,0,0,0,1},{1,1,1,1,0,0,1,0},{1,1,1,1,0,0,1,1},{1,1,1,1,0,1,0,0},{1,1,1,1,0,1,0,1},{1,1,1,1,0,1,1,0},{1,1,1,1,0,1,1,1},	//240~247
	{1,1,1,1,1,0,0,0},{1,1,1,1,1,0,0,1},{1,1,1,1,1,0,1,0},{1,1,1,1,1,0,1,1},{1,1,1,1,1,1,0,0},{1,1,1,1,1,1,0,1},{1,1,1,1,1,1,1,0},{1,1,1,1,1,1,1,1}		//248~255
};
// �Ժ���Ϊ��λ���������ȷ�����ݵ�����ؾ��Ե����״����
int randEx()
{
	LARGE_INTEGER seed;
	QueryPerformanceFrequency(&seed);
	QueryPerformanceCounter(&seed);
	srand(seed.QuadPart);
	return rand();
}
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
// �ж������ֽ��в�����ظ���
int ErrBits(unsigned char byte1, unsigned char byte2)
{
	int i, j = 0;
	unsigned char tmpbyte = byte1 ^ byte2;
	// ͳ�Ʋ������
	for (i = 7; i >= 0; --i) {
		if ((tmpbyte >> i) & 0x01) {
			j++;
		}
	}
	return j;
}
// ���ڴ�������ݱ�����
int QPSK_AWGN_ERRRECOVERY(unsigned int Length, double snr, int printIt)
{
	int i = 0, j = 0, Limit, RandDigit, count = 0, errbits = 0, tj = 1, ber_count = 0;
	unsigned char tmpByte = 0x00;
	double R, realber = 0.0;
	PARAMETERS* par = NULL;
	WJL_ERRRECOVERY_ENCODER* encoder = NULL;
	WJL_ERRRECOVERY_DECODER* decoder = NULL;
	WJL_ERRRECOVERY_ENCODER** list = NULL;
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
	// ָ��ϵ��������������MAX_NUMBER_OF_0xFFϵ����������ͬ���ڱ���ʱ����MAX_NUMBER_OF_0xFF��������ϵ������Ч
	encoder->par = par;
	decoder->par = par;
	// Ĭ�ϻ��ڵ�ǰ���ݱ���ǰ���ȱ�������0x00��������ʵ��Ӧ��������������2
	encoder->InBytesLength = Length;
	decoder->OutBytesLength = Length;
	// ������澡���ŵĴ�һЩ����Ϊ���ݲ�ͬ��SUBSECTION�����ǲ�һ���ģ�Ϊ��֧��0.25�����ʣ���Ҫ�����ı�����
	encoder->OutBytesLength = (unsigned int)(encoder->InBytesLength * 4);
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

	if (Length <= 1024) {
		if (printIt) printf("������ɵ�ԭʼ����(The randomly generated raw data)��%d\n", encoder->InBytesLength);
		// �����������
		for (i = 0; i < Length; ++i) {
			encoder->InBytesArray[i] = randEx() % 256; //rand() % 256; // Set_In_BUFF[i];
			if (printIt) printf("%03d->%02X,", i, encoder->InBytesArray[i]);
		}
		if (printIt)printf("\n");
	}
	else {
		// �����������
		for (i = 0; i < Length; ++i) {
			encoder->InBytesArray[i] = randEx() % 256; //rand() % 256; // Set_In_BUFF[i];
		}
	}

	/********************�������벿��**********************/
	WJLErrRecoveryEncoder(encoder);

	// ʵ�ʱ�������
	R = (double)encoder->InBytesLength / (double)encoder->OutBytesIndex;
	if (printIt) {
		if (Length <= 1024) {
			printf("�������ֽ�(Encoded bytes)��%d\n", encoder->OutBytesIndex);
			for (i = 0; i < encoder->OutBytesIndex; ++i) {
				printf("%03d->%02X,", i, encoder->OutBytesArray[i]);
			}
			printf("\n");
			printf("����ǰ��%d, �����%d��ʵ������R = %1.6f, ���۱�������R=-1/log2(1/3)=1/1.5849625=0.63092975\n", encoder->InBytesLength, encoder->OutBytesIndex, R);
			printf("Before coding:%d, After coding:%d��Actual code rate:R = %1.6f, Theoretical coding code rate:R=-1/log2(1/3)=1/1.5849625=0.63092975\n", encoder->InBytesLength, encoder->OutBytesIndex, R);
			printf("\n");
		}
		else {
			printf("����ǰ��%d, �����%d��ʵ������R = %1.6f, ���۱�������R=-1/log2(1/3)=1/1.5849625=0.63092975\n", encoder->InBytesLength, encoder->OutBytesIndex, R);
			printf("Before coding:%d, After coding:%d��Actual code rate:R = %1.6f, Theoretical coding code rate:R=-1/log2(1/3)=1/1.5849625=0.63092975\n", encoder->InBytesLength, encoder->OutBytesIndex, R);
			printf("\n");
		}
	}
	// ��������Ľ��ͨ��Qpsk_awgn������������������
	Qpsk_awgn(encoder->OutBytesArray, decoder->InBytesArray, encoder->OutBytesIndex, snr, &ber_count);
	if (printIt) {
		printf("ʵ���������Ϊ(The actual bit error rate is)��ber = %1.6f\n", (double)ber_count / ((double)encoder->OutBytesIndex * 8));
	}
	// �趨��������ĳ���
	decoder->InBytesLength = encoder->OutBytesIndex;

	if (printIt) {
		if (Length <= 1024) {
			printf("�ŵ����洫��õ����ֽ�(Bytes obtained from the channel simulation transmission)��%d\n", encoder->OutBytesIndex);
			for (i = 0; i < encoder->OutBytesIndex; ++i) {
				if (encoder->OutBytesArray[i] != decoder->InBytesArray[i]) {
					errbits = ErrBits(encoder->OutBytesArray[i], decoder->InBytesArray[i]);
					printf("%03d->%02X-*%02X-%d,", i, decoder->InBytesArray[i], encoder->OutBytesArray[i], errbits);
					//printf("*%02X-%02X %d,", decoder->InBytesArray[i], encoder->OutBytesArray[i], errbits);
					//printf("%03d->%02X,", i, decoder->InBytesArray[i]);
				}
				else {
					printf("%03d->%02X,", i, decoder->InBytesArray[i]);
					//printf("%02X,", decoder->InBytesArray[i]);
				}
			}
			printf("\n");
		}
		else {
			printf("\n");
		}
	}

	/********************�������벿��**********************/
	WJLErrRecoveryDecoder(decoder, list);

	if (printIt) {
		if (Length <= 1024) {
			printf("�������ֽ�(Decoded byte)��%d\n", decoder->OutBytesIndex - 1);
			for (i = 0; i < Length; ++i) {
				printf("%03d->%02X,", i, decoder->OutBytesArray[i]);
			}
			printf("\n");
		}
	}
	// ����Ƿ��д���
	for (i = 0; i < Length; ++i) {
		if (encoder->InBytesArray[i] != decoder->OutBytesArray[i]) {
			printf("*******************����λ��(Error location)��%d\n", i);
			goto Err;
		}
	}
	// �ͷ���Դ
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
	printf("���д����Ѿ�����������������ȷ��\nAll errors have been corrected and are decoded correctly!\n");
	return 1;
Err:
	// �ͷ���Դ
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
	printf("\n�������\nDecoding error!\n");
	return 0;
}
// �������һ��0-255��ֵA�������ж���������ֵB��A�ı��ز���Ӧ�ôﵽ4��������
void RandombyQualification()
{
	int i = 0, j = 0, k = 0, l = 0, sign = 1;
	int _ucBytes[256];
	int _iucBytes[256];
	int tmp;
	int err, uperr = 0;
	// ��ʼ��_ucBytes��_iucBytes
	for (i = 0; i < 256; ++i) {
		_ucBytes[i] = -1;
		_iucBytes[i] = -1;
	}
	// �����������
	while (j < 256) {
		tmp = randEx() % 256;
		sign = 1;
		for (i = 0; i < 256; ++i) {
			// �ҵ�����ͬ��ֵ
			if (_ucBytes[i] == tmp) {
				sign = 0;
				break;
			}
		}
		//�ж��Ƿ�������������tmp�����ڵ�_ucBytes[i]����
		for (i = 0; i < 256; ++i) {
			// �ҵ�����ͬ��ֵ
			if (_ucBytes[i] == -1) {
				break;
			}
		}
		if (i != 0 && abs((int)_ucBytes[i - 1] - tmp) < 32) {
			sign = 0;
		}
		// û���ҵ���ͬ��ֵ
		if (sign) {
			// Ȼ���ж���������е�ֵ�Ƿ�������ز���Ҫ��
			for (k = 0; k < 256; ++k) {
				if ((unsigned char)k == tmp) {
					err = ErrBits((unsigned char)j, tmp);
					// �ټ���k��tmp�Ƿ�������ز���Ҫ��abs(j - tmp) >= 8
					if (err >= 3) {//err >= 2 && 
						_ucBytes[j] = tmp;
						_iucBytes[tmp] = j;
						printf("0x%02X 0x%02X,", _ucBytes[j], _iucBytes[tmp]);
						j++;
						uperr = err;
					}
					else {
						sign = 0;
						break;
					}
				}
			}
		}
	}
	printf("\n����\n");
	// �ȴ�ӡ_ucBytes
	for (i = 0; i < 256; ++i) {
		if (i % 16 == 0) {
			printf("\n");
		}
		printf("0x%02X,", _ucBytes[i]);
	}
	printf("\n");
	printf("\n");
	printf("�����\n");
	// Ȼ����������󲢴�ӡ
	for (i = 0; i < 256; ++i) {
		if (i % 16 == 0) {
			printf("\n");
		}
		printf("0x%02X,", _iucBytes[i]);
	}
	printf("\n");
	printf("\n");
	return 0;
}