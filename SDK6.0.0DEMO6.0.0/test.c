#include "test.h"
#include "BPSK_AWGN.h"
#include "QPSK_AWGN.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// �Ƿ���������ز���������1.0dB��Χ�ڣ�0�����ƣ�1����
#define LIMIT_MODEL 0

void SynchronizationBytes(WJL_ALGORITHM_PARAMETERS* par)
{
	int i, j;
	WJL_ERRRECOVERY_ENCODER* encoder1 = NULL;
	WJL_ERRRECOVERY_ENCODER* encoder2 = NULL;
	// ��ֹ����Ĳ����д���
	if (par->CODE_LENGTH <= 0 || par->LIST_SIZE <= 0) return 0;
	encoder1 = (WJL_ERRRECOVERY_ENCODER*)malloc(sizeof(WJL_ERRRECOVERY_ENCODER));
	encoder2 = (WJL_ERRRECOVERY_ENCODER*)malloc(sizeof(WJL_ERRRECOVERY_ENCODER));
	if (encoder1 == NULL || encoder2 == NULL) {
		goto Err;
	}
	encoder1->par = par;
	encoder2->par = par;
	encoder1->OutBytesLength = encoder2->OutBytesLength = encoder1->InBytesLength = encoder2->InBytesLength = par->CODE_LENGTH * 16;

	encoder1->InBytesArray = (unsigned char*)malloc(encoder1->InBytesLength);
	encoder1->OutBytesArray = (unsigned char*)malloc(encoder1->OutBytesLength);
	encoder2->InBytesArray = (unsigned char*)malloc(encoder2->InBytesLength);
	encoder2->OutBytesArray = (unsigned char*)malloc(encoder2->OutBytesLength);
	if (encoder1->InBytesArray == NULL || encoder1->OutBytesArray == NULL || encoder2->InBytesArray == NULL || encoder2->OutBytesArray == NULL) {
		goto Err;
	}
	// ��encoder1����par->CODE_LENGTH��0x00��ȫ0x00��ȫ0xFF���������˵ı�Ե����
	for (i = 0; i < par->CODE_LENGTH; ++i) encoder1->InBytesArray[i] = 0x00;
	WJLErrRecoveryEncoder(encoder1);
	// ��encoder2����par->CODE_LENGTH��0xFF
	for (i = 0; i < par->CODE_LENGTH; ++i) encoder2->InBytesArray[i] = 0xFF;
	WJLErrRecoveryEncoder(encoder2);
	// ���ݾ�̬����
	if (encoder1->OutBytesIndex != encoder2->OutBytesIndex) {
		printf("��̬�������۳�����\n");
		goto Err;
	}
	// �Ƚ�encoder1->OutBytesArray��encoder2->OutBytesArrayĩβ�ֽ�
	j = 0;
	for (i = encoder1->OutBytesIndex - 1; i >= 0; --i) {
		if (encoder1->OutBytesArray[i] != encoder2->OutBytesArray[i]) {
			break;
		}
		j++;
	}
	// Ȼ��ĩβj���ֽڴ洢��par->SynchronizationBytes��
	par->SynchronizationLength = 0;
	for (i = encoder1->OutBytesIndex - j; i < encoder1->OutBytesIndex; ++i) {
		par->SynchronizationBytes[par->SynchronizationLength] = encoder1->OutBytesArray[i];
		par->SynchronizationLength++;
	}
Err:
	// �ͷ���Դ
	if (encoder1) {
		if (encoder1->InBytesArray)free(encoder1->InBytesArray);
		if (encoder1->OutBytesArray)free(encoder1->OutBytesArray);
		free(encoder1);
	}
	if (encoder2) {
		if (encoder2->InBytesArray)free(encoder2->InBytesArray);
		if (encoder2->OutBytesArray)free(encoder2->OutBytesArray);
		free(encoder2);
	}
}


// �޴�����µı��������
int NoErrByBytesArrayLength(WJL_ALGORITHM_PARAMETERS* par)
{
	int i = 0;
	WJL_ERRRECOVERY_ENCODER* encoder = NULL;
	WJL_ERRRECOVERY_DECODER* decoder = NULL;
	WJL_ERRRECOVERY_DECODER** list = NULL;
	unsigned int OutBUFF1_Length = 0, OutBUFF2_Length = 0;
	// ��ֹ����Ĳ����д���
	if (par->CODE_LENGTH <= 0 || par->LIST_SIZE <= 0) return 0;
	// ���ٻ���
	encoder = (WJL_ERRRECOVERY_ENCODER*)malloc(sizeof(WJL_ERRRECOVERY_ENCODER));
	decoder = (WJL_ERRRECOVERY_DECODER*)malloc(sizeof(WJL_ERRRECOVERY_DECODER));
	list = (WJL_ERRRECOVERY_DECODER**)malloc(par->LIST_SIZE * sizeof(WJL_ERRRECOVERY_DECODER*));
	if (encoder == NULL || decoder == NULL || list == NULL) {
		goto Err;
	}

	// ֱ�ӿ���LIST_SIZE��list
	for (i = 0; i < par->LIST_SIZE; ++i) {
		list[i] = (WJL_ERRRECOVERY_DECODER*)malloc(sizeof(WJL_ERRRECOVERY_DECODER));
		if (list[i] == NULL) {
			goto Err;
		}
	}
	// ָ��ϵ��������������MAX_NUMBER_OF_0xFFϵ����������ͬ���ڱ���ʱ����MAX_NUMBER_OF_0xFF��������ϵ������Ч
	encoder->par = par;
	decoder->par = par;

	decoder->OutBytesLength = encoder->InBytesLength = par->CODE_LENGTH * 16;
	// ���������ֵ��Ϊ��֧��1���ֽڵı����룬����������encoder->InBytesLength��4��
	encoder->OutBytesLength = encoder->InBytesLength;

	// ����Ļ���
	encoder->InBytesArray = (unsigned char*)malloc(encoder->InBytesLength);
	encoder->OutBytesArray = (unsigned char*)malloc(encoder->OutBytesLength);
	// ����Ļ���
	decoder->InBytesArray = (unsigned char*)malloc(encoder->InBytesLength);
	decoder->OutBytesArray = (unsigned char*)malloc(encoder->InBytesLength);
	decoder->BytesArray = (unsigned char*)malloc(encoder->InBytesLength);
	decoder->wfp = (WJL_FUNCTION_PARAMETERS*)malloc(sizeof(WJL_FUNCTION_PARAMETERS));
	if (encoder->InBytesArray == NULL || encoder->OutBytesArray == NULL || decoder->InBytesArray == NULL || decoder->OutBytesArray == NULL || decoder->BytesArray == NULL || decoder->wfp == NULL) {
		goto Err;
	}

	// �����������
	for (i = 0; i < par->CODE_LENGTH; ++i) {
		encoder->InBytesArray[i] = rand() % 256;
		printf("%02X,", encoder->InBytesArray[i]);
	}
	printf("\n");


	/********************�������벿��**********************/
	WJLErrRecoveryEncoder(encoder);


	// encoder->OutBytesIndexΪʵ��������ֽڳ��ȣ���encoder->OutBytesArray�е��ֽڸ��Ƹ�decoder->InBytesArray
	memcpy(decoder->InBytesArray, encoder->OutBytesArray, encoder->OutBytesIndex);
	decoder->InBytesLength = encoder->OutBytesIndex;

	printf("�������ֽڣ�%d\n", encoder->OutBytesIndex);
	for (i = 0; i < encoder->OutBytesIndex; ++i) {
		printf("%02X,", decoder->InBytesArray[i]);
	}
	printf("\n");
	printf("����ǰ��%d, �����%d��ʵ������R = %1.6f, ���۱�������R=-1/log_2(1/3)=1/1.5849625=0.63092975\n", par->CODE_LENGTH, encoder->OutBytesIndex, (double)par->CODE_LENGTH / (double)encoder->OutBytesIndex);
	printf("\n");

	/********************�������벿��**********************/
	WJLErrRecoveryDecoder(decoder, list);

	// ����Ƿ��д���
	for (i = 0; i < par->CODE_LENGTH; ++i) {
		if (encoder->InBytesArray[i] != decoder->OutBytesArray[i]) {
			goto Err;
		}
	}

	printf("�������ֽڣ�%d\n", decoder->OutBytesIndex - 1);
	for (i = 0; i < par->CODE_LENGTH; ++i) {
		printf("%02X,", decoder->OutBytesArray[i]);
	}
	printf("\n");

	// �ͷ���Դ
	if (encoder) {
		if (encoder->InBytesArray)free(encoder->InBytesArray);
		if (encoder->OutBytesArray)free(encoder->OutBytesArray);
		free(encoder);
	}
	if (list) {
		for (i = 0; i < decoder->par->LIST_SIZE; ++i) if (list[i]) free(list[i]);
		free(list);
	}
	if (decoder) {
		if (decoder->InBytesArray)free(decoder->InBytesArray);
		if (decoder->OutBytesArray)free(decoder->OutBytesArray);
		if (decoder->BytesArray)free(decoder->BytesArray);
		if (decoder->wfp)free(decoder->wfp);
		free(decoder);
	}
	if (par) free(par);
	return 1;
Err:
	// �ͷ���Դ
	if (encoder) {
		if (encoder->InBytesArray)free(encoder->InBytesArray);
		if (encoder->OutBytesArray)free(encoder->OutBytesArray);
		free(encoder);
	}
	if (list) {
		for (i = 0; i < decoder->par->LIST_SIZE; ++i) if (list[i]) free(list[i]);
		free(list);
	}
	if (decoder) {
		if (decoder->InBytesArray)free(decoder->InBytesArray);
		if (decoder->OutBytesArray)free(decoder->OutBytesArray);
		if (decoder->BytesArray)free(decoder->BytesArray);
		if (decoder->wfp)free(decoder->wfp);
		free(decoder);
	}
	if (par) free(par);
	return 0;
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
int AWGN_ERRRECOVERY(WJL_ALGORITHM_PARAMETERS* par, int model, int BlockSize, double snr, int printIt, int* EncodedBytesCount, double* BERSum, double* BLERSum)
{
	int i = 0, j = 0, k = 0, ber_count = 0, bler_count = 0, EncodedSize = 0, BytesCount = 0;
	unsigned char tmpByte = 0x00;
	double R, realber = 0.0;
	WJL_ERRRECOVERY_ENCODER* encoder = NULL;
	WJL_ERRRECOVERY_DECODER* decoder = NULL;
	WJL_ERRRECOVERY_DECODER** list = NULL;
	unsigned char* BytesForEncode = NULL, * BytesForDecode = NULL;
	unsigned char* SendBuff = NULL, * ReceiveBuff = NULL;
	
	if (par->CODE_LENGTH <= 0 || par->LIST_SIZE <= 0 || BlockSize <= 0) return 0;

	printf("SNR = %1.2f\n", snr);
	printf("CODE_LENGTH = %d\n", par->CODE_LENGTH);
	printf("START_LIMIT = %d\n", par->START_LIMIT);
	printf("END_LIMIT = %d\n", par->END_LIMIT);
	printf("ERRBITS_LIMIT = %d\n", par->ERRBITS_LIMIT);

	// ���ٻ���
	encoder = (WJL_ERRRECOVERY_ENCODER*)malloc(sizeof(WJL_ERRRECOVERY_ENCODER));
	decoder = (WJL_ERRRECOVERY_DECODER*)malloc(sizeof(WJL_ERRRECOVERY_DECODER));
	list = (WJL_ERRRECOVERY_DECODER**)malloc(par->LIST_SIZE * sizeof(WJL_ERRRECOVERY_DECODER*));
	if (encoder == NULL || decoder == NULL || list == NULL) {
		goto Err;
	}
	// ֱ�ӿ���LIST_SIZE��list
	for (i = 0; i < par->LIST_SIZE; ++i) {
		list[i] = (WJL_ERRRECOVERY_DECODER*)malloc(sizeof(WJL_ERRRECOVERY_DECODER));
		if (list[i] == NULL) {
			goto Err;
		}
	}
	// ����������ݵĻ���
	BytesForEncode = (unsigned char*)malloc(par->CODE_LENGTH * BlockSize);
	BytesForDecode = (unsigned char*)malloc(par->CODE_LENGTH * BlockSize);
	// �����㹻��ķ��ͻ��棬�����ÿ֡�����洢��λ�ã�һ�㿪���㹻��Ŀռ�
	SendBuff = (unsigned char*)malloc(par->CODE_LENGTH * BlockSize * 16);
	// ����SendBuff��ͬ��С�Ļ��棬ͨ������-AWGN����-�����Ļ���
	ReceiveBuff = (unsigned char*)malloc(par->CODE_LENGTH * BlockSize * 16);
	// �ų��쳣
	if (BytesForEncode == NULL || BytesForDecode == NULL || SendBuff == NULL || ReceiveBuff == NULL) goto Err;

	// ָ��ϵ��������������MAX_NUMBER_OF_0xFFϵ����������ͬ���ڱ���ʱ����MAX_NUMBER_OF_0xFF��������ϵ������Ч
	encoder->par = par;
	decoder->par = par;

	// ��ʼ������
	decoder->OutBytesLength = decoder->InBytesLength = encoder->OutBytesLength = encoder->InBytesLength = par->CODE_LENGTH * 16;

	// ����Ļ���
	encoder->InBytesArray = (unsigned char*)malloc(encoder->InBytesLength);
	encoder->OutBytesArray = (unsigned char*)malloc(encoder->OutBytesLength);
	// ����Ļ���
	decoder->InBytesArray = (unsigned char*)malloc(decoder->InBytesLength);
	decoder->OutBytesArray = (unsigned char*)malloc(decoder->InBytesLength);
	decoder->BytesArray = (unsigned char*)malloc(decoder->InBytesLength); 
	decoder->wfp = (WJL_FUNCTION_PARAMETERS*)malloc(sizeof(WJL_FUNCTION_PARAMETERS));
	if (encoder->InBytesArray == NULL || encoder->OutBytesArray == NULL || decoder->InBytesArray == NULL || decoder->OutBytesArray == NULL || decoder->BytesArray == NULL || decoder->wfp == NULL) {
		goto Err;
	}

	if (printIt) printf("������ɴ������ֽ�(Random bytes for waiting encoding)��%d\n", par->CODE_LENGTH * BlockSize);

	// �������par->CODE_LENGTH * BlockSize���ֽ�
	for (i = 0; i < par->CODE_LENGTH * BlockSize; ++i) {
		BytesForEncode[i] = rand() % 256;
		if (printIt) printf("%02X,", BytesForEncode[i]);
	}
	if (printIt)printf("\n");

	/********************�������벿��**********************/
	BytesCount = 0;
	for (i = 0; i < BlockSize; ++i) {
		// �����ǰ�ÿ֡�����ݴ洢��encoder->InBytesArray
		for (j = 0; j < par->CODE_LENGTH; ++j) {
			encoder->InBytesArray[j] = BytesForEncode[i * par->CODE_LENGTH + j];
		}
		// ���뵱ǰ��֡
		WJLErrRecoveryEncoder(encoder);
		// �ۼӱ������ֽ���
		BytesCount += encoder->OutBytesIndex;
		// ����ÿ֡����Ľ������̬���ʱ��볤����һ�µģ����Է���EncodedSize
		if (EncodedSize == 0) EncodedSize = encoder->OutBytesIndex;
		// ����һ�¾�̬�����ǲ��Ǳ��볤��һ��
		else if (EncodedSize != encoder->OutBytesIndex) {
			printf("�������ÿ֡���볤�Ȳ�һ�£�����ʱ����ֲ����ֹ���룡\nRandom data has inconsistent encoding length per frame! Decoding occurs with an error! End-coding!\n");
			goto Err;
		}
		// ��ÿ֡�ı�����encoder->OutBytesArray�洢��SendBuff��
		for (j = 0; j < (int)encoder->OutBytesIndex; ++j) {
			SendBuff[k] = encoder->OutBytesArray[j];
			k++;
		}
	}

	// ʵ�ʱ�������
	R = ((double)par->CODE_LENGTH * (double)BlockSize) / (double)k;
	if (printIt) {
		printf("�������ֽ�(Encoded bytes)��%d\n", k);
		for (i = 0; i < k; ++i) {
			printf("0x%02X,", SendBuff[i]);
		}
		printf("\n");
	}
	printf("����ǰ��%d, �����%d��ʵ�ʱ�������R = %1.6f, ���۱�������R = -1/log2(1/3) = 1/1.5849625 = 0.63092975\n", par->CODE_LENGTH * BlockSize, k, R);
	printf("Before coding:%d, After coding:%d, Actual code rate:R = %1.6f, Theoretical code rate:R = -1/log2(1/3) = 1/1.5849625 = 0.63092975\n", par->CODE_LENGTH * BlockSize, k, R);
	printf("\n");

	// ������ͨ������-awgn����-������棬�����㹻����������ʲ����б���
	if (model == BPSK) bpsk_awgn(SendBuff, ReceiveBuff, k, snr, &ber_count);
	else if (model == QPSK) Qpsk_awgn(SendBuff, ReceiveBuff, k, snr, &ber_count);

	// ��0.5dB����
	if (LIMIT_MODEL) {
		// ����AWGN_BPSK_BER(snr - 0.5)��Ϊ�����������snr�ķ���
		if ((double)ber_count / ((double)k * 8) >= AWGN_BPSK_BER(snr - 0.5) || (double)ber_count / ((double)k * 8) <= AWGN_BPSK_BER(snr + 0.5)) {
			bler_count = -1;
			goto Invalid;
		}
		else {
			*EncodedBytesCount = *EncodedBytesCount + BytesCount;
			// �ۼ�ʵ���ϵ��������������ó�ƽ��������awgn��Ӧ���������
			*BERSum = *BERSum + ber_count;
		}
	}
	else {
		*EncodedBytesCount = *EncodedBytesCount + BytesCount;
		// �ۼ�ʵ���ϵ��������������ó�ƽ��������awgn��Ӧ���������
		*BERSum = *BERSum + ber_count;
	}
	printf("��ǰ֡�������Ϊ(The current frame bit error rate is)��ber = %1.8f\n�����������(Theoretical bit error rate)��ber = %1.8f\n\n", (double)ber_count / ((double)k * 8), AWGN_BPSK_BER(snr));
	if (printIt) {
		printf("����-AWGN����-����������ֽ�(Error bytes after Modulation-AWGN transmission-demodulation)��\n");
		for (i = 0; i < k; ++i) {
			// ��ӡ��������ֽڣ������������㷨�Ƿ�����˾���
			if (ReceiveBuff[i] != SendBuff[i]) {
				printf("%d:(S)%02X->(R)%02X %d,", i, SendBuff[i], ReceiveBuff[i], ErrBits(SendBuff[i], ReceiveBuff[i]));
				//printf("0x%02X,", ReceiveBuff[i]);
			}
			else {
				printf("0x%02X,", ReceiveBuff[i]);
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
		for (j = 0; j < par->CODE_LENGTH; ++j) {
			BytesForDecode[i * par->CODE_LENGTH + j] = decoder->OutBytesArray[j];
		}
		// �жϵ�ǰ�Ŀ��Ƿ���ڴ���
		for (j = 0; j < par->CODE_LENGTH; ++j) {
			if (BytesForDecode[i * par->CODE_LENGTH + j] != BytesForEncode[i * par->CODE_LENGTH + j]) {
				if (printIt) printf("%d->%02X %02X\n", i * par->CODE_LENGTH + j, BytesForEncode[i * par->CODE_LENGTH + j], BytesForDecode[i * par->CODE_LENGTH + j]);
				// �ۼƴ���Ŀ�������������������
				bler_count++;
				if (printIt) printf("��%d�����ʧ�ܣ��������\nThe %d block error correction failure, decoding error!\n", i, i);
				// ����ط�����ֱ������ѭ��������bler_count�ᱻ��μ���
				break;
			}
		}
	}
	// �ۼ��ϵ�ǰ�������
	*BLERSum = *BLERSum + bler_count;
	if (printIt) {
		printf("��ǰ֡����%d��������������������������%d\n", BlockSize, bler_count);
		printf("The current frame has a %d block random data, Number of error blocks after error correction and decoding:%d\n", BlockSize, bler_count);
	}

	if (printIt) {
		printf("�������ֽ�(Decoded bytes)��\n");
		for (i = 0; i < par->CODE_LENGTH * BlockSize; ++i) {
			printf("%02X,", BytesForDecode[i]);
		}
		printf("\n");
	}
Invalid:
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
			for (i = 0; i < decoder->par->LIST_SIZE; ++i) if (list[i]) free(list[i]);
			free(list);
		}
		if (decoder->InBytesArray)free(decoder->InBytesArray);
		if (decoder->OutBytesArray)free(decoder->OutBytesArray);
		if (decoder->BytesArray)free(decoder->BytesArray);
		if (decoder->wfp)free(decoder->wfp);
		free(decoder);
	}
	//if (par) free(par);
	// ����bler_count�����ж���֡�ʣ���Ϊÿ֡���ܰ����ܶ��
	if (bler_count > 0) {
		printf("�������Decoding error!\n");
		return 0;
	}
	else if(bler_count == 0){
		printf("���д����Ѿ�����������������ȷ��All errors have been corrected and are decoded correctly!\n");
		return 1;
	}
	else {
		// bler_count < 0 �����
		return 2;
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
			for (i = 0; i < decoder->par->LIST_SIZE; ++i) if (list[i]) free(list[i]);
			free(list);
		}
		if (decoder->InBytesArray)free(decoder->InBytesArray);
		if (decoder->OutBytesArray)free(decoder->OutBytesArray);
		if (decoder->BytesArray)free(decoder->BytesArray);
		if (decoder->wfp)free(decoder->wfp);
		free(decoder);
	}
	//if (par) free(par);
	printf("�������Decoding error!\n");
	return 0;
}

// ���ڴ�������ݱ�����
int CHECK_ERRRECOVERY(WJL_ALGORITHM_PARAMETERS* par)
{
	int i = 0, j = 0, k = 0, ber_count = 0, bler_count = 0, EncodedSize = 0, BytesCount = 0;
	unsigned char tmpByte = 0x00;
	double R, realber = 0.0;
	WJL_ERRRECOVERY_DECODER* decoder = NULL;
	WJL_ERRRECOVERY_DECODER** list = NULL;
	unsigned char ReceiveBuff[16] = { 0x8C, 0xB2, 0xD0, 0x99, 0x02, 0x1C, 0x48, 0x3C, 0x01, 0x59, 0xA3, 0xC9, 0xFE, 0xBC, 0x80, 0x29 };

	decoder = (WJL_ERRRECOVERY_DECODER*)malloc(sizeof(WJL_ERRRECOVERY_DECODER));
	list = (WJL_ERRRECOVERY_DECODER**)malloc(par->LIST_SIZE * sizeof(WJL_ERRRECOVERY_DECODER*));
	if (decoder == NULL || list == NULL) {
		goto Err;
	}
	// ֱ�ӿ���LIST_SIZE��list
	for (i = 0; i < par->LIST_SIZE; ++i) {
		list[i] = (WJL_ERRRECOVERY_DECODER*)malloc(sizeof(WJL_ERRRECOVERY_DECODER));
		if (list[i] == NULL) {
			goto Err;
		}
	}
	// ָ��ϵ��������������MAX_NUMBER_OF_0xFFϵ����������ͬ���ڱ���ʱ����MAX_NUMBER_OF_0xFF��������ϵ������Ч
	decoder->par = par;

	// ��ʼ������
	decoder->OutBytesLength = decoder->InBytesLength = par->CODE_LENGTH * 16;
	// ����Ļ���
	decoder->InBytesArray = (unsigned char*)malloc(decoder->InBytesLength);
	decoder->OutBytesArray = (unsigned char*)malloc(decoder->InBytesLength);
	decoder->BytesArray = (unsigned char*)malloc(decoder->InBytesLength);
	decoder->wfp = (WJL_FUNCTION_PARAMETERS*)malloc(sizeof(WJL_FUNCTION_PARAMETERS));
	if (decoder->InBytesArray == NULL || decoder->OutBytesArray == NULL || decoder->BytesArray == NULL || decoder->wfp == NULL) {
		goto Err;
	}
	/********************�������벿��**********************/
	decoder->InBytesLength = 16;
	// ����EncodedSize��ȡһ֡����
	printf("��Ҫ������ֽ����ͣ�\n");
	for (j = 0; j < 16; ++j) {
		decoder->InBytesArray[j] = ReceiveBuff[j];
		printf("%02X,", ReceiveBuff[j]);
	}
	printf("\n");
	// ���뵱ǰ��֡
	WJLErrRecoveryDecoder(decoder, list);

Err:
	// �ͷ���Դ
	if (decoder) {
		if (list) {
			for (i = 0; i < decoder->par->LIST_SIZE; ++i) if (list[i]) free(list[i]);
			free(list);
		}
		if (decoder->InBytesArray)free(decoder->InBytesArray);
		if (decoder->OutBytesArray)free(decoder->OutBytesArray);
		if (decoder->BytesArray)free(decoder->BytesArray);
		if (decoder->wfp)free(decoder->wfp);
		free(decoder);
	}
	return 0;
}