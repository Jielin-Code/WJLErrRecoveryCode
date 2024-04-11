#include "test.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <math.h>
#ifdef WIN32
#define  inline __inline
#endif // WIN32

// 带信噪比下的错误数据纠错实验
int main() { // 2
	double BerSum = 0.0;
	double BlerSum = 0.0;
	int i, j, sign, count = 0, EncodedBytesCount = 0, inputerr = 0, printSign = 0, model = BPSK;
	double EbN0_dB = 0.0;
	WJL_ALGORITHM_PARAMETERS* par = NULL;
	// 测试的总帧数，每帧独立通过“调制-AWGN信道传输-解调”仿真
	int loop = 1;
	// 每帧中块的数量，最低设置为1，最大不限制，BlockSize * BytesForEachBlock是每帧的随机数
	int BlockSize = 1;
	printf("SDK Version 6.0.0\nTESTING DEMO Version 6.0.0\n\n");
	par = (WJL_ALGORITHM_PARAMETERS*)malloc(sizeof(WJL_ALGORITHM_PARAMETERS));
	if (par == NULL) return 0;
	par->LIST_SIZE = 48;
	par->CODE_LENGTH = 8;      // 同步字节后，设置为8可得
	par->Q_LENGTH = 3;         // 最小值为1，末尾编码Q_LENGTH个0xFF，以满足末尾字节的纠错校验长度，末尾一定范围内的字节会出现相同值
	par->START_LIMIT = 10;     // 5dB和6dB检验此处为8
	par->END_LIMIT = 8;
	par->ERRBITS_LIMIT = 10;   // 最大为10，最小为1
	par->ERGODIC_MODEL = 0;    // 0表示从1比特翻转遍历到ERRBITS_LIMIT比特的翻转，1表示从ERRBITS_LIMIT比特翻转遍历到1比特的翻转
	par->SynchronizationBytes = (unsigned char*)malloc(par->CODE_LENGTH * 10);
	par->SynchronizationLength = 0;
	// 求出需要同步的字节，并且存储在par->SynchronizationBytes中
	SynchronizationBytes(par);
STEP_0:
	printf("请输入码长(单位：比特)-> n \n(Please enter the code length (Unit: bits) -> n ):\n");
	inputerr = scanf_s("%d", &par->CODE_LENGTH);
	// 验证是否合法
	if (par->START_LIMIT <= 0) {
		printf("n错误! (n error!)\n");
		goto STEP_0;
	}
	// 这个是指V的长度
	par->CODE_LENGTH = par->CODE_LENGTH >> 3;
STEP_1:
	printf("请输入纠错区间上限(单位：比特)->α\n(Please enter the upper limit of the error correction interval(Unit: bits) -> α):\n");
	inputerr = scanf_s("%d", &par->START_LIMIT);
	// 验证是否合法
	if (par->START_LIMIT >= 96) {
		printf("α错误! (α error!),α<= 96\n");
		goto STEP_1;
	}
	// 这个是指V的长度
	par->START_LIMIT = par->START_LIMIT >> 3;
STEP_2:
	printf("请输入纠错区间下限(单位：比特)->β\n(Please enter the lower limit of the error correction interval(Unit: bits) -> β):\n");
	inputerr = scanf_s("%d", &par->END_LIMIT);
	// 验证是否合法
	if (par->END_LIMIT >= 128) {
		printf("β错误! (β error!),β<=128\n");
		goto STEP_2;
	}
	// 这个是指V的长度
	par->END_LIMIT = par->END_LIMIT >> 3;
STEP_3:
	printf("请输入末尾待编码0xFF的个数->κ\n(Enter the number to be encoded 0xFF at the end ->κ):\n");
	inputerr = scanf_s("%d", &par->Q_LENGTH);
	// 验证是否合法
	if (par->Q_LENGTH >= 10) {
		printf("κ错误! (κ error!),κ<=10\n");
		goto STEP_3;
	}
STEP_4:
	printf("请输入纠错区间内最大比特错误个数->τ_max\n(Please enter the maximum number of bit errors in the error correction interval -> τ_max):\n");
	inputerr = scanf_s("%d", &par->ERRBITS_LIMIT);
	// 验证是否合法
	if (par->ERRBITS_LIMIT > 10) {
		printf("τ_max错误! (τ_max error!),τ_max<=10\n");
		goto STEP_4;
	}
STEP0:
	printf("请输入调制方式(0为BPSK，1为QPSK)\n(Please enter the modulation mode (0 for BPSK, 1 for QPSK)):\n");
	inputerr = scanf_s("%d", &model);
	// 验证是否合法
	if (model != 0 && model != 1) {
		printf("只能输入0或1!(only enter 0 or 1!)\n");
		goto STEP1;
	}
STEP1:
	printf("请输入总共需要测试的帧数(不得小于1)\n(Please enter the number of test frames (not less than 1)):\n");
	inputerr = scanf_s("%d", &loop);
	// 验证是否合法
	if (loop < 1) {
		printf("帧数至少为1个!(At least 1 frame count!)\n");
		goto STEP1;
	}
//STEP2:
//	printf("请输入每帧的块数(不得小于1)\n(Enter the number of blocks per frame (not less than 1)):\n");
//	inputerr = scanf_s("%d", &BlockSize);
//	if (BlockSize < 1) {
//		printf("每帧块数至少为1个!(At least 1 block per frame!)\n");
//		goto STEP2;
//	}
STEP3:
	printf("请输入遍历模式(0表示从1比特翻转遍历到τ_max，1表示从τ_max比特翻转遍历到1比特翻转)\n(Enter the traversal mode (0 for flipping from 1 bit to τ_max, 1 for flipping from τ_max bit to 1 bit to flip)):\n");
	inputerr = scanf_s("%d", &par->ERGODIC_MODEL);
	if (BlockSize < 1) {
		printf("每帧块数至少为1个!(At least 1 block per frame!)\n");
		goto STEP3;
	}
STEP4:
	printf("请输入AWGN信噪比Eb/N0(dB)(大于等于100视为无误传输)\n(Please enter AWGN Eb/N0(dB)(greater than 100 as correct transmission)):\n");
	inputerr = scanf_s("%lf", &EbN0_dB);
	if (EbN0_dB < -10.0) {
		printf("请输入大于-10.0的值!(Please enter a value greater than -10.0!)\n");
		goto STEP4;
	}

	printf("请输入是否打印随机数(0不打印,1打印)\n(Please enter whether to print the random number (0 no print, 1 print)):\n");
	inputerr = scanf_s("%d", &printSign);

	// 可以通过for进行10万、100万组随机数据的实测
	i = 0;
	while (i <= loop) {
		printf("-----------------------------------------------------------------------------------------------------\n");
		printf("第%d帧测试:\n", i);
		printf("The %d frame testing:\n", i);
		// 需要同步的字节为
		printf("需要同步的字节有%d个(There are %d bytes to synchronize):\n", par->SynchronizationLength, par->SynchronizationLength);
		for (j = 0; j < par->SynchronizationLength; ++j) {
			printf("%02X,", par->SynchronizationBytes[j]);
		}
		printf("\n");
		sign = AWGN_ERRRECOVERY(par, model, BlockSize, EbN0_dB, 1, &EncodedBytesCount, &BerSum, &BlerSum);
		if (sign == 1) {
			count++;
			i++;
		}
		else if (sign == 0) i++;
		printf("AWGN信道仿真下的平均BER为(The average BER under the AWGN channel simulation is)：%1.12f\n", BerSum / ((double)EncodedBytesCount * 8.0));
		printf("纠错译码后的平均BLER为(The average BLER after error-corrected decoding is)：%1.12f\n", BlerSum / ((double)i * BlockSize));
		printf("累计正确译码帧数%d，FER为：%1.12f:\n", count, ((double)i - (double)count) / (double)i);
		printf("the cumulative number of correctly decoding frames %d，FER为：%1.12f:\n", count, ((double)i - (double)count) / (double)i);
		printf("\n");
	}
	printf("全部%d帧测试结束, 最终正确译码帧数%d, 最终FER为：%1.12f：\n", loop, count, ((double)i - (double)count) / (double)i);
	printf("All %d Frames test ended, Final number of correctly decoding frames %d, The final FER is: %1.12f:\n", loop, count, ((double)i - (double)count) / (double)i);
	if (par->SynchronizationBytes) free(par->SynchronizationBytes);
	if (par) free(par);
	system("pause");
	return 0;
}