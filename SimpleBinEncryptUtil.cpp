
#include "SimpleBinEncryptUtil.h"

SimpleBinEncryptUtil::SimpleBinEncryptUtil()
{
    prime_array_[0] = 2;
    prime_array_[1] = 3;
    int dst, trial_v;
    bool ok;

    for(int i = 2; i < 10000; i++)
    {
        trial_v = prime_array_[i - 1] + 2;

        while(true)
        {
            dst = sqrt(trial_v + 1);
            ok = true;

            for(int j = 0; j < i; j++)
            {
                if(trial_v % prime_array_[j] == 0)
                {
                    ok = false;
                    break;
                }

                if(prime_array_[j] > dst)
                    break;
            }

            if(ok)
                break;

            trial_v += 2;
        }

        prime_array_[i] = trial_v;
    }
}

SimpleBinEncryptUtil::~SimpleBinEncryptUtil()
{
    
}

void SimpleBinEncryptUtil::Encrypt(const char* in_buf, char* out_buf, int len, std::string key, int pad_len, int part_len, int swap_percentage)
{
    char* tmp_in_buf = (char*)malloc(sizeof(char) * len);
    memcpy(tmp_in_buf, in_buf, sizeof(char) * len);

    int key_sum = ComputeOverallValue(key + "doge");
    int* rand_num = GenerateRandSeq(len * 3, key.size(), key_sum);
    int* pad_rand = GenerateRandSeq(pad_len, key.size(), key_sum);


    for(int i = 0; i < len; i++)
    {
        out_buf[i] = tmp_in_buf[i] + rand_num[i];
    }

    if(swap_percentage > 100)
        swap_percentage = 100;
    if(swap_percentage < 0)
        swap_percentage = 0;

    for(int i = 0; i < len * swap_percentage / 100; i++)
    {
        int idx1 = ((rand_num[len + 2 * i] % len) + len) % len;
        int idx2 = ((rand_num[len + 2 * i + 1] % len) + len) % len;
        char tmp = out_buf[idx1];
        out_buf[idx1] = out_buf[idx2];
        out_buf[idx2] = tmp;

        //cout<<"enc swap "<<idx1<<" "<<idx2<<endl;
    }


    for(int i = 0; i < pad_len; i++)
    {
        out_buf[i + len] = pad_rand[i];
    }

    free(tmp_in_buf);
    free(rand_num);
    free(pad_rand);
}

void SimpleBinEncryptUtil::Decrypt(const char* in_buf, char* out_buf, int len, std::string key, int pad_len, int part_len, int swap_percentage)
{
    char* tmp_in_buf = (char*)malloc(sizeof(char) * len);
    memcpy(tmp_in_buf, in_buf, sizeof(char) * len);

    int key_sum = ComputeOverallValue(key + "doge");
    int* rand_num = GenerateRandSeq(len * 3, key.size(), key_sum);
    int* pad_rand = GenerateRandSeq(pad_len, key.size(), key_sum);

    if(swap_percentage > 200)
        swap_percentage = 200;
    if(swap_percentage < 0)
        swap_percentage = 0;

    for(int i = len * swap_percentage / 100 - 1; i >= 0; i--)
    {
        int idx1 = ((rand_num[len + 2 * i] % len) + len) % len;
        int idx2 = ((rand_num[len + 2 * i + 1] % len) + len) % len;
        char tmp = tmp_in_buf[idx1];
        tmp_in_buf[idx1] = tmp_in_buf[idx2];
        tmp_in_buf[idx2] = tmp;

        //cout<<"dec swap "<<idx1<<" "<<idx2<<endl;
    }

    for(int i = 0; i < len; i++)
    {
        out_buf[i] = tmp_in_buf[i] - rand_num[i];
    }


    for(int i = 0; i < pad_len; i++)
    {
        out_buf[i + len] = pad_rand[i];
    }

    free(tmp_in_buf);
    if(rand_num) free(rand_num);
    if(pad_rand) free(pad_rand);
}

int SimpleBinEncryptUtil::ComputeOverallValue(const std::string& s)
{
    int overall_value = 0;

    for(int i = 0; i < s.size(); i++)
    {
        overall_value += s[i];
    }

    return overall_value;
}

//顺便一提，这里是生成不是真的非常随机的数字。
int* SimpleBinEncryptUtil::GenerateRandSeq(int len, int seed_a, int seed_b)
{
    if(len <= 0)
        return NULL;

    int* lv = (int*)malloc(sizeof(int) * len);

    int part = len / 1000;
    if (part * 1000 < len)
        part++;
    int jla = seed_a + seed_b + len;
    int jlb = seed_a + seed_b;
    int ovva = 2587936, ovvb = -32495853;

    int geer_v = 0;

    for (int p = 0; p < part; p++)
    {
        //jla: 0~8000
        //jlb: 1000~9000
        jla = (prime_array_[((ovva % 8000) + 8000) % 8000] + jla + ovvb) % 8000 + 8000; jla %= 8000;
        jlb = ((prime_array_[(((ovvb % 8000) + 8000) % 8000) + 1000] + jlb + ovva) % 8000) + 8000; jlb = jlb % 8000 + 1000;

        int cur = p * 1000;
        int dst = (p + 1) * 1000;
        int loop_len = 1000;
        if (dst > len)//1000 >= 1000, ok
        {
            loop_len -= dst - len;
        }

        int vec_part = loop_len / 16;
        int vec_remain_start = vec_part * 16;

        for (int i = 0; i < vec_remain_start; i += 16)
        {
            for (int iv = 0; iv < 16; iv++)//在第一次调用的时候，这里的值变化太慢了。
            {
                lv[i + cur + iv] = prime_array_[jla + iv] + prime_array_[jlb + iv] +
                    ovva * prime_array_[jlb + iv] +
                    ovvb * prime_array_[jla + iv];
            }

            ovva = ovva * lv[i + cur] + ovvb + prime_array_[jla] + prime_array_[geer_v];
            ovvb = ovvb * lv[i + cur] + ovva + prime_array_[jlb] + prime_array_[geer_v];

            jla += 16;
            jlb += 16;
        }

        int iv2 = 0;
        for (int i = vec_remain_start; i < loop_len; i++)
        {
            lv[i + cur] = prime_array_[jla + iv2] + prime_array_[jlb + iv2] +
                ovva * prime_array_[jlb + iv2] +
                ovvb * prime_array_[jla + iv2];

            iv2++;
        }

        geer_v += 17;
        geer_v %= 789;
        if(lv[loop_len - 1 + cur] % 2 == 0)
            geer_v += 2333;
        if(lv[loop_len - 1 + cur] > 0)
            geer_v += 4666;
    }

    return lv;
}
