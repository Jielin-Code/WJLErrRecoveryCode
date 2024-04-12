  X is a discrete memoryless binary source , Sequence X undergoes lossless transformation to satisfy the requirement “Each 1 is separated by one or more zeros", "Each 0 is separated by one or two 1s" or similar condition , These conditions are the decision conditions for error detection in channel transmission. A new channel coding method is proposed based on a weighted probability model for lossless coding , The encoding rate and encoding and decoding steps of different conversion methods are different , It is proved that the decoding error probability can reach 0 when the code length is long enough. In the simulation experiment of BPSK signal in AWGN channel, At 0.5 bit rate, the proposed method improves 1.0dB over Polar code when FER is 0.001, and 1.1dB over Polar code when FER is 0.0001.

  Based on the theory of jielin code error correction algorithm and based on method 2:
1. According to scheme 2 of Principles and Application of Jielin Code, the symbol 0 in sequence X is replaced with 101, and the symbol 1 is replaced with 01. From the perspective of code rate, the code rate of scheme 1 is the largest, but the error correction ability is the weakest
2. The error correction scheme of bit flip is optimized from the efficiency, and the efficiency improvement is more than one million times compared with the previous version;
3, the maximum support of 0dB error correction, due to CPU programming, no error correction of multithreaded, if the efficiency based on GPU or NPU can be improved to the maximum extent;
4, the longer the input byte, the higher the code rate, the theoretical encode rate is-1 / log_2 (1 / 3) = 0.630929;
5. Different correction can be realized by setting parameters, START _ LIMIT, END _ LIMIT, COMPARE _ LIMIT and ERRBITS _ LIMIT;
6. The DECODER _ LIST _ SIZE parameter is recommended not to set too low to be less than START _ LIMIT + END _ LIMIT + 1

Theory: Principles and Application of jielin-Lin Code
Author: Wang Jielin
Description: The error correction algorithm based on the weighted probability model is a brand new error correction algorithm
Time: 20240410
Version: 6.0.0


par->LIST_SIZE = 48;       // 设置译码器队列长度

par->CODE_LENGTH = 8;      // 同步字节后，设置为8可得

par->Q_LENGTH = 3;         // 最小值为1，末尾编码Q_LENGTH个0xFF，以满足末尾字节的纠错校验长度，末尾一定范围内的字节会出现相同值

par->START_LIMIT = 10;     // 5dB和6dB检验此处为8

par->END_LIMIT = 8;

par->ERRBITS_LIMIT = 10;   // 最大为10，最小为1

par->ERGODIC_MODEL = 0;    // 0表示从1比特翻转遍历到ERRBITS_LIMIT比特的翻转，1表示从ERRBITS_LIMIT比特翻转遍历到1比特的翻转

par->SynchronizationBytes = (unsigned char*)malloc(par->CODE_LENGTH * 10);

par->SynchronizationLength = 0;
