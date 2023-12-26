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
// 存在错误的数据编译码
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
	// 参数开辟是整个算法的核心
	par = (PARAMETERS*)malloc(sizeof(PARAMETERS));
	if (par == NULL) {
		goto Err;
	}
	// 然后是设置参数
	par->DECODER_LIST_SIZE = 64;
	par->MAX_NUMBER_OF_0xFF = 3;
	par->START_LIMIT = 10;
	par->END_LIMIT = 8;
	par->FIRST_ERR_COMPARE_LIMIT = 3;
	par->BLOCK_ERR_COMPARE_LIMIT = 4;
	par->ERRBITS_LIMIT = 5;
	par->MAXIMUM_TRAVERSAL_TIMES = 8;

	// 开辟缓存
	encoder = (WJL_ERRRECOVERY_ENCODER*)malloc(sizeof(WJL_ERRRECOVERY_ENCODER));
	decoder = (WJL_ERRRECOVERY_DECODER*)malloc(sizeof(WJL_ERRRECOVERY_DECODER));
	list = (WJL_ERRRECOVERY_DECODER**)malloc(par->DECODER_LIST_SIZE * sizeof(WJL_ERRRECOVERY_DECODER*));
	if (encoder == NULL || decoder == NULL || list == NULL) {
		goto Err;
	}
	// 直接开辟DECODER_LIST_SIZE个list
	for (i = 0; i < par->DECODER_LIST_SIZE; ++i) {
		list[i] = (WJL_ERRRECOVERY_DECODER*)malloc(sizeof(WJL_ERRRECOVERY_DECODER));
		if (list[i] == NULL) {
			goto Err;
		}
	}
	// 随机生成数据的缓存
	BytesForEncode = (unsigned char*)malloc(BytesForEachBlock * BlockSize);
	BytesForDecode = (unsigned char*)malloc(BytesForEachBlock * BlockSize);
	// 开辟足够大的发送缓存，这个是每帧编码后存储的位置，一般开辟足够大的空间
	SendBuff = (unsigned char*)malloc(BytesForEachBlock * BlockSize * 16);
	// 开辟SendBuff相同大小的缓存，通过调制-AWGN传输-解调后的缓存
	ReceiveBuff = (unsigned char*)malloc(BytesForEachBlock * BlockSize * 16);
	// 排除异常
	if (BytesForEncode == NULL || BytesForDecode == NULL || SendBuff == NULL || ReceiveBuff == NULL) goto Err;

	// 指定系数，编码和译码的MAX_NUMBER_OF_0xFF系数必须是相同，在编码时除了MAX_NUMBER_OF_0xFF，其他的系数均无效
	encoder->par = par;
	decoder->par = par;

	// 默认会在当前数据编码前，先编码两个0x00，所以在实际应用中这里必须加上2
	encoder->InBytesLength = BytesForEachBlock;
	decoder->OutBytesLength = BytesForEachBlock;
	// 输出缓存尽量放的大一些，因为根据不同的SUBSECTION码率是不一样的，为了支持0.25的码率，需要设置四倍长度
	encoder->OutBytesLength = (unsigned int)(encoder->InBytesLength * 16);
	// 编码的缓存
	encoder->InBytesArray = (unsigned char*)malloc(encoder->InBytesLength);
	encoder->OutBytesArray = (unsigned char*)malloc(encoder->OutBytesLength);
	// 译码的缓存
	decoder->InBytesArray = (unsigned char*)malloc(encoder->OutBytesLength);
	decoder->OutBytesArray = (unsigned char*)malloc(encoder->InBytesLength);
	decoder->BytesArray = (unsigned char*)malloc(decoder->par->DECODER_LIST_SIZE);
	decoder->InBytesArraySection = (unsigned char*)malloc(decoder->par->DECODER_LIST_SIZE);
	if (encoder->InBytesArray == NULL || encoder->OutBytesArray == NULL || decoder->InBytesArray == NULL || decoder->OutBytesArray == NULL || decoder->BytesArray == NULL || decoder->InBytesArraySection == NULL) {
		goto Err;
	}

	srand((unsigned)time(NULL));
	if (printIt) printf("随机生成待编码数据(Random bytes for waiting encoding)：%d\n", BytesForEachBlock * BlockSize);
	// 随机生成BytesForEachBlock * BlockSize个字节
	for (i = 0; i < BytesForEachBlock * BlockSize; ++i) {
		BytesForEncode[i] = rand() % 256;
		if (printIt) printf("%02X,", BytesForEncode[i]);
	}
	if (printIt)printf("\n");

	/********************编码译码部分**********************/
	for (i = 0; i < BlockSize; ++i) {
		// 首先是把每帧的数据存储到encoder->InBytesArray
		for (j = 0; j < BytesForEachBlock; ++j) {
			encoder->InBytesArray[j] = BytesForEncode[i * BytesForEachBlock + j];
		}
		// 编码当前的帧
		WJLErrRecoveryEncoder(encoder);
		// 保存每帧编码的结果，编码长度是一致的，可以返回EncodedSize
		if (EncodedSize == 0) EncodedSize = encoder->OutBytesIndex;
		// 为了防止可能出现不同长度的编码结果，这里做一个判断
		else if (EncodedSize != encoder->OutBytesIndex) {
			printf("随机数据每帧编码长度不一致！解码时会出现差错！终止编码！\nRandom data has inconsistent encoding length per frame! Decoding occurs with an error! End-coding!\n");
			goto Err;
		}
		// 把每帧的编码结果encoder->OutBytesArray存储到SendBuff中
		for (j = 0; j < encoder->OutBytesIndex; ++j) {
			SendBuff[k] = encoder->OutBytesArray[j];
			k++;
		}
	}

	// 实际编码码率
	R = ((double)BytesForEachBlock * (double)BlockSize) / (double)k;
	if (printIt) {
		printf("编码后的字节(Encoded bytes)：%d\n", k);
		for (i = 0; i < k; ++i) {
			printf("%02X,", SendBuff[i]);
		}
		printf("\n");
	}
	printf("编码前：%d, 编码后：%d，实际编码码率R = %1.6f\n理论编码码率R = -1/log2(1/3) = 1/1.5849625 = 0.63092975\n", BytesForEachBlock * BlockSize, k, R);
	printf("Before coding:%d  After coding:%d Actual code rate:R = %1.6f\nTheoretical code rate:R = -1/log2(1/3) = 1/1.5849625 = 0.63092975\n", BytesForEachBlock * BlockSize, k, R);
	printf("\n");

	// 编码结果通过调制-awgn发收-解调仿真，数据足够大是误比特率才能有保障
	if (type == QPSK) Qpsk_awgn(SendBuff, ReceiveBuff, k, snr, &ber_count);
	if (type == BPSK) Bpsk_awgn(SendBuff, ReceiveBuff, k, snr, &ber_count);
	// 累计实际上的误比特数，方便得出平均环境下awgn对应的误比特率
	*BERSum = *BERSum + ((double)ber_count / ((double)k * 8.0));
	printf("实际误比特率为(The actual bit error rate is)：ber = %1.6f，理论误比特率(Theoretical bit error rate)：ber = %1.6f\n\n", (double)ber_count / ((double)k * 8), AWGN_BPSK_BER(snr));
	if (printIt) {
		printf("调制-AWGN传输-解调后错误的字节(Error bytes after Modulation-AWGN transmission-demodulation)：\n");
		for (i = 0; i < k; ++i) {
			if (ReceiveBuff[i] != SendBuff[i]) {
				printf("%d:(S)%02X->(R)%02X,", i, SendBuff[i], ReceiveBuff[i]);
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
		for (j = 0; j < BytesForEachBlock; ++j) {
			BytesForDecode[i * BytesForEachBlock + j] = decoder->OutBytesArray[j];
		}
		// 判断当前的块是否存在错误
		for (j = 0; j < BytesForEachBlock; ++j) {
			if (BytesForDecode[i * BytesForEachBlock + j] != BytesForEncode[i * BytesForEachBlock + j]) {
				// 累计错误的块个数，方便在外面计算
				bler_count++;
				if (printIt) printf("第%d块纠错失败，译码错误！\nThe %d block error correction failure, decoding error!\n", i, i);
				// 不再存储当前的块
				break;
			}
		}
	}
	// 累加上当前的误块率
	*BLERSum = *BLERSum + (double)bler_count / (double)BlockSize;
	if (printIt) {
		printf("当前共测试了%d块随机数，纠错译码后错误块数：%d, 误块率为：BLER = %1.6f\n", BlockSize, bler_count, (double)bler_count / (double)BlockSize);
		printf("A total of %d block random numbers is currently tested, Number of error blocks after error correction and decoding:%d, BLER = %1.6f\n", BlockSize, bler_count, (double)bler_count / (double)BlockSize);
	}

	if (printIt) {
		printf("译码后的字节(Decoded bytes)：\n");
		for (i = 0; i < BytesForEachBlock * BlockSize; ++i) {
			printf("%02X,", BytesForDecode[i]);
		}
		printf("\n");
	}

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
	// 根据bler_count可以判断误帧率
	if (bler_count > 0) {
		printf("译码错误！Decoding error!\n");
		return 0;
	}
	else {
		printf("所有错误已经纠正，并且译码正确！All errors have been corrected and are decoded correctly!\n");
		return 1;
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
	printf("译码错误！Decoding error!\n");
	return 0;
}