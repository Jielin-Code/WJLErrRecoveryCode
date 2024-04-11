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
	int i, j, sign, count = 0, EncodedBytesCount = 0, inputerr = 0, printSign = 0, model = BPSK;
	double EbN0_dB = 0.0;
	WJL_ALGORITHM_PARAMETERS* par = NULL;
	// ���Ե���֡����ÿ֡����ͨ��������-AWGN�ŵ�����-���������
	int loop = 1;
	// ÿ֡�п���������������Ϊ1��������ƣ�BlockSize * BytesForEachBlock��ÿ֡�������
	int BlockSize = 1;
	printf("SDK Version 6.0.0\nTESTING DEMO Version 6.0.0\n\n");
	par = (WJL_ALGORITHM_PARAMETERS*)malloc(sizeof(WJL_ALGORITHM_PARAMETERS));
	if (par == NULL) return 0;
	par->LIST_SIZE = 48;
	par->CODE_LENGTH = 8;      // ͬ���ֽں�����Ϊ8�ɵ�
	par->Q_LENGTH = 3;         // ��СֵΪ1��ĩβ����Q_LENGTH��0xFF��������ĩβ�ֽڵľ���У�鳤�ȣ�ĩβһ����Χ�ڵ��ֽڻ������ֵͬ
	par->START_LIMIT = 10;     // 5dB��6dB����˴�Ϊ8
	par->END_LIMIT = 8;
	par->ERRBITS_LIMIT = 10;   // ���Ϊ10����СΪ1
	par->ERGODIC_MODEL = 0;    // 0��ʾ��1���ط�ת������ERRBITS_LIMIT���صķ�ת��1��ʾ��ERRBITS_LIMIT���ط�ת������1���صķ�ת
	par->SynchronizationBytes = (unsigned char*)malloc(par->CODE_LENGTH * 10);
	par->SynchronizationLength = 0;
	// �����Ҫͬ�����ֽڣ����Ҵ洢��par->SynchronizationBytes��
	SynchronizationBytes(par);
STEP_0:
	printf("�������볤(��λ������)-> n \n(Please enter the code length (Unit: bits) -> n ):\n");
	inputerr = scanf_s("%d", &par->CODE_LENGTH);
	// ��֤�Ƿ�Ϸ�
	if (par->START_LIMIT <= 0) {
		printf("n����! (n error!)\n");
		goto STEP_0;
	}
	// �����ָV�ĳ���
	par->CODE_LENGTH = par->CODE_LENGTH >> 3;
STEP_1:
	printf("�����������������(��λ������)->��\n(Please enter the upper limit of the error correction interval(Unit: bits) -> ��):\n");
	inputerr = scanf_s("%d", &par->START_LIMIT);
	// ��֤�Ƿ�Ϸ�
	if (par->START_LIMIT >= 96) {
		printf("������! (�� error!),��<= 96\n");
		goto STEP_1;
	}
	// �����ָV�ĳ���
	par->START_LIMIT = par->START_LIMIT >> 3;
STEP_2:
	printf("�����������������(��λ������)->��\n(Please enter the lower limit of the error correction interval(Unit: bits) -> ��):\n");
	inputerr = scanf_s("%d", &par->END_LIMIT);
	// ��֤�Ƿ�Ϸ�
	if (par->END_LIMIT >= 128) {
		printf("�´���! (�� error!),��<=128\n");
		goto STEP_2;
	}
	// �����ָV�ĳ���
	par->END_LIMIT = par->END_LIMIT >> 3;
STEP_3:
	printf("������ĩβ������0xFF�ĸ���->��\n(Enter the number to be encoded 0xFF at the end ->��):\n");
	inputerr = scanf_s("%d", &par->Q_LENGTH);
	// ��֤�Ƿ�Ϸ�
	if (par->Q_LENGTH >= 10) {
		printf("�ʴ���! (�� error!),��<=10\n");
		goto STEP_3;
	}
STEP_4:
	printf("��������������������ش������->��_max\n(Please enter the maximum number of bit errors in the error correction interval -> ��_max):\n");
	inputerr = scanf_s("%d", &par->ERRBITS_LIMIT);
	// ��֤�Ƿ�Ϸ�
	if (par->ERRBITS_LIMIT > 10) {
		printf("��_max����! (��_max error!),��_max<=10\n");
		goto STEP_4;
	}
STEP0:
	printf("��������Ʒ�ʽ(0ΪBPSK��1ΪQPSK)\n(Please enter the modulation mode (0 for BPSK, 1 for QPSK)):\n");
	inputerr = scanf_s("%d", &model);
	// ��֤�Ƿ�Ϸ�
	if (model != 0 && model != 1) {
		printf("ֻ������0��1!(only enter 0 or 1!)\n");
		goto STEP1;
	}
STEP1:
	printf("�������ܹ���Ҫ���Ե�֡��(����С��1)\n(Please enter the number of test frames (not less than 1)):\n");
	inputerr = scanf_s("%d", &loop);
	// ��֤�Ƿ�Ϸ�
	if (loop < 1) {
		printf("֡������Ϊ1��!(At least 1 frame count!)\n");
		goto STEP1;
	}
//STEP2:
//	printf("������ÿ֡�Ŀ���(����С��1)\n(Enter the number of blocks per frame (not less than 1)):\n");
//	inputerr = scanf_s("%d", &BlockSize);
//	if (BlockSize < 1) {
//		printf("ÿ֡��������Ϊ1��!(At least 1 block per frame!)\n");
//		goto STEP2;
//	}
STEP3:
	printf("���������ģʽ(0��ʾ��1���ط�ת��������_max��1��ʾ�Ӧ�_max���ط�ת������1���ط�ת)\n(Enter the traversal mode (0 for flipping from 1 bit to ��_max, 1 for flipping from ��_max bit to 1 bit to flip)):\n");
	inputerr = scanf_s("%d", &par->ERGODIC_MODEL);
	if (BlockSize < 1) {
		printf("ÿ֡��������Ϊ1��!(At least 1 block per frame!)\n");
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
	i = 0;
	while (i <= loop) {
		printf("-----------------------------------------------------------------------------------------------------\n");
		printf("��%d֡����:\n", i);
		printf("The %d frame testing:\n", i);
		// ��Ҫͬ�����ֽ�Ϊ
		printf("��Ҫͬ�����ֽ���%d��(There are %d bytes to synchronize):\n", par->SynchronizationLength, par->SynchronizationLength);
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
		printf("AWGN�ŵ������µ�ƽ��BERΪ(The average BER under the AWGN channel simulation is)��%1.12f\n", BerSum / ((double)EncodedBytesCount * 8.0));
		printf("����������ƽ��BLERΪ(The average BLER after error-corrected decoding is)��%1.12f\n", BlerSum / ((double)i * BlockSize));
		printf("�ۼ���ȷ����֡��%d��FERΪ��%1.12f:\n", count, ((double)i - (double)count) / (double)i);
		printf("the cumulative number of correctly decoding frames %d��FERΪ��%1.12f:\n", count, ((double)i - (double)count) / (double)i);
		printf("\n");
	}
	printf("ȫ��%d֡���Խ���, ������ȷ����֡��%d, ����FERΪ��%1.12f��\n", loop, count, ((double)i - (double)count) / (double)i);
	printf("All %d Frames test ended, Final number of correctly decoding frames %d, The final FER is: %1.12f:\n", loop, count, ((double)i - (double)count) / (double)i);
	if (par->SynchronizationBytes) free(par->SynchronizationBytes);
	if (par) free(par);
	system("pause");
	return 0;
}