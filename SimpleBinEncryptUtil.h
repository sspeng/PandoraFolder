#include <cmath>
#include <string>
#include <memory.h>
#include <cstdio>
#include <cstdlib>

class SimpleBinEncryptUtil
{
private:
    int prime_array_[10000];

    int ComputeOverallValue(const std::string&);


public:
    int* GenerateRandSeq(int len, int seed_a, int seed_b);
    void Encrypt(const char* in_buf, char* out_buf, int len, std::string key, int pad_len = 0, int part_len = 1000, int swap_percentage = 20);
    void Decrypt(const char* in_buf, char* out_buf, int len, std::string key, int pad_len = 0, int part_len = 1000, int swap_percentage = 20);

    SimpleBinEncryptUtil();
    ~SimpleBinEncryptUtil();
};

