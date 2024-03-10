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
	double BerSum = 0.0;
	double BlerSum = 0.0;
	int i, count = 0, EncodedBytes = 0, inputerr = 0, printSign = 0, type = BPSK;
	double ber, EbN0_dB = 0.0;
	PARAMETERS* par = NULL;
	// ���Ե���֡����ÿ֡����ͨ��������-AWGN�ŵ�����-���������
	int loop = 1;
	// ÿ֡�п���������������Ϊ1��������ƣ�BlockSize * BytesForEachBlock��ÿ֡�������
	int BlockSize = 1;
	// ÿ���ֽ������������˱������ʣ�20��������Ϊ0.5
	int BytesForEachBlock = 20;
	par = (PARAMETERS*)malloc(sizeof(PARAMETERS));
	if(par == NULL) return 0;
	// �������������г���
	par->DECODER_LIST_SIZE = 64;
	// ����Ĳ������������еĲ���
	par->START_LIMIT = 7;
	par->END_LIMIT = 7;
	par->ERRBITS_LIMIT = 5;
	par->MAXIMUM_TRAVERSAL_TIMES = 8;

	printf("SDK Version 5.2.0\nDEMO Version 5.5.0\n\n");
STEP_1:
	printf("�����������������->��\n(Please enter the upper limit of the error correction interval -> ��):\n");
	inputerr = scanf_s("%d", &par->START_LIMIT);
	// ��֤�Ƿ�Ϸ�
	if (par->START_LIMIT >= 96) {
		printf("������! (�� error!),��<=96\n");
		goto STEP_1;
	}
	// �����ָV�ĳ���
	par->START_LIMIT = par->START_LIMIT / 8;
STEP_2:
	printf("�����������������->��\n(Please enter the lower limit of the error correction interval -> ��):\n");
	inputerr = scanf_s("%d", &par->END_LIMIT);
	// ��֤�Ƿ�Ϸ�
	if (par->END_LIMIT >= 128) {
		printf("�´���! (�� error!),��<=128\n");
		goto STEP_2;
	}
	// �����ָV�ĳ���
	par->END_LIMIT = par->END_LIMIT / 8;
STEP_3:
	printf("������ѭ���������Ʋ���->��\n(Please enter cycle number control parameters -> ��):\n");
	inputerr = scanf_s("%d", &par->MAXIMUM_TRAVERSAL_TIMES);
	// ��֤�Ƿ�Ϸ�
	if (par->MAXIMUM_TRAVERSAL_TIMES >= 16) {
		printf("�ش���! (�� error!),��<=16\n");
		goto STEP_3;
	}
STEP_4:
	printf("��������������������ش������->��_max\n(Please enter the maximum number of bit errors in the error correction interval -> ��_max):\n");
	inputerr = scanf_s("%d", &par->ERRBITS_LIMIT);
	// ��֤�Ƿ�Ϸ�
	if (par->ERRBITS_LIMIT >= 10) {
		printf("��_max����! (��_max error!),��_max<=10\n");
		goto STEP_4;
	}

STEP0:
	printf("��������Ʒ���(0��ʾBPSK��1��ʾQPSK)\n(Please enter the modulation method (0 is BPSK, 1 is QPSK)):\n");
	inputerr = scanf_s("%d", &type);
	// ��֤�Ƿ�Ϸ�
	if (type != 1 && type != 0) {
		printf("���Ʒ�������!(the modulation method error!)\n");
		goto STEP0;
	}
STEP1:
	printf("�������ܹ���Ҫ���Ե�֡��(����С��1)\n(Please enter the number of test frames (not less than 1)):\n");
	inputerr = scanf_s("%d", &loop);
	// ��֤�Ƿ�Ϸ�
	if (loop < 1) {
		printf("֡������Ϊ1��!(At least 1 frame count!)\n");
		goto STEP1;
	}
STEP2:
	printf("������ÿ֡�Ŀ���(����С��1)\n(Enter the number of blocks per frame (not less than 1)):\n");
	inputerr = scanf_s("%d", &BlockSize);
	if (BlockSize < 1) {
		printf("ÿ֡��������Ϊ1��!(At least 1 block per frame!)\n");
		goto STEP2;
	}
STEP3:
	printf("������ÿ����ֽ���(����С��1)\n(Enter the number of bytes per block (not less than 1)):\n");
	inputerr = scanf_s("%d", &BytesForEachBlock);
	if (BytesForEachBlock < 1) {
		printf("ÿ�����ֽ�������Ϊ1��!(At least 1 byte per block!)\n");
		goto STEP3;
	}
STEP4:
	printf("������AWGN�����Eb/N0(dB)(���ڵ���100��Ϊ������)\n(Please enter AWGN Eb/N0(dB)(greater than 100 as correct transmission)):\n");
	inputerr = scanf_s("%lf", &EbN0_dB);
	if (EbN0_dB < -10.0) {
		printf("���������-10.0��ֵ!(Please enter a value greater than -10.0!)\n");
		goto STEP4;
	}

	printf("�������Ƿ��ӡ�����(0����ӡ,1��ӡ)\n(Please enter whether to print the random number (0 no print, 1 print)):\n");
	inputerr = scanf_s("%d", &printSign);
	
	// ����ͨ��for����10��100����������ݵ�ʵ��
	for (i = 1; i <= loop; ++i) {
		printf("-----------------------------------------------------------------------------------------------------\n");
		printf("��%d֡����:\n", i);
		printf("The %d frame testing:\n", i);
		if (AWGN_ERRRECOVERY(par, type, BlockSize, BytesForEachBlock, EbN0_dB, 1, &EncodedBytes, &BerSum, &BlerSum)) {
			count++;
		}
		printf("AWGN�ŵ������µ�ƽ��BERΪ(The average BER under the AWGN channel simulation is)��%1.12f\n", BerSum / ((double)EncodedBytes * 8.0));
		printf("����������ƽ��BLERΪ(The average BLER after error-corrected decoding is)��%1.12f\n", BlerSum / ((double)i * BlockSize));
		printf("�ۼ���ȷ����֡��%d��FERΪ��%1.12f:\n", count, ((double)i - (double)count) / (double)i);
		printf("the cumulative number of correctly decoding frames %d��FERΪ��%1.12f:\n", count, ((double)i - (double)count) / (double)i);
		printf("\n");
	}
	printf("ȫ��%d֡���Խ���, ������ȷ����֡��%d, ����FERΪ��%1.12f��\n", loop, count, ((double)i - (double)count) / (double)i);
	printf("All %d Frames test ended, Final number of correctly decoding frames %d, The final FER is: %1.12f:\n", loop, count, ((double)i - (double)count) / (double)i);
	if (par) free(par);
	system("pause");
	return 0;
}