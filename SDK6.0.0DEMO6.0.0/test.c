#include "test.h"
#include "BPSK_AWGN.h"
#include "QPSK_AWGN.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// 是否限制误比特差异在上下1.0dB范围内，0不限制，1限制
#define LIMIT_MODEL 0

void SynchronizationBytes(WJL_ALGORITHM_PARAMETERS* par)
{
	int i, j;
	WJL_ERRRECOVERY_ENCODER* encoder1 = NULL;
	WJL_ERRRECOVERY_ENCODER* encoder2 = NULL;
	// 防止输入的参数有错误
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
	// 用encoder1编码par->CODE_LENGTH个0x00，全0x00和全0xFF是两个极端的边缘序列
	for (i = 0; i < par->CODE_LENGTH; ++i) encoder1->InBytesArray[i] = 0x00;
	WJLErrRecoveryEncoder(encoder1);
	// 用encoder2编码par->CODE_LENGTH个0xFF
	for (i = 0; i < par->CODE_LENGTH; ++i) encoder2->InBytesArray[i] = 0xFF;
	WJLErrRecoveryEncoder(encoder2);
	// 根据静态码率
	if (encoder1->OutBytesIndex != encoder2->OutBytesIndex) {
		printf("静态码率理论出错误！\n");
		goto Err;
	}
	// 比较encoder1->OutBytesArray和encoder2->OutBytesArray末尾字节
	j = 0;
	for (i = encoder1->OutBytesIndex - 1; i >= 0; --i) {
		if (encoder1->OutBytesArray[i] != encoder2->OutBytesArray[i]) {
			break;
		}
		j++;
	}
	// 然后将末尾j个字节存储到par->SynchronizationBytes中
	par->SynchronizationLength = 0;
	for (i = encoder1->OutBytesIndex - j; i < encoder1->OutBytesIndex; ++i) {
		par->SynchronizationBytes[par->SynchronizationLength] = encoder1->OutBytesArray[i];
		par->SynchronizationLength++;
	}
Err:
	// 释放资源
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


// 无错情况下的编译码检验
int NoErrByBytesArrayLength(WJL_ALGORITHM_PARAMETERS* par)
{
	int i = 0;
	WJL_ERRRECOVERY_ENCODER* encoder = NULL;
	WJL_ERRRECOVERY_DECODER* decoder = NULL;
	WJL_ERRRECOVERY_DECODER** list = NULL;
	unsigned int OutBUFF1_Length = 0, OutBUFF2_Length = 0;
	// 防止输入的参数有错误
	if (par->CODE_LENGTH <= 0 || par->LIST_SIZE <= 0) return 0;
	// 开辟缓存
	encoder = (WJL_ERRRECOVERY_ENCODER*)malloc(sizeof(WJL_ERRRECOVERY_ENCODER));
	decoder = (WJL_ERRRECOVERY_DECODER*)malloc(sizeof(WJL_ERRRECOVERY_DECODER));
	list = (WJL_ERRRECOVERY_DECODER**)malloc(par->LIST_SIZE * sizeof(WJL_ERRRECOVERY_DECODER*));
	if (encoder == NULL || decoder == NULL || list == NULL) {
		goto Err;
	}

	// 直接开辟LIST_SIZE个list
	for (i = 0; i < par->LIST_SIZE; ++i) {
		list[i] = (WJL_ERRRECOVERY_DECODER*)malloc(sizeof(WJL_ERRRECOVERY_DECODER));
		if (list[i] == NULL) {
			goto Err;
		}
	}
	// 指定系数，编码和译码的MAX_NUMBER_OF_0xFF系数必须是相同，在编码时除了MAX_NUMBER_OF_0xFF，其他的系数均无效
	encoder->par = par;
	decoder->par = par;

	decoder->OutBytesLength = encoder->InBytesLength = par->CODE_LENGTH * 16;
	// 这个是理论值，为了支持1个字节的编译码，这个最好设置encoder->InBytesLength的4倍
	encoder->OutBytesLength = encoder->InBytesLength;

	// 编码的缓存
	encoder->InBytesArray = (unsigned char*)malloc(encoder->InBytesLength);
	encoder->OutBytesArray = (unsigned char*)malloc(encoder->OutBytesLength);
	// 译码的缓存
	decoder->InBytesArray = (unsigned char*)malloc(encoder->InBytesLength);
	decoder->OutBytesArray = (unsigned char*)malloc(encoder->InBytesLength);
	decoder->BytesArray = (unsigned char*)malloc(encoder->InBytesLength);
	decoder->wfp = (WJL_FUNCTION_PARAMETERS*)malloc(sizeof(WJL_FUNCTION_PARAMETERS));
	if (encoder->InBytesArray == NULL || encoder->OutBytesArray == NULL || decoder->InBytesArray == NULL || decoder->OutBytesArray == NULL || decoder->BytesArray == NULL || decoder->wfp == NULL) {
		goto Err;
	}

	// 产生随机数据
	for (i = 0; i < par->CODE_LENGTH; ++i) {
		encoder->InBytesArray[i] = rand() % 256;
		printf("%02X,", encoder->InBytesArray[i]);
	}
	printf("\n");


	/********************编码译码部分**********************/
	WJLErrRecoveryEncoder(encoder);


	// encoder->OutBytesIndex为实际输出的字节长度，把encoder->OutBytesArray中的字节复制给decoder->InBytesArray
	memcpy(decoder->InBytesArray, encoder->OutBytesArray, encoder->OutBytesIndex);
	decoder->InBytesLength = encoder->OutBytesIndex;

	printf("编码后的字节：%d\n", encoder->OutBytesIndex);
	for (i = 0; i < encoder->OutBytesIndex; ++i) {
		printf("%02X,", decoder->InBytesArray[i]);
	}
	printf("\n");
	printf("编码前：%d, 编码后：%d，实际码率R = %1.6f, 理论编码码率R=-1/log_2(1/3)=1/1.5849625=0.63092975\n", par->CODE_LENGTH, encoder->OutBytesIndex, (double)par->CODE_LENGTH / (double)encoder->OutBytesIndex);
	printf("\n");

	/********************纠错译码部分**********************/
	WJLErrRecoveryDecoder(decoder, list);

	// 检查是否有错误
	for (i = 0; i < par->CODE_LENGTH; ++i) {
		if (encoder->InBytesArray[i] != decoder->OutBytesArray[i]) {
			goto Err;
		}
	}

	printf("译码后的字节：%d\n", decoder->OutBytesIndex - 1);
	for (i = 0; i < par->CODE_LENGTH; ++i) {
		printf("%02X,", decoder->OutBytesArray[i]);
	}
	printf("\n");

	// 释放资源
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
	// 释放资源
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
// Q函数
double Q(double EbN0)
{
	return 0.5 * erfc(sqrt(2 * EbN0) / sqrt(2.0));
}
//------------------------------------------------------------------------------------------------------
// 下面的几个函数主要是针对不同的系统进行仿真，主要是基于不同的信道类型，输入不同的信噪比计算出对应的BER
// BPSK是一个公式
double AWGN_BPSK_BER(double EbN0_dB)
{
	// 参考文献：丁凯. AWGN信道中BPSK误码率仿真分析[J]. 微处理机,2021,42(3):23-26. DOI:10.3969/j.issn.1002-2279.2021.03.006.
	double EbN0 = pow(10, EbN0_dB / 10);
	// 计算误比特率
	return 0.5 * erfc(sqrt(EbN0));
	//return Q(EbN0);
}
// 判断两个字节中差异比特个数
int ErrBits(unsigned char byte1, unsigned char byte2)
{
	int i, j = 0;
	unsigned char tmpbyte = byte1 ^ byte2;
	// 统计差异个数
	for (i = 7; i >= 0; --i) {
		if ((tmpbyte >> i) & 0x01) {
			j++;
		}
	}
	return j;
}
// 存在错误的数据编译码
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

	// 开辟缓存
	encoder = (WJL_ERRRECOVERY_ENCODER*)malloc(sizeof(WJL_ERRRECOVERY_ENCODER));
	decoder = (WJL_ERRRECOVERY_DECODER*)malloc(sizeof(WJL_ERRRECOVERY_DECODER));
	list = (WJL_ERRRECOVERY_DECODER**)malloc(par->LIST_SIZE * sizeof(WJL_ERRRECOVERY_DECODER*));
	if (encoder == NULL || decoder == NULL || list == NULL) {
		goto Err;
	}
	// 直接开辟LIST_SIZE个list
	for (i = 0; i < par->LIST_SIZE; ++i) {
		list[i] = (WJL_ERRRECOVERY_DECODER*)malloc(sizeof(WJL_ERRRECOVERY_DECODER));
		if (list[i] == NULL) {
			goto Err;
		}
	}
	// 随机生成数据的缓存
	BytesForEncode = (unsigned char*)malloc(par->CODE_LENGTH * BlockSize);
	BytesForDecode = (unsigned char*)malloc(par->CODE_LENGTH * BlockSize);
	// 开辟足够大的发送缓存，这个是每帧编码后存储的位置，一般开辟足够大的空间
	SendBuff = (unsigned char*)malloc(par->CODE_LENGTH * BlockSize * 16);
	// 开辟SendBuff相同大小的缓存，通过调制-AWGN传输-解调后的缓存
	ReceiveBuff = (unsigned char*)malloc(par->CODE_LENGTH * BlockSize * 16);
	// 排除异常
	if (BytesForEncode == NULL || BytesForDecode == NULL || SendBuff == NULL || ReceiveBuff == NULL) goto Err;

	// 指定系数，编码和译码的MAX_NUMBER_OF_0xFF系数必须是相同，在编码时除了MAX_NUMBER_OF_0xFF，其他的系数均无效
	encoder->par = par;
	decoder->par = par;

	// 初始化长度
	decoder->OutBytesLength = decoder->InBytesLength = encoder->OutBytesLength = encoder->InBytesLength = par->CODE_LENGTH * 16;

	// 编码的缓存
	encoder->InBytesArray = (unsigned char*)malloc(encoder->InBytesLength);
	encoder->OutBytesArray = (unsigned char*)malloc(encoder->OutBytesLength);
	// 译码的缓存
	decoder->InBytesArray = (unsigned char*)malloc(decoder->InBytesLength);
	decoder->OutBytesArray = (unsigned char*)malloc(decoder->InBytesLength);
	decoder->BytesArray = (unsigned char*)malloc(decoder->InBytesLength); 
	decoder->wfp = (WJL_FUNCTION_PARAMETERS*)malloc(sizeof(WJL_FUNCTION_PARAMETERS));
	if (encoder->InBytesArray == NULL || encoder->OutBytesArray == NULL || decoder->InBytesArray == NULL || decoder->OutBytesArray == NULL || decoder->BytesArray == NULL || decoder->wfp == NULL) {
		goto Err;
	}

	if (printIt) printf("随机生成待编码字节(Random bytes for waiting encoding)：%d\n", par->CODE_LENGTH * BlockSize);

	// 随机生成par->CODE_LENGTH * BlockSize个字节
	for (i = 0; i < par->CODE_LENGTH * BlockSize; ++i) {
		BytesForEncode[i] = rand() % 256;
		if (printIt) printf("%02X,", BytesForEncode[i]);
	}
	if (printIt)printf("\n");

	/********************编码译码部分**********************/
	BytesCount = 0;
	for (i = 0; i < BlockSize; ++i) {
		// 首先是把每帧的数据存储到encoder->InBytesArray
		for (j = 0; j < par->CODE_LENGTH; ++j) {
			encoder->InBytesArray[j] = BytesForEncode[i * par->CODE_LENGTH + j];
		}
		// 编码当前的帧
		WJLErrRecoveryEncoder(encoder);
		// 累加编码后的字节数
		BytesCount += encoder->OutBytesIndex;
		// 保存每帧编码的结果，静态码率编码长度是一致的，可以返回EncodedSize
		if (EncodedSize == 0) EncodedSize = encoder->OutBytesIndex;
		// 测试一下静态码率是不是编码长度一致
		else if (EncodedSize != encoder->OutBytesIndex) {
			printf("随机数据每帧编码长度不一致！解码时会出现差错！终止编码！\nRandom data has inconsistent encoding length per frame! Decoding occurs with an error! End-coding!\n");
			goto Err;
		}
		// 把每帧的编码结果encoder->OutBytesArray存储到SendBuff中
		for (j = 0; j < (int)encoder->OutBytesIndex; ++j) {
			SendBuff[k] = encoder->OutBytesArray[j];
			k++;
		}
	}

	// 实际编码码率
	R = ((double)par->CODE_LENGTH * (double)BlockSize) / (double)k;
	if (printIt) {
		printf("编码后的字节(Encoded bytes)：%d\n", k);
		for (i = 0; i < k; ++i) {
			printf("0x%02X,", SendBuff[i]);
		}
		printf("\n");
	}
	printf("编码前：%d, 编码后：%d，实际编码码率R = %1.6f, 理论编码码率R = -1/log2(1/3) = 1/1.5849625 = 0.63092975\n", par->CODE_LENGTH * BlockSize, k, R);
	printf("Before coding:%d, After coding:%d, Actual code rate:R = %1.6f, Theoretical code rate:R = -1/log2(1/3) = 1/1.5849625 = 0.63092975\n", par->CODE_LENGTH * BlockSize, k, R);
	printf("\n");

	// 编码结果通过调制-awgn发收-解调仿真，数据足够大是误比特率才能有保障
	if (model == BPSK) bpsk_awgn(SendBuff, ReceiveBuff, k, snr, &ber_count);
	else if (model == QPSK) Qpsk_awgn(SendBuff, ReceiveBuff, k, snr, &ber_count);

	// 以0.5dB步进
	if (LIMIT_MODEL) {
		// 大于AWGN_BPSK_BER(snr - 0.5)认为不符合信噪比snr的仿真
		if ((double)ber_count / ((double)k * 8) >= AWGN_BPSK_BER(snr - 0.5) || (double)ber_count / ((double)k * 8) <= AWGN_BPSK_BER(snr + 0.5)) {
			bler_count = -1;
			goto Invalid;
		}
		else {
			*EncodedBytesCount = *EncodedBytesCount + BytesCount;
			// 累计实际上的误比特数，方便得出平均环境下awgn对应的误比特率
			*BERSum = *BERSum + ber_count;
		}
	}
	else {
		*EncodedBytesCount = *EncodedBytesCount + BytesCount;
		// 累计实际上的误比特数，方便得出平均环境下awgn对应的误比特率
		*BERSum = *BERSum + ber_count;
	}
	printf("当前帧误比特率为(The current frame bit error rate is)：ber = %1.8f\n理论误比特率(Theoretical bit error rate)：ber = %1.8f\n\n", (double)ber_count / ((double)k * 8), AWGN_BPSK_BER(snr));
	if (printIt) {
		printf("调制-AWGN传输-解调后错误的字节(Error bytes after Modulation-AWGN transmission-demodulation)：\n");
		for (i = 0; i < k; ++i) {
			// 打印出错误的字节，方便检验纠错算法是否完成了纠错
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
	/********************纠错译码部分**********************/
	k = 0;
	for (i = 0; i < BlockSize; ++i) {
		// 按照EncodedSize获取一帧数据
		for (j = 0; j < EncodedSize; ++j) {
			decoder->InBytesArray[j] = ReceiveBuff[k];
			k++;
		}
		// 一定要初始化编码器输入的长度
		decoder->InBytesLength = EncodedSize;
		// 解码当前的帧
		WJLErrRecoveryDecoder(decoder, list);
		// 把解码后的结果decoder->OutBytesArray存储到BytesForDecode中
		for (j = 0; j < par->CODE_LENGTH; ++j) {
			BytesForDecode[i * par->CODE_LENGTH + j] = decoder->OutBytesArray[j];
		}
		// 判断当前的块是否存在错误
		for (j = 0; j < par->CODE_LENGTH; ++j) {
			if (BytesForDecode[i * par->CODE_LENGTH + j] != BytesForEncode[i * par->CODE_LENGTH + j]) {
				if (printIt) printf("%d->%02X %02X\n", i * par->CODE_LENGTH + j, BytesForEncode[i * par->CODE_LENGTH + j], BytesForDecode[i * par->CODE_LENGTH + j]);
				// 累计错误的块个数，方便在外面计算
				bler_count++;
				if (printIt) printf("第%d块纠错失败，译码错误！\nThe %d block error correction failure, decoding error!\n", i, i);
				// 这个地方必须直接跳出循环，否则bler_count会被多次计算
				break;
			}
		}
	}
	// 累加上当前的误块率
	*BLERSum = *BLERSum + bler_count;
	if (printIt) {
		printf("当前帧共有%d块随机数，纠错译码后错误块数：%d\n", BlockSize, bler_count);
		printf("The current frame has a %d block random data, Number of error blocks after error correction and decoding:%d\n", BlockSize, bler_count);
	}

	if (printIt) {
		printf("译码后的字节(Decoded bytes)：\n");
		for (i = 0; i < par->CODE_LENGTH * BlockSize; ++i) {
			printf("%02X,", BytesForDecode[i]);
		}
		printf("\n");
	}
Invalid:
	// 释放资源
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
	// 根据bler_count可以判断误帧率，因为每帧可能包括很多块
	if (bler_count > 0) {
		printf("译码错误！Decoding error!\n");
		return 0;
	}
	else if(bler_count == 0){
		printf("所有错误已经纠正，并且译码正确！All errors have been corrected and are decoded correctly!\n");
		return 1;
	}
	else {
		// bler_count < 0 的情况
		return 2;
	}
Err:
	// 释放资源
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
	printf("译码错误！Decoding error!\n");
	return 0;
}

// 存在错误的数据编译码
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
	// 直接开辟LIST_SIZE个list
	for (i = 0; i < par->LIST_SIZE; ++i) {
		list[i] = (WJL_ERRRECOVERY_DECODER*)malloc(sizeof(WJL_ERRRECOVERY_DECODER));
		if (list[i] == NULL) {
			goto Err;
		}
	}
	// 指定系数，编码和译码的MAX_NUMBER_OF_0xFF系数必须是相同，在编码时除了MAX_NUMBER_OF_0xFF，其他的系数均无效
	decoder->par = par;

	// 初始化长度
	decoder->OutBytesLength = decoder->InBytesLength = par->CODE_LENGTH * 16;
	// 译码的缓存
	decoder->InBytesArray = (unsigned char*)malloc(decoder->InBytesLength);
	decoder->OutBytesArray = (unsigned char*)malloc(decoder->InBytesLength);
	decoder->BytesArray = (unsigned char*)malloc(decoder->InBytesLength);
	decoder->wfp = (WJL_FUNCTION_PARAMETERS*)malloc(sizeof(WJL_FUNCTION_PARAMETERS));
	if (decoder->InBytesArray == NULL || decoder->OutBytesArray == NULL || decoder->BytesArray == NULL || decoder->wfp == NULL) {
		goto Err;
	}
	/********************纠错译码部分**********************/
	decoder->InBytesLength = 16;
	// 按照EncodedSize获取一帧数据
	printf("需要检验的字节类型：\n");
	for (j = 0; j < 16; ++j) {
		decoder->InBytesArray[j] = ReceiveBuff[j];
		printf("%02X,", ReceiveBuff[j]);
	}
	printf("\n");
	// 解码当前的帧
	WJLErrRecoveryDecoder(decoder, list);

Err:
	// 释放资源
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