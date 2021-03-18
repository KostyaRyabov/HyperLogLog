#include <iostream>
#include <string>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

uint32_t pow_2_32 = 0xFFFFFFFF;

class HyperLogLog
{
public:
    float alpha_m;
    uint32_t k, k_comp, m;
    vector<uint8_t> M;

    HyperLogLog(float std_error) {
        k = ceil(log2(pow(1.01 / std_error, 2))), k_comp = 32 - k;
        m = pow(2, k);

        M = vector<uint8_t>(m, 0);

        
        alpha_m = m == 16 ? 0.673
            : m == 32 ? 0.697
            : m == 64 ? 0.709
            : 0.7213 / (1 + 1.079 / (float)m);
    }

    uint8_t rank(uint32_t hash, uint16_t max)
    {
        uint8_t r = 1;
        while ((hash & 1) == 0 && r <= max) {
            ++r;
            hash >>= 1;
        }
        return r;
    }

    void count(uint32_t hash)
    {
        uint16_t j = hash >> k_comp;
        M[j] = max(M[j], rank(hash, k_comp));
    }

    double count() {
        double c = 0.0;
        for (uint16_t i = 0; i < m; ++i)
            c += 1 / pow(2, M[i]);

        double E = alpha_m * m * m / c;

        if (E <= 5 / 2 * m)
        {
            double V = 0;
            for (uint16_t i = 0; i < m; ++i)
                if (M[i] == 0) ++V;
            if (V > 0)
                E = m * log(m / V);
        }
        else if (E > 1 / 30 * pow_2_32)
            E = (pow_2_32 * log(1 - E / pow_2_32)) * -1;

        return E;
    }
};

int main() {
    uint32_t req_count;
    string req;
    
    cin >> req_count;
    std::cin >> std::ws;
    
    HyperLogLog log_log = HyperLogLog(0.01);

    for (uint32_t i = 0; i < req_count; i++) {
        getline(cin, req);
        log_log.count(hash<string>{}(req));
    }
    

    cout << log_log.count();
}
