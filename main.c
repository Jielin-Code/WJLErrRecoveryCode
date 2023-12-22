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
	int i, size = 1, count = 0, inputerr = 0, printSign = 0;
	double ber, EbN0_dB = 0.0;
	int bytes = 1;
STEP1:
	printf("请输入随机帧的字节长度(不得小于1)\n(Please enter the byte length of the random frame (not less than 1)):\n");
	inputerr = scanf_s("%d", &bytes);
	// 验证是否合法
	if (bytes < 1) {
		printf("字节长度错误!(Byte length error!)\n");
		goto STEP1;
	}
STEP2:
	printf("请输入需要测试帧的数量(不得小于1)\n(Please enter the number of test frames required (not less than 1)):\n");
	inputerr = scanf_s("%d", &size);
	if (size < 1) {
		printf("帧数至少为1个!(At least 1 frame count!)\n");
		goto STEP2;
	}
	printf("请输入AWGN_QPSK信噪比Eb/N0(dB)(大于等于100视为无误传输)\n(Please enter AWGN_QPSK Eb/N0(dB)(greater than 100 as correct transmission)):\n");
	inputerr = scanf_s("%lf", &EbN0_dB);
	printf("请输入是否打印随机数(0不打印,1打印)\n(Please enter whether to print the random number (0 no print, 1 print)):\n");
	inputerr = scanf_s("%d", &printSign);
	
	// 可以通过for进行10万、100万组随机数据的实测
	for (i = 0; i < size; ++i) {
		printf("-----------------------------------------------------------------------------------------------------\n");
		printf("第%d帧随机测试,累计正确译码帧数%d，FER为：%1.12f:\n", i, count, ((double)i - (double)count)/ (double)i);
		printf("Frame %d was randomly tested, the cumulative number of correctly decoding frames %d，FER为：%1.6f:\n", i, count, ((double)i - (double)count) / (double)i);
		if (QPSK_AWGN_ERRRECOVERY(bytes, EbN0_dB, printSign)) {
			count++;
		}
	}
	printf("全部%d帧测试结束, 最终正确译码帧数%d, 最终FER为：%1.6f：\n", size, count, ((double)i - (double)count) / (double)i);
	printf("All %d Frames test ended, Final number of correctly decoding frames %d, The final FER is: %1.6f:\n", size, count, ((double)i - (double)count) / (double)i);
	system("pause");
	return 0;
}