#pragma once
/******************************************************************************************
���ڽ���������㷨���ۣ������ڷ���������Դ��������
1�����ݡ�������ԭ��Ӧ�á��ķ�����������X�еķ���0�滻Ϊ101,�ҽ�����1�滻Ϊ01�������ʵĽǶ�����������һ�����ʲ������ģ����Ǿ�������ȴ��������
2������λ��ת�ľ���������Ч���������Ż���Ч��������֮ǰ�汾����˰������ϣ�
3�����֧��0dB�ľ���������CPU�ı�̣���δʵ�ֶ��̵߳ľ����������GPU��NPUЧ�������̶ȵ�������
4�������ֽ�Խ��������Խ�ߣ����۱�������Ϊ-1/log_2(1/3) = 0.630929��
5����ͨ�����ò���ʵ�ֲ�ͬ�̶ȵľ���START_LIMIT��END_LIMIT��COMPARE_LIMIT��ERRBITS_LIMIT��
6��LIST_SIZE�������鲻Ҫ���õ�̫�ͣ�����С��START_LIMIT + END_LIMIT + 1

���ۣ���������ԭ��Ӧ�á�
���ߣ�������
���������ڼ�Ȩ����ģ�͵ľ����㷨����ȫ�µľ����㷨
ʱ�䣺20240314
�汾��6.0.0
BUG�޸�˵����
1����ȫ�������ı�д�Ĳ��Գ���
2������Ч������
******************************************************************************************/
#ifndef _WJLERRRECOVERYCORE_H
#define _WJLERRRECOVERYCORE_H

// �����ṹ�壬��Щֵ��Ӱ�����Ч���ĺ��Ĳ��������Ը��ݲ�ͬ���ŵ����ò�ͬ�Ĳ���
typedef struct
{
	// WJL_ERRRECOVERY_DECODER�����б��С��һ������Ϊ24��36�����listIndex >= coder->par->LIST_SIZE˵��LIST_SIZE���õ�̫С��
	int LIST_SIZE;
	// �볤����λΪ�ֽڣ������볤��
	int CODE_LENGTH;
	// ĩβ׷�ӵĶ���������Q���ֽڳ��ȣ���Ӧ�����еĦ�
	int Q_LENGTH;
	// ������������޼�ֵ����Ӧ�����еĦ�
	int START_LIMIT;
	// ������������޼�ֵ����Ӧ�����еĦ�
	int END_LIMIT;
	// �������������ת���ظ�������Ӧ�����еĦ�_max
	int ERRBITS_LIMIT;
	// �������ͣ�0��ʾ��1���ش��������ERRBITS_LIMIT��1��ʾ��ERRBITS_LIMIT������1
	int ERGODIC_MODEL;
	// CODE_LENGTH��Q_LENGTHȷ��ʱ�������ĩβ�м����ֽ�����ȫ��ͬ�ģ����������������Щ�ֽ���ȫ����������ʱ���ϣ����贫��Ӷ���ߴ�������
	unsigned char* SynchronizationBytes;
	int SynchronizationLength;

}WJL_ALGORITHM_PARAMETERS;

// ������������ṹ��
typedef struct
{
	// ����
	WJL_ALGORITHM_PARAMETERS* par; // �������󣬲�ͬ�Ĳ�����Ӧ��ͬ����������

	unsigned char* InBytesArray;   // �����ֽڻ�������
	unsigned int InBytesLength;    // �����ֽڵ��ܳ���
	unsigned int InBytesIndex;     // InBytesArray���±�

	// ���
	unsigned char* OutBytesArray;  // ����ֽڻ�������
	unsigned int OutBytesLength;   // ����ֽڵ��ܳ���
	unsigned int OutBytesIndex;    // OutBytesArray���±�

	// �������������Li��Ri������ο���������ԭ��Ӧ�á�һ��
	unsigned int Li;
	unsigned int Ri;
	unsigned int Delay;
	unsigned int DelayCount;

}WJL_ERRRECOVERY_ENCODER;

// ���������ݽṹ��
typedef struct
{
	int max_errbits;              // errbits�����ֵ
	int listIndex;                // list���±�
	int errbits;                  // ��ǰ�������
	int findPos;                  // ��������ҵ��Ĵ����ֽ�λ��
	int determine;                // ����findPos��DETECTION_LENGTH�ж������������׸������ֽ��Ƿ񱻾���
	int compare;                  // ��¼�������Ե��о�
	int startBytePos;             // ����findPos��START_LIMITȷ���ľ����������ޣ�ָInBytesArray������±�
	int endBytePos;               // ����findPos��END_LIMITȷ���ľ����������ޣ�ָInBytesArray������±�
	int endpos;                   // ��Ҫ��ת�ı����������ޣ�endBytePos��Ӧ�ֽڵ����һ������λ
	int startpos;                 // ��Ҫ��ת�ı����������ޣ�startBytePos��Ӧ�ֽڵĵ�һ������λ
	int correctedBytePos;         // �������ֽڵ��±ָ꣬InBytesArray������±�
	int max_checkfindPos;         // ��¼����������findPos
}WJL_FUNCTION_PARAMETERS;

// ������Ϻ�Ҳ��Ҫ������
typedef struct
{
	// ����
	WJL_ALGORITHM_PARAMETERS* par;           // �������󣬲�ͬ�Ĳ�����Ӧ��ͬ����������

	unsigned char* InBytesArray;             // �����ֽڻ�������
	unsigned int InBytesLength;              // �����ֽڵ��ܳ���
	unsigned int InBytesIndex;               // InBytesArray���±�
	// ���
	unsigned char* OutBytesArray;            // ����ֽڻ�������
	unsigned int OutBytesLength;             // ����ֽڵ��ܳ���
	unsigned int OutBytesIndex;              // OutBytesArray���±�

	unsigned int Li;
	unsigned int Ri;
	unsigned int value;		                 // ����ʱ��������������루���ն�ΪU��

	unsigned char mask;                      // �ֽ���ֵ��8��������ϳ�һ���ֽ�
	unsigned char outbyte;                   // ������ֽ�

	unsigned char full;                      // ���ձ������룬����Ƿ�����һ���ֽ�
	unsigned char status;                    // ״̬��0x01��ʾ����״̬��0x00��ʾ����״̬�������Ƿ���List�л���coder

	unsigned char keepBackSymbol;            // ������Դ�������жϵĽṹ��

	unsigned char* BytesArray;               // ����ʱ����InBytesArray
	
	// �������������ݱ�����������������ʹ�ü���
	WJL_FUNCTION_PARAMETERS* wfp;

}WJL_ERRRECOVERY_DECODER;

#ifdef	__cplusplus
extern "C" {
#endif
	/******************************************************************************************
	������뺯��
	******************************************************************************************/
	int WJLErrRecoveryEncoder(WJL_ERRRECOVERY_ENCODER* coder);

	/******************************************************************************************
	�������뺯��
	******************************************************************************************/
	int WJLErrRecoveryDecoder(WJL_ERRRECOVERY_DECODER* coder, WJL_ERRRECOVERY_DECODER** list);

#ifdef	__cplusplus
}
#endif
#endif
