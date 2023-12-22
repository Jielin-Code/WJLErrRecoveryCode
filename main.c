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
// ��������µĴ������ݾ���ʵ��
int main() { // 2
	int i, size = 1, count = 0, inputerr = 0, printSign = 0;
	double ber, EbN0_dB = 0.0;
	int bytes = 1;
STEP1:
	printf("���������֡���ֽڳ���(����С��1)\n(Please enter the byte length of the random frame (not less than 1)):\n");
	inputerr = scanf_s("%d", &bytes);
	// ��֤�Ƿ�Ϸ�
	if (bytes < 1) {
		printf("�ֽڳ��ȴ���!(Byte length error!)\n");
		goto STEP1;
	}
STEP2:
	printf("��������Ҫ����֡������(����С��1)\n(Please enter the number of test frames required (not less than 1)):\n");
	inputerr = scanf_s("%d", &size);
	if (size < 1) {
		printf("֡������Ϊ1��!(At least 1 frame count!)\n");
		goto STEP2;
	}
	printf("������AWGN_QPSK�����Eb/N0(dB)(���ڵ���100��Ϊ������)\n(Please enter AWGN_QPSK Eb/N0(dB)(greater than 100 as correct transmission)):\n");
	inputerr = scanf_s("%lf", &EbN0_dB);
	printf("�������Ƿ��ӡ�����(0����ӡ,1��ӡ)\n(Please enter whether to print the random number (0 no print, 1 print)):\n");
	inputerr = scanf_s("%d", &printSign);
	
	// ����ͨ��for����10��100����������ݵ�ʵ��
	for (i = 0; i < size; ++i) {
		printf("-----------------------------------------------------------------------------------------------------\n");
		printf("��%d֡�������,�ۼ���ȷ����֡��%d��FERΪ��%1.12f:\n", i, count, ((double)i - (double)count)/ (double)i);
		printf("Frame %d was randomly tested, the cumulative number of correctly decoding frames %d��FERΪ��%1.6f:\n", i, count, ((double)i - (double)count) / (double)i);
		if (QPSK_AWGN_ERRRECOVERY(bytes, EbN0_dB, printSign)) {
			count++;
		}
	}
	printf("ȫ��%d֡���Խ���, ������ȷ����֡��%d, ����FERΪ��%1.6f��\n", size, count, ((double)i - (double)count) / (double)i);
	printf("All %d Frames test ended, Final number of correctly decoding frames %d, The final FER is: %1.6f:\n", size, count, ((double)i - (double)count) / (double)i);
	system("pause");
	return 0;
}