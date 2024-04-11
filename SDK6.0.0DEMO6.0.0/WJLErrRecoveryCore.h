#pragma once
/******************************************************************************************
基于杰林码纠错算法理论，并基于方法二的信源处理方法：
1、根据《杰林码原理及应用》的方案二：序列X中的符号0替换为101,且将符号1替换为01，从码率的角度上来讲方案一的码率才是最大的，但是纠错能力却是最弱的
2、采用位翻转的纠错方案，从效率上做了优化，效率提高相比之前版本提高了百万倍以上；
3、最大支持0dB的纠错，由于是CPU的编程，并未实现多线程的纠错，如果基于GPU或NPU效率能最大程度的提升；
4、输入字节越长则码率越高，理论编码码率为-1/log_2(1/3) = 0.630929；
5、可通过设置参数实现不同程度的纠错，START_LIMIT、END_LIMIT、COMPARE_LIMIT和ERRBITS_LIMIT；
6、LIST_SIZE参数建议不要设置的太低，不得小于START_LIMIT + END_LIMIT + 1

理论：《杰林码原理及应用》
作者：王杰林
描述：基于加权概率模型的纠错算法，是全新的纠错算法
时间：20240411
版本：6.0.0
BUG修复说明：
1、完全按照论文编写的测试程序
2、纠错效率提升
******************************************************************************************/
#ifndef _WJLERRRECOVERYCORE_H
#define _WJLERRRECOVERYCORE_H

// 参数结构体，这些值是影响纠错效果的核心参数，可以根据不同的信道设置不同的参数
typedef struct
{
	// WJL_ERRRECOVERY_DECODER对象列表大小，一般设置为24，36，如果listIndex >= coder->par->LIST_SIZE说明LIST_SIZE设置的太小了
	int LIST_SIZE;
	// 码长，单位为字节，编译码长度
	int CODE_LENGTH;
	// 末尾追加的二进制序列Q的字节长度，对应论文中的κ
	int Q_LENGTH;
	// 纠错区间的下限极值，对应论文中的α
	int START_LIMIT;
	// 纠错区间的上限极值，对应论文中的β
	int END_LIMIT;
	// 纠错区间内最大翻转比特个数，对应论文中的τ_max
	int ERRBITS_LIMIT;
	// 遍历类型，0表示从1比特错误遍历到ERRBITS_LIMIT，1表示从ERRBITS_LIMIT遍历到1
	int ERGODIC_MODEL;
	// CODE_LENGTH和Q_LENGTH确定时，编码后末尾有几个字节是完全相同的，对于译码端来讲这些字节完全可以在译码时加上，无需传输从而提高传输速率
	unsigned char* SynchronizationBytes;
	int SynchronizationLength;

}WJL_ALGORITHM_PARAMETERS;

// 杰林码纠错编码结构体
typedef struct
{
	// 输入
	WJL_ALGORITHM_PARAMETERS* par; // 参数对象，不同的参数对应不同的译码能力

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

}WJL_ERRRECOVERY_ENCODER;

// 纠错函数传递结构体
typedef struct
{
	int max_errbits;              // errbits的最大值
	int listIndex;                // list的下标
	int errbits;                  // 当前纠错比特
	int findPos;                  // 检错译码找到的错误字节位置
	int determine;                // 根据findPos和DETECTION_LENGTH判定纠错区间内首个错误字节是否被纠正
	int compare;                  // 记录最大可能性的判决
	int startBytePos;             // 根据findPos和START_LIMIT确定的纠错区间下限，指InBytesArray数组的下标
	int endBytePos;               // 根据findPos和END_LIMIT确定的纠错区间下限，指InBytesArray数组的下标
	int endpos;                   // 需要翻转的比特区间上限，endBytePos对应字节的最后一个比特位
	int startpos;                 // 需要翻转的比特区间下限，startBytePos对应字节的第一个比特位
	int correctedBytePos;         // 被纠正字节的下标，指InBytesArray数组的下标
	int max_checkfindPos;         // 记录检错译码最大findPos
}WJL_FUNCTION_PARAMETERS;

// 纠错完毕后也需要解码器
typedef struct
{
	// 输入
	WJL_ALGORITHM_PARAMETERS* par;           // 参数对象，不同的参数对应不同的译码能力

	unsigned char* InBytesArray;             // 输入字节缓存数组
	unsigned int InBytesLength;              // 输入字节的总长度
	unsigned int InBytesIndex;               // InBytesArray的下标
	// 输出
	unsigned char* OutBytesArray;            // 输出字节缓存数组
	unsigned int OutBytesLength;             // 输出字节的总长度
	unsigned int OutBytesIndex;              // OutBytesArray的下标

	unsigned int Li;
	unsigned int Ri;
	unsigned int value;		                 // 解码时用来保存输入编码（接收端为U）

	unsigned char mask;                      // 字节轮值，8个比特组合成一个字节
	unsigned char outbyte;                   // 输出的字节

	unsigned char full;                      // 按照比特译码，输出是否满足一个字节
	unsigned char status;                    // 状态，0x01表示译码状态，0x00表示纠错状态，用来是否在List中缓存coder

	unsigned char keepBackSymbol;            // 用于信源处理方法判断的结构体

	unsigned char* BytesArray;               // 纠错时缓存InBytesArray
	
	// 检错译码参数传递变量，仅需两个迭代使用即可
	WJL_FUNCTION_PARAMETERS* wfp;

}WJL_ERRRECOVERY_DECODER;

#ifdef	__cplusplus
extern "C" {
#endif
	/******************************************************************************************
	纠错编码函数
	******************************************************************************************/
	int WJLErrRecoveryEncoder(WJL_ERRRECOVERY_ENCODER* coder);

	/******************************************************************************************
	纠错译码函数
	******************************************************************************************/
	int WJLErrRecoveryDecoder(WJL_ERRRECOVERY_DECODER* coder, WJL_ERRRECOVERY_DECODER** list);

#ifdef	__cplusplus
}
#endif
#endif
