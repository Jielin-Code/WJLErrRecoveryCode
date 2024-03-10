#pragma once
/******************************************************************************************
基于杰林码纠错算法理论，并基于方法二的信源处理方法：
1、根据《杰林码原理及应用》的方案二：序列X中的符号0替换为101,且将符号1替换为01，从码率的角度上来讲方案一的码率才是最大的，但是纠错能力却是最弱的
2、采用位翻转的纠错方案，从效率上做了优化，效率提高相比之前版本提高了百万倍以上；
3、最大支持0dB的纠错，由于是CPU的编程，并未实现多线程的纠错，如果基于GPU或NPU效率能最大程度的提升；
4、输入字节越长则码率越高，理论编码码率为-1/log_2(1/3) = 0.630929；
5、可通过设置参数实现不同程度的纠错，START_LIMIT、END_LIMIT、COMPARE_LIMIT和ERRBITS_LIMIT；
6、DECODER_LIST_SIZE参数建议不要设置的太低，不得小于START_LIMIT + END_LIMIT + 1

理论：《杰林码原理及应用》
作者：王杰林
描述：基于加权概率模型的纠错算法，是全新的纠错算法
时间：20240310
版本：5.2.0
BUG修复说明：
1、修复了末尾字节纠错释放报错问题
2、采用参数对象替换掉了预处理指令，使得部分参数在纠错过程动态调整更方便，比如0dB-1dB采用一套参数，1.01dB到2.0dB又采用一套参数等等
3、修复了纠错失败释放资源报错问题
******************************************************************************************/
#ifndef _WJLERRRECOVERYCORE_H
#define _WJLERRRECOVERYCORE_H

// 参数结构体，这些值是影响纠错效果的核心参数，可以根据不同的信道设置不同的参数
typedef struct
{
	// WJL_ERRRECOVERY_DECODER对象列表大小，一般设置为24，需要注意DECODER_LIST_SIZE必须大于(START_LIMIT + END_LIMIT + FIRST_ERR_COMPARE_LIMIT + 1) * 2
	int DECODER_LIST_SIZE;
	// 杰林码纠错译码过程，会自动定位错误位置（即不满足“每个符号0被一个或两个1隔开”自动中止），定位错误位置后
	// 通过设置START_LIMIT和END_LIMIT来确定纠错字节的范围，越大纠错能力越强，同时需要运算的时间也越长
	// 根据杰林码的理论START_LIMIT取值范围为5到12，对应的纠错能力也不一样，最大值为12，根据理论得出最大前向纠错范围为12个字节
	// 一般设置为6，8，10或12
	int START_LIMIT;
	// 为保障纠错验证的正确概率，向后放宽一定的数量，当错误在END_LIMIT的基础上错误定位在COMPARE_LIMIT之后，则说明首个错误已经纠正
	// 可以根据实际信噪比和信道要求设置纠错范围
	// 一般设置为6,8,10
	int END_LIMIT;
	// 限制错误比特的个数，即在START_LIMIT到END_LIMIT个字节范围内错误比特的个数，暂时仅支持10（含）个比特以下的差错，越多需要遍历的可能性也越多
	// 在算力足够的情况下，完全可以采用并行验证的方案实现成倍的效率提升，比如GPU、NPU或其他硬件化，本库只考虑在CPU环境下的方案
	// 一般设置为3，4，5，6，7，8，默认设置为4或5
	int ERRBITS_LIMIT;
	// 根据理论，本算法存在“AB串”问题容易造成算法出现死循环，满足最大似然的MAXIMUM_TRAVERSAL_TIMES组可能性中挑选出最大可能的一组数据，也可以是一组数据
	int MAXIMUM_TRAVERSAL_TIMES;

}PARAMETERS;

// 根据杰林码纠错算法，译码判断条件“每个符号0被一个或两个符号1隔开”，不满足该条件的都是错误的
typedef enum
{
	KEEPBACK_NULL = 0,		    // 前面无符号
	KEEPBACK_ZERO,				// 前面有符号0
	KEEPBACK_ONE,				// 前面有符号1
	KEEPBACK_ONEZERO			// 前面有符号1和符号0
}KEEPBACK_SYMBOL;

// 杰林码纠错编码结构体
typedef struct
{
	// 输入
	PARAMETERS* par;               // 参数对象，不同的参数对应不同的译码能力
	unsigned char* InBytesArray;   // 输入字节缓存数组
	unsigned int InBytesLength;    // 输入字节的总长度
	unsigned int InBytesIndex;     // InBytesArray的下标

	// 输出
	unsigned char* OutBytesArray;  // 输出字节缓存数组
	unsigned int OutBytesLength;   // 输出字节的总长度
	unsigned int OutBytesIndex;    // OutBytesArray的下标

	// 运算变量，其中Li和Ri的运算参考《杰林码原理及应用》一书
	unsigned int Li;
	unsigned int Ri;
	unsigned int Delay;
	unsigned int DelayCount;
	unsigned char abandon;           // 用于标记哪些字节需要丢弃

}WJL_ERRRECOVERY_ENCODER;

// 纠错完毕后也需要解码器
typedef struct
{
	// 输入
	PARAMETERS* par;               // 参数对象，不同的参数对应不同的译码能力
	unsigned char* InBytesArray;   // 输入字节缓存数组
	unsigned int InBytesLength;    // 输入字节的总长度
	unsigned int InBytesIndex;     // InBytesArray的下标
	// 输出
	unsigned char* OutBytesArray;  // 输出字节缓存数组
	unsigned int OutBytesLength;   // 输出字节的总长度
	unsigned int OutBytesIndex;    // OutBytesArray的下标

	unsigned int Li;
	unsigned int Ri;
	unsigned int value;		        // 解码时用来保存输入编码（接收端为U）

	unsigned char mask;             // 字节轮值，8个比特组合成一个字节
	unsigned char outbyte;          // 输出的字节

	unsigned char full;             // 按照比特译码，输出是否满足一个字节
	unsigned char abandon;          // 用于标记哪些字节需要丢弃
	unsigned char status;           // 状态，0x01表示译码状态，0x00表示纠错状态，会影响到对象队列的缓存

	KEEPBACK_SYMBOL keepBackSymbol; // 用于信源处理方法判断的结构体

	unsigned char* BytesArray;          // 缓存尚未纠错的字节块，共DECODER_LIST_SIZE个字节

}WJL_ERRRECOVERY_DECODER;

#ifdef	__cplusplus
extern "C" {
#endif
	/******************************************************************************************
	纠错编码函数
	******************************************************************************************/
	int WJLErrRecoveryEncoder(WJL_ERRRECOVERY_ENCODER* coder);

	/******************************************************************************************
	纠错译码函数，也是核心的纠错译码函数
	WJL_ERRRECOVERY_DECODER** list 用来缓存DECODER_LIST_SIZE个coder
	int cumulativeZerosLimit 是通过连续译码0的个数来判定当前的错误是否完成纠错
	******************************************************************************************/
	int WJLErrRecoveryDecoder(WJL_ERRRECOVERY_DECODER* coder, WJL_ERRRECOVERY_DECODER** list);
#ifdef	__cplusplus
}
#endif
#endif
