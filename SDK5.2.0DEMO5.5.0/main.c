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
	int i, count = 0, EncodedBytes = 0, inputerr = 0, printSign = 0, type = BPSK;
	double ber, EbN0_dB = 0.0;
	PARAMETERS* par = NULL;
	// 测试的总帧数，每帧独立通过“调制-AWGN信道传输-解调”仿真
	int loop = 1;
	// 每帧中块的数量，最低设置为1，最大不限制，BlockSize * BytesForEachBlock是每帧的随机数
	int BlockSize = 1;
	// 每块字节数量，决定了编码码率，20编码码率为0.5
	int BytesForEachBlock = 20;
	par = (PARAMETERS*)malloc(sizeof(PARAMETERS));
	if(par == NULL) return 0;
	// 设置译码器队列长度
	par->DECODER_LIST_SIZE = 64;
	// 下面的参数就是论文中的参数
	par->START_LIMIT = 7;
	par->END_LIMIT = 7;
	par->ERRBITS_LIMIT = 5;
	par->MAXIMUM_TRAVERSAL_TIMES = 8;

	printf("SDK Version 5.2.0\nDEMO Version 5.5.0\n\n");
STEP_1:
	printf("请输入纠错区间上限->α\n(Please enter the upper limit of the error correction interval -> α):\n");
	inputerr = scanf_s("%d", &par->START_LIMIT);
	// 验证是否合法
	if (par->START_LIMIT >= 96) {
		printf("α错误! (α error!),α<=96\n");
		goto STEP_1;
	}
	// 这个是指V的长度
	par->START_LIMIT = par->START_LIMIT / 8;
STEP_2:
	printf("请输入纠错区间下限->β\n(Please enter the lower limit of the error correction interval -> β):\n");
	inputerr = scanf_s("%d", &par->END_LIMIT);
	// 验证是否合法
	if (par->END_LIMIT >= 128) {
		printf("β错误! (β error!),β<=128\n");
		goto STEP_2;
	}
	// 这个是指V的长度
	par->END_LIMIT = par->END_LIMIT / 8;
STEP_3:
	printf("请输入循环次数控制参数->ω\n(Please enter cycle number control parameters -> ω):\n");
	inputerr = scanf_s("%d", &par->MAXIMUM_TRAVERSAL_TIMES);
	// 验证是否合法
	if (par->MAXIMUM_TRAVERSAL_TIMES >= 16) {
		printf("ω错误! (ω error!),ω<=16\n");
		goto STEP_3;
	}
STEP_4:
	printf("请输入纠错区间内最大比特错误个数->τ_max\n(Please enter the maximum number of bit errors in the error correction interval -> τ_max):\n");
	inputerr = scanf_s("%d", &par->ERRBITS_LIMIT);
	// 验证是否合法
	if (par->ERRBITS_LIMIT >= 10) {
		printf("τ_max错误! (τ_max error!),τ_max<=10\n");
		goto STEP_4;
	}

STEP0:
	printf("请输入调制方法(0表示BPSK，1表示QPSK)\n(Please enter the modulation method (0 is BPSK, 1 is QPSK)):\n");
	inputerr = scanf_s("%d", &type);
	// 验证是否合法
	if (type != 1 && type != 0) {
		printf("调制方法错误!(the modulation method error!)\n");
		goto STEP0;
	}
STEP1:
	printf("请输入总共需要测试的帧数(不得小于1)\n(Please enter the number of test frames (not less than 1)):\n");
	inputerr = scanf_s("%d", &loop);
	// 验证是否合法
	if (loop < 1) {
		printf("帧数至少为1个!(At least 1 frame count!)\n");
		goto STEP1;
	}
STEP2:
	printf("请输入每帧的块数(不得小于1)\n(Enter the number of blocks per frame (not less than 1)):\n");
	inputerr = scanf_s("%d", &BlockSize);
	if (BlockSize < 1) {
		printf("每帧块数至少为1个!(At least 1 block per frame!)\n");
		goto STEP2;
	}
STEP3:
	printf("请输入每块的字节数(不得小于1)\n(Enter the number of bytes per block (not less than 1)):\n");
	inputerr = scanf_s("%d", &BytesForEachBlock);
	if (BytesForEachBlock < 1) {
		printf("每块中字节数至少为1个!(At least 1 byte per block!)\n");
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
	for (i = 1; i <= loop; ++i) {
		printf("-----------------------------------------------------------------------------------------------------\n");
		printf("第%d帧测试:\n", i);
		printf("The %d frame testing:\n", i);
		if (AWGN_ERRRECOVERY(par, type, BlockSize, BytesForEachBlock, EbN0_dB, 1, &EncodedBytes, &BerSum, &BlerSum)) {
			count++;
		}
		printf("AWGN信道仿真下的平均BER为(The average BER under the AWGN channel simulation is)：%1.12f\n", BerSum / ((double)EncodedBytes * 8.0));
		printf("纠错译码后的平均BLER为(The average BLER after error-corrected decoding is)：%1.12f\n", BlerSum / ((double)i * BlockSize));
		printf("累计正确译码帧数%d，FER为：%1.12f:\n", count, ((double)i - (double)count) / (double)i);
		printf("the cumulative number of correctly decoding frames %d，FER为：%1.12f:\n", count, ((double)i - (double)count) / (double)i);
		printf("\n");
	}
	printf("全部%d帧测试结束, 最终正确译码帧数%d, 最终FER为：%1.12f：\n", loop, count, ((double)i - (double)count) / (double)i);
	printf("All %d Frames test ended, Final number of correctly decoding frames %d, The final FER is: %1.12f:\n", loop, count, ((double)i - (double)count) / (double)i);
	if (par) free(par);
	system("pause");
	return 0;
}