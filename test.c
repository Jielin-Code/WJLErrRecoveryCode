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
// 以毫秒为单位的随机数，确保数据的误比特绝对的随机状况下
int randEx()
{
	LARGE_INTEGER seed;
	QueryPerformanceFrequency(&seed);
	QueryPerformanceCounter(&seed);
	srand(seed.QuadPart);
	return rand();
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
int QPSK_AWGN_ERRRECOVERY(unsigned int Length, double snr, int printIt)
{
	int i = 0, j = 0, Limit, RandDigit, count = 0, errbits = 0, tj = 1, ber_count = 0;
	unsigned char tmpByte = 0x00;
	double R, realber = 0.0;
	PARAMETERS* par = NULL;
	WJL_ERRRECOVERY_ENCODER* encoder = NULL;
	WJL_ERRRECOVERY_DECODER* decoder = NULL;
	WJL_ERRRECOVERY_ENCODER** list = NULL;
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
	// 指定系数，编码和译码的MAX_NUMBER_OF_0xFF系数必须是相同，在编码时除了MAX_NUMBER_OF_0xFF，其他的系数均无效
	encoder->par = par;
	decoder->par = par;
	// 默认会在当前数据编码前，先编码两个0x00，所以在实际应用中这里必须加上2
	encoder->InBytesLength = Length;
	decoder->OutBytesLength = Length;
	// 输出缓存尽量放的大一些，因为根据不同的SUBSECTION码率是不一样的，为了支持0.25的码率，需要设置四倍长度
	encoder->OutBytesLength = (unsigned int)(encoder->InBytesLength * 4);
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

	if (Length <= 1024) {
		if (printIt) printf("随机生成的原始数据(The randomly generated raw data)：%d\n", encoder->InBytesLength);
		// 产生随机数据
		for (i = 0; i < Length; ++i) {
			encoder->InBytesArray[i] = randEx() % 256; //rand() % 256; // Set_In_BUFF[i];
			if (printIt) printf("%03d->%02X,", i, encoder->InBytesArray[i]);
		}
		if (printIt)printf("\n");
	}
	else {
		// 产生随机数据
		for (i = 0; i < Length; ++i) {
			encoder->InBytesArray[i] = randEx() % 256; //rand() % 256; // Set_In_BUFF[i];
		}
	}

	/********************编码译码部分**********************/
	WJLErrRecoveryEncoder(encoder);

	// 实际编码码率
	R = (double)encoder->InBytesLength / (double)encoder->OutBytesIndex;
	if (printIt) {
		if (Length <= 1024) {
			printf("编码后的字节(Encoded bytes)：%d\n", encoder->OutBytesIndex);
			for (i = 0; i < encoder->OutBytesIndex; ++i) {
				printf("%03d->%02X,", i, encoder->OutBytesArray[i]);
			}
			printf("\n");
			printf("编码前：%d, 编码后：%d，实际码率R = %1.6f, 理论编码码率R=-1/log2(1/3)=1/1.5849625=0.63092975\n", encoder->InBytesLength, encoder->OutBytesIndex, R);
			printf("Before coding:%d, After coding:%d，Actual code rate:R = %1.6f, Theoretical coding code rate:R=-1/log2(1/3)=1/1.5849625=0.63092975\n", encoder->InBytesLength, encoder->OutBytesIndex, R);
			printf("\n");
		}
		else {
			printf("编码前：%d, 编码后：%d，实际码率R = %1.6f, 理论编码码率R=-1/log2(1/3)=1/1.5849625=0.63092975\n", encoder->InBytesLength, encoder->OutBytesIndex, R);
			printf("Before coding:%d, After coding:%d，Actual code rate:R = %1.6f, Theoretical coding code rate:R=-1/log2(1/3)=1/1.5849625=0.63092975\n", encoder->InBytesLength, encoder->OutBytesIndex, R);
			printf("\n");
		}
	}
	// 编码输出的结果通过Qpsk_awgn仿真后给到译码的输入端
	Qpsk_awgn(encoder->OutBytesArray, decoder->InBytesArray, encoder->OutBytesIndex, snr, &ber_count);
	if (printIt) {
		printf("实际误比特率为(The actual bit error rate is)：ber = %1.6f\n", (double)ber_count / ((double)encoder->OutBytesIndex * 8));
	}
	// 设定译码输入的长度
	decoder->InBytesLength = encoder->OutBytesIndex;

	if (printIt) {
		if (Length <= 1024) {
			printf("信道仿真传输得到的字节(Bytes obtained from the channel simulation transmission)：%d\n", encoder->OutBytesIndex);
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

	/********************纠错译码部分**********************/
	WJLErrRecoveryDecoder(decoder, list);

	if (printIt) {
		if (Length <= 1024) {
			printf("译码后的字节(Decoded byte)：%d\n", decoder->OutBytesIndex - 1);
			for (i = 0; i < Length; ++i) {
				printf("%03d->%02X,", i, decoder->OutBytesArray[i]);
			}
			printf("\n");
		}
	}
	// 检查是否有错误
	for (i = 0; i < Length; ++i) {
		if (encoder->InBytesArray[i] != decoder->OutBytesArray[i]) {
			printf("*******************错误位置(Error location)：%d\n", i);
			goto Err;
		}
	}
	// 释放资源
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
	printf("所有错误已经纠正，并且译码正确！\nAll errors have been corrected and are decoded correctly!\n");
	return 1;
Err:
	// 释放资源
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
	printf("\n译码错误！\nDecoding error!\n");
	return 0;
}
// 随机生成一个0-255的值A，并且判断其逆矩阵的值B与A的比特差异应该达到4比特以上
void RandombyQualification()
{
	int i = 0, j = 0, k = 0, l = 0, sign = 1;
	int _ucBytes[256];
	int _iucBytes[256];
	int tmp;
	int err, uperr = 0;
	// 初始化_ucBytes和_iucBytes
	for (i = 0; i < 256; ++i) {
		_ucBytes[i] = -1;
		_iucBytes[i] = -1;
	}
	// 产生随机数据
	while (j < 256) {
		tmp = randEx() % 256;
		sign = 1;
		for (i = 0; i < 256; ++i) {
			// 找到了相同的值
			if (_ucBytes[i] == tmp) {
				sign = 0;
				break;
			}
		}
		//判断是否满足条件，即tmp与相邻的_ucBytes[i]差异
		for (i = 0; i < 256; ++i) {
			// 找到了相同的值
			if (_ucBytes[i] == -1) {
				break;
			}
		}
		if (i != 0 && abs((int)_ucBytes[i - 1] - tmp) < 32) {
			sign = 0;
		}
		// 没有找到相同的值
		if (sign) {
			// 然后判断其逆矩阵中的值是否满足比特差异要求
			for (k = 0; k < 256; ++k) {
				if ((unsigned char)k == tmp) {
					err = ErrBits((unsigned char)j, tmp);
					// 再检验k和tmp是否满足比特差异要求abs(j - tmp) >= 8
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
	printf("\n矩阵：\n");
	// 先打印_ucBytes
	for (i = 0; i < 256; ++i) {
		if (i % 16 == 0) {
			printf("\n");
		}
		printf("0x%02X,", _ucBytes[i]);
	}
	printf("\n");
	printf("\n");
	printf("逆矩阵：\n");
	// 然后生成逆矩阵并打印
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