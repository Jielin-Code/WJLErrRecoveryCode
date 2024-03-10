#pragma once
/******************************************************************************************
���ڽ���������㷨���ۣ������ڷ���������Դ��������
1�����ݡ�������ԭ��Ӧ�á��ķ�����������X�еķ���0�滻Ϊ101,�ҽ�����1�滻Ϊ01�������ʵĽǶ�����������һ�����ʲ������ģ����Ǿ�������ȴ��������
2������λ��ת�ľ���������Ч���������Ż���Ч��������֮ǰ�汾����˰������ϣ�
3�����֧��0dB�ľ���������CPU�ı�̣���δʵ�ֶ��̵߳ľ����������GPU��NPUЧ�������̶ȵ�������
4�������ֽ�Խ��������Խ�ߣ����۱�������Ϊ-1/log_2(1/3) = 0.630929��
5����ͨ�����ò���ʵ�ֲ�ͬ�̶ȵľ���START_LIMIT��END_LIMIT��COMPARE_LIMIT��ERRBITS_LIMIT��
6��DECODER_LIST_SIZE�������鲻Ҫ���õ�̫�ͣ�����С��START_LIMIT + END_LIMIT + 1

���ۣ���������ԭ��Ӧ�á�
���ߣ�������
���������ڼ�Ȩ����ģ�͵ľ����㷨����ȫ�µľ����㷨
ʱ�䣺20240310
�汾��5.2.0
BUG�޸�˵����
1���޸���ĩβ�ֽھ����ͷű�������
2�����ò��������滻����Ԥ����ָ�ʹ�ò��ֲ����ھ�����̶�̬���������㣬����0dB-1dB����һ�ײ�����1.01dB��2.0dB�ֲ���һ�ײ����ȵ�
3���޸��˾���ʧ���ͷ���Դ��������
******************************************************************************************/
#ifndef _WJLERRRECOVERYCORE_H
#define _WJLERRRECOVERYCORE_H

// �����ṹ�壬��Щֵ��Ӱ�����Ч���ĺ��Ĳ��������Ը��ݲ�ͬ���ŵ����ò�ͬ�Ĳ���
typedef struct
{
	// WJL_ERRRECOVERY_DECODER�����б��С��һ������Ϊ24����Ҫע��DECODER_LIST_SIZE�������(START_LIMIT + END_LIMIT + FIRST_ERR_COMPARE_LIMIT + 1) * 2
	int DECODER_LIST_SIZE;
	// ���������������̣����Զ���λ����λ�ã��������㡰ÿ������0��һ��������1�������Զ���ֹ������λ����λ�ú�
	// ͨ������START_LIMIT��END_LIMIT��ȷ�������ֽڵķ�Χ��Խ���������Խǿ��ͬʱ��Ҫ�����ʱ��ҲԽ��
	// ���ݽ����������START_LIMITȡֵ��ΧΪ5��12����Ӧ�ľ�������Ҳ��һ�������ֵΪ12���������۵ó����ǰ�����ΧΪ12���ֽ�
	// һ������Ϊ6��8��10��12
	int START_LIMIT;
	// Ϊ���Ͼ�����֤����ȷ���ʣ����ſ�һ������������������END_LIMIT�Ļ����ϴ���λ��COMPARE_LIMIT֮����˵���׸������Ѿ�����
	// ���Ը���ʵ������Ⱥ��ŵ�Ҫ�����þ���Χ
	// һ������Ϊ6,8,10
	int END_LIMIT;
	// ���ƴ�����صĸ���������START_LIMIT��END_LIMIT���ֽڷ�Χ�ڴ�����صĸ�������ʱ��֧��10���������������µĲ��Խ����Ҫ�����Ŀ�����ҲԽ��
	// �������㹻������£���ȫ���Բ��ò�����֤�ķ���ʵ�ֳɱ���Ч������������GPU��NPU������Ӳ����������ֻ������CPU�����µķ���
	// һ������Ϊ3��4��5��6��7��8��Ĭ������Ϊ4��5
	int ERRBITS_LIMIT;
	// �������ۣ����㷨���ڡ�AB����������������㷨������ѭ�������������Ȼ��MAXIMUM_TRAVERSAL_TIMES�����������ѡ�������ܵ�һ�����ݣ�Ҳ������һ������
	int MAXIMUM_TRAVERSAL_TIMES;

}PARAMETERS;

// ���ݽ���������㷨�������ж�������ÿ������0��һ������������1��������������������Ķ��Ǵ����
typedef enum
{
	KEEPBACK_NULL = 0,		    // ǰ���޷���
	KEEPBACK_ZERO,				// ǰ���з���0
	KEEPBACK_ONE,				// ǰ���з���1
	KEEPBACK_ONEZERO			// ǰ���з���1�ͷ���0
}KEEPBACK_SYMBOL;

// ������������ṹ��
typedef struct
{
	// ����
	PARAMETERS* par;               // �������󣬲�ͬ�Ĳ�����Ӧ��ͬ����������
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
	unsigned char abandon;           // ���ڱ����Щ�ֽ���Ҫ����

}WJL_ERRRECOVERY_ENCODER;

// ������Ϻ�Ҳ��Ҫ������
typedef struct
{
	// ����
	PARAMETERS* par;               // �������󣬲�ͬ�Ĳ�����Ӧ��ͬ����������
	unsigned char* InBytesArray;   // �����ֽڻ�������
	unsigned int InBytesLength;    // �����ֽڵ��ܳ���
	unsigned int InBytesIndex;     // InBytesArray���±�
	// ���
	unsigned char* OutBytesArray;  // ����ֽڻ�������
	unsigned int OutBytesLength;   // ����ֽڵ��ܳ���
	unsigned int OutBytesIndex;    // OutBytesArray���±�

	unsigned int Li;
	unsigned int Ri;
	unsigned int value;		        // ����ʱ��������������루���ն�ΪU��

	unsigned char mask;             // �ֽ���ֵ��8��������ϳ�һ���ֽ�
	unsigned char outbyte;          // ������ֽ�

	unsigned char full;             // ���ձ������룬����Ƿ�����һ���ֽ�
	unsigned char abandon;          // ���ڱ����Щ�ֽ���Ҫ����
	unsigned char status;           // ״̬��0x01��ʾ����״̬��0x00��ʾ����״̬����Ӱ�쵽������еĻ���

	KEEPBACK_SYMBOL keepBackSymbol; // ������Դ�������жϵĽṹ��

	unsigned char* BytesArray;          // ������δ������ֽڿ飬��DECODER_LIST_SIZE���ֽ�

}WJL_ERRRECOVERY_DECODER;

#ifdef	__cplusplus
extern "C" {
#endif
	/******************************************************************************************
	������뺯��
	******************************************************************************************/
	int WJLErrRecoveryEncoder(WJL_ERRRECOVERY_ENCODER* coder);

	/******************************************************************************************
	�������뺯����Ҳ�Ǻ��ĵľ������뺯��
	WJL_ERRRECOVERY_DECODER** list ��������DECODER_LIST_SIZE��coder
	int cumulativeZerosLimit ��ͨ����������0�ĸ������ж���ǰ�Ĵ����Ƿ���ɾ���
	******************************************************************************************/
	int WJLErrRecoveryDecoder(WJL_ERRRECOVERY_DECODER* coder, WJL_ERRRECOVERY_DECODER** list);
#ifdef	__cplusplus
}
#endif
#endif
