离散无记忆二进制信源序列X经无损变换使其具备错误校验条件，变换后X经加权概率算术编码具有动态码率和静态码率，动态码率相较于静态码率能及时响应信道变化，能有效提升传输性能，从而适应更复杂的通信环境。经证明当码长足够长时加权概率算术编码的译码错误概率为0且编码码率可达1，并当序列X中各符号均匀分布时传输速率可达到信道容量。在AWGN信道BPSK信号仿真实验中比较了各静态码率下的纠错性能，当静态码率为0.5且BLER为〖10〗^(-3)时加权概率算术编码相较Polar CA-SCL提升了0.37dB，BLER为10-5时提升0.48dB。实验代码https://github.com/Jielin-Code/WJLErrRecoveryCode。
1948 年，信息论创始人Shannon提出了著名的信道编码定理。70 多年来，构造逼近信道容量的编码是信道编码理论的中心目标，出现了LDPC码，Turbo码等优秀算法。2009 年，土耳其学者Arikan提出了极化码，首次证明信道容量渐近可达。6G主要性能指标，如时延为50~100 μs，峰值传输速率超100 Gbit/s，中断概率小于10-6，以及每立方米超100 台终端连接等。由此可见，未来6G 需要同时满足高可靠、低时延、高频谱效率、高密度、大容量的性能要求。为了应对这些艰巨挑战，能及时响应信道好坏变化[7-9]可达信道容量的编码算法是非常有竞争力的一种候选技术，其优势主要体现在以下几个方面：
自适应性能：这种编码方式能够根据信道的质量实时调整编码策略，以适应信道的变化。当信道质量良好时，它可以减少冗余信息的数量，提高传输效率；当信道质量变差时，它可以增加冗余信息的数量，以提高数据的可靠性。
鲁棒性：这种编码方式可以在信道质量不佳的情况下，仍然保持相对较高的数据传输可靠性，增强了系统的鲁棒性。
效率与可靠性平衡：及时响应信道变化的信道编码能够在效率和可靠性之间取得较好的平衡。


par->LIST_SIZE = 48;       // 设置译码器队列长度

par->CODE_LENGTH = 8;      // 同步字节后，设置为8可得

par->Q_LENGTH = 3;         // 最小值为1，末尾编码Q_LENGTH个0xFF，以满足末尾字节的纠错校验长度，末尾一定范围内的字节会出现相同值

par->START_LIMIT = 10;     // 5dB和6dB检验此处为8

par->END_LIMIT = 8;

par->ERRBITS_LIMIT = 10;   // 最大为10，最小为1

par->ERGODIC_MODEL = 0;    // 0表示从1比特翻转遍历到ERRBITS_LIMIT比特的翻转，1表示从ERRBITS_LIMIT比特翻转遍历到1比特的翻转

par->SynchronizationBytes = (unsigned char*)malloc(par->CODE_LENGTH * 10);

par->SynchronizationLength = 0;
