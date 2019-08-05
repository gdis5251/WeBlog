#include <iostream>
#include <sstream>
#include <fstream>

#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include <fcntl.h>
#include <unistd.h>
#include <iomanip>

//512bit = 64Byte
//64Byte = 16 * 4 Byte
static const int N = 64;


class MD5 {
public:

    ~MD5() {
        delete[](block);
    }

    void StrMD5(const char *s) {
        str = s;
        A = 0x67452301;
        B = 0xefcdab89;
        C = 0x98badcfe;
        D = 0x10325476;
        CalculateStr();
    }

    void FileMD5(const char *p) {
        path = p;
        A = 0x67452301;
        B = 0xefcdab89;
        C = 0x98badcfe;
        D = 0x10325476;
        CalculateFile();
    }

    //输出是要注意大小端
    std::string getMD5() {
        std::stringstream ss;
#if 1
        uint64_t buf[4] = {A, B, C, D};
        for (int i = 0; i < 4; ++i) {
            unsigned char *tmp = (unsigned char *) &buf[i];
            for (int j = 0; j < 4; ++j) {
                ss << std::setfill('0') << std::setw(2) << std::hex << (int) tmp[j];
            }
        }
#endif
        std::string res = ss.str();
        return res;
    }

private:
    //todo:使用stringstream 和 fstream 合并str和file的计算函数
    void CalculateStr() {
        if (block != nullptr) {
            delete[] (block);
            block = nullptr;
        }
        unsigned long length = 0;
        while (1) {
            int len = readstr(str);
            if (len == 64) {
                length += len;
                str += len;
                calculateChunk((uint32_t *) chunk);
            } else {
                length += len;
                file_length = length * 8;
                unsigned char *rep = fill(len);
                calculateChunk((uint32_t *) chunk);
                if (rep) {
                    calculateChunk((uint32_t *) chunk);
                }
                break;
            }
        }
    }

    void CalculateFile() {
        if (block != nullptr) {
            delete[](block);
            block = nullptr;
        }
        unsigned long length = 0;
        std::fstream fs;
        fs.open(path, std::fstream::binary | std::fstream::in);
        while (1) {
            fs.read((char *) chunk, 64);
            // 如果可以读满 64 个字节说明不是最后一块
            // 不需要填充
            if (fs.gcount() == 64) {
                length += fs.gcount();
                calculateChunk((uint32_t *) chunk);
            } else { // 若读不到 64 个字节，说明已经是最后一块了, 对其进行填充
                length += fs.gcount();
                file_length = length * 8;
                unsigned char *rep = fill(fs.gcount());
                calculateChunk((uint32_t *) chunk);
                if (rep) {
                    calculateChunk((uint32_t *) rep);
                }
                break;
            }
        }
    }


    //填充最后的一个64字节块
    unsigned char *fill(int n) {
        // 若当前已经 大于等于56 字节了，则需要填充完当前字符串之后
        // 在需要重新生成一个字符串并计算
        if (n >= 56) {  
            int len = 64 - n;
            chunk[n] = 0x80;//补一个1其他用0填充
            for (int i = n + 1; i < 64; ++i) {
                chunk[i] = 0x0;
            }
            block = new unsigned char[64]();
            unsigned char *slide = block + 56;
            // 最后 8 个字节存放文件的长度
            (*((uint64_t *) slide)) = file_length; 
        } else {   //小于488补到488
            chunk[n] = 0x80;
            for (int i = n + 1; i < 57; ++i) {
                chunk[i] = 0x0;
            }
            block = nullptr;
            unsigned char *slide = chunk + 56;
            (*((uint64_t *) slide)) = file_length; //填充
        }
        return block;
    }

    //对每块进行64次计算
    void calculateChunk(uint32_t *chk) {  // 512
        uint32_t a = A, b = B, c = C, d = D;
        uint32_t logic, g, tmp;

        for (uint32_t i = 0; i < 64; ++i) {
            if (i >= 0 && i < 16) {  // a b c d循环左移16次 用F
                logic = F(b, c, d);
                g = i;
            } else if (i >= 16 && i < 32) {     // 用G
                logic = G(b, c, d);
                g = (5 * i + 1) % 16;
            } else if (i >= 32 && i < 48) {
                logic = H(b, c, d);
                g = (3 * i + 5) % 16;
            } else {
                logic = I(b, c, d);
                g = (7 * i) % 16;
            }

            tmp = d;
            d = c;
            c = b;
            b = b + shifting(a + logic + chk[g] + k[i], s[i]);
            a = tmp;
        }

        A += a;
        B += b;
        C += c;
        D += d;
    }

    uint32_t F(uint32_t x, uint32_t y, uint32_t z) {
        return (x & y) | ((~x) & z);
    }

    uint32_t G(uint32_t x, uint32_t y, uint32_t z) {
        return (x & z) | (y & (~z));
    }

    uint32_t H(uint32_t x, uint32_t y, uint32_t z) {
        return x ^ y ^ z;
    }

    uint32_t I(uint32_t x, uint32_t y, uint32_t z) {
        return y ^ (x | (~z));
    }

    uint32_t shifting(uint32_t i, int n) {
        return (i << n) ^ (i >> (32 - n));
    }

    int readstr(const char *str) {
        int len = 0;
        for (int i = 0; i < 64; ++i) {
            if (str[i] == 0) {
                break;
            }
            chunk[i] = (unsigned char) str[i];
            ++len;
        }
        return len;
    }

private:
    const char *path = nullptr;
    const char *str = nullptr;

    uint32_t A = 0x67452301;
    uint32_t B = 0xefcdab89;
    uint32_t C = 0x98badcfe;
    uint32_t D = 0x10325476;


    uint32_t k[N] = {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
                     0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
                     0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
                     0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
                     0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
                     0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
                     0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
                     0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
                     0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
                     0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
                     0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
                     0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
                     0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
                     0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
                     0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
                     0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};
    //用来计算循环左移多少位
    uint32_t s[N] = {7, 12, 17, 22, 7, 12, 17, 22,
                     7, 12, 17, 22, 7, 12, 17, 22,
                     5, 9, 14, 20, 5, 9, 14, 20,
                     5, 9, 14, 20, 5, 9, 14, 20,
                     4, 11, 16, 23, 4, 11, 16, 23,
                     4, 11, 16, 23, 4, 11, 16, 23,
                     6, 10, 15, 21, 6, 10, 15, 21,
                     6, 10, 15, 21, 6, 10, 15, 21};

    //64字节的块计算hash
    unsigned char chunk[64] = {0};        //每次计算的块大小
    unsigned char *block = nullptr;  //最后一块填充的额外部分
    uint64_t file_length = 0;   //文件bit长度
};


int help() {
    std::cout << "参数错误!" << std::endl;
    std::cout << "-s 后面加要计算md5的字符串数量不限" << std::endl;
    std::cout << "./md5 -s str1 str2 ....." << std::endl;
    std::cout << "-f 后面加要计算md5的文件名数量不限" << std::endl;
    std::cout << "./md5 -f filename1 filename2 ..." << std::endl;
    return 1;
}

