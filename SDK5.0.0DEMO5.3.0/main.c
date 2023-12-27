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
	int i, count = 0, inputerr = 0, printSign = 0, type = BPSK;
	double ber, EbN0_dB = 0.0;

	// 测试的总帧数，每帧独立通过“调制-AWGN信道传输-解调”仿真
	int loop = 1;
	// 每帧中块的数量，最低设置为1，最大不限制，BlockSize * BytesForEachBlock是每帧的随机数
	int BlockSize = 1;
	// 每块字节数量，决定了编码码率，20编码码率为0.5
	int BytesForEachBlock = 20;

	printf("SDK Version 5.0.0\nDEMO Version 5.3.0\n\n");
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
	for (i = 0; i < loop; ++i) {
		printf("-----------------------------------------------------------------------------------------------------\n");
		printf("第%d帧测试:\n", i);
		printf("The %d frame testing:\n", i);
		if (AWGN_ERRRECOVERY(type, BlockSize, BytesForEachBlock, EbN0_dB, 1, &BerSum, &BlerSum)) {
			count++;
		}
		printf("AWGN信道仿真下的平均BER为(The average BER under the AWGN channel simulation is)：%1.12f\n", BerSum / ((double)i + 1));
		printf("纠错译码后的平均BLER为(The average BLER after error-corrected decoding is)：%1.12f\n", BlerSum / ((double)i + 1));
		printf("累计正确译码帧数%d，FER为：%1.12f:\n", count, ((double)i + 1.0 - (double)count) / ((double)i + 1.0));
		printf("the cumulative number of correctly decoding frames %d，FER为：%1.12f:\n", count, ((double)i + 1.0 - (double)count) / ((double)i + 1.0));
		printf("\n");
	}
	printf("全部%d帧测试结束, 最终正确译码帧数%d, 最终FER为：%1.12f：\n", loop, count, ((double)i - (double)count) / (double)i);
	printf("All %d Frames test ended, Final number of correctly decoding frames %d, The final FER is: %1.12f:\n", loop, count, ((double)i - (double)count) / (double)i);
	system("pause");
	return 0;
}