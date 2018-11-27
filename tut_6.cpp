#include <iostream>
#include <string>
#include <vector>
#include <math.h>

class cache{
public:
    cache (uint8_t Lin, uint8_t Nin, uint8_t Kin){
        L = Lin;
        N = Nin;
        K = Kin;

        /* Create Cache Matrix */
        cache3D = new uint32_t**[N];                    // N Sets
        for (int i = 0; i < N; i++) {           
            cache3D[i] = new uint32_t*[K];              // K tags
            for (int j = 0; j < K; j++){
                cache3D[i][j] = new uint32_t[L/4];      // L/4 words
                for (int k = 0; k < L/4; k++){
                    cache3D[i][j][k] = 0;
                }
            }
        }

        /* Create Tag Matrix */
        tag_vals = new uint16_t*[N];
        for (int i = 0; i < N; i++){
            tag_vals[i] = new uint16_t[K];
            for (int j = 0; j < K; j++){
                tag_vals[i][j] = 0xFFFF;            // 0xFFFF Signals empty tag
            }                                       // This works as tags will never
        }                                           // be full 16 bits long normally

        /* Create N 2D real LRU matrix */
        real_LRU = new uint8_t**[N];
        for (int i = 0; i < N; i++){
            real_LRU[i] = new uint8_t*[K];
            for (int j = 0; j < K; j++){
                real_LRU[i][j] = new uint8_t[K];
                for (int k = 0; k < K; k++){
                    real_LRU[i][j][k] = 0;
                }
            }
        }
    }

    ~cache (){
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < K; j++){
                delete[] cache3D[i][j];
            }
            delete[] cache3D[i];
        }
        delete[] cache3D;

        for (int i = 0; i < N; i++){
            delete[] tag_vals[i];
        }
        delete[] tag_vals;

        for (int i = 0; i < N; i++){
            for (int j = 0; j < K; j++){
                delete[] real_LRU[i][j];
            }
            delete[] real_LRU[i];
        }
        delete[] real_LRU;
    }

    struct hit_info{
        uint32_t misses;
        uint32_t hits;
    };

    struct hit_info memory_requests (std::vector <uint16_t> mem_req){
        struct hit_info info;
        info.misses = 0;
        info.hits = 0;

        for (std::vector<uint16_t>::iterator it = mem_req.begin(); it != mem_req.end(); ++it){
            uint16_t offsetSize = std::log2(L);
            uint16_t offset = *it & (L-1);
            
            uint16_t setSize = std::log2 (N);
            uint16_t set = (*it >> offsetSize) & (N-1);

            uint16_t tagSize = 16 - setSize - offsetSize;
            uint16_t tag = (*it >> (offsetSize + setSize));
            
            bool hit = false;
            for (int i = 0; i < K; i++){
                if (tag_vals[set][i] == tag){
                    // HIT!
                    hit = true;
                    info.hits++;
                    LRU_addition (i,set);
                    break;
                }
            }
            if (!hit){
                // MISS!
                info.misses++;
                // Need To decide which Tag to replace
                tag_vals[set][LRU(set)] = tag;
            }
            hit = false;
        }
        return info;
    }

private:
    /* Cache Parametres */
    uint8_t L, N, K;                // Cache Shapping Parametres
    uint32_t*** cache3D;            // 3D Dynamic Array to store cache memory (Not used in this experiment)
    uint16_t** tag_vals;            // 2D Array to store tag values
    uint8_t*** real_LRU;            // N 2D Arrays to Calulate Real LRU

    void LRU_addition (uint8_t line, uint16_t set){
        for (int i = 0; i < K; i++){        // Set 1s in all values of row
            real_LRU[set][line][i] = 1;
        }
        for (int i = 0; i < K; i++){        // Set values in column to all zeros
            real_LRU[set][i][line] = 0;
        }
    }

    uint8_t LRU(uint16_t set){
        bool temp = false;
        for (int i = 0; i < K; i++){
            for (int j = 0; j < K; j++){
                if (real_LRU[set][i][j] == 0){
                    temp = true;
                }
                else{
                    temp = false;
                    break;
                }
            }
            if (temp) return i;
        }
    }
};

void test1 (std::vector<uint16_t> memory_inputs){
    std::cout << "-----------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "Beginning Test 1: 128 byte 1-way cache with 16 bytes per line (direct mapped)";
    std::cout << std::endl << std::endl;

    cache* myCache = new cache(16, 8, 1);
    struct cache::hit_info info = myCache->memory_requests(memory_inputs);

    std::cout << "Number of Cache Misses: " << info.misses << "\nNumber of Cache Hits: " << info.hits << std::endl;
    std::cout << std::endl << std::endl;

    delete myCache;
}

void test2 (std::vector<uint16_t> memory_inputs){
    std::cout << "-----------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "Beginning Test 2: 128 byte 2-way set associative cache with 16 bytes per line";
    std::cout << std::endl << std::endl;

    cache* myCache = new cache(16, 4, 2);
    struct cache::hit_info info = myCache->memory_requests(memory_inputs);

    std::cout << "Number of Cache Misses: " << info.misses << "\nNumber of Cache Hits: " << info.hits << std::endl;
    std::cout << std::endl << std::endl;

    delete myCache;
}

void test3 (std::vector<uint16_t> memory_inputs){
    std::cout << "-----------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "Beginning Test 3: 128 byte 4-way set associative cache with 16 bytes per line";
    std::cout << std::endl << std::endl;

    cache* myCache = new cache(16, 2, 4);
    struct cache::hit_info info = myCache->memory_requests(memory_inputs);

    std::cout << "Number of Cache Misses: " << info.misses << "\nNumber of Cache Hits: " << info.hits << std::endl;
    std::cout << std::endl << std::endl;
    delete myCache;
}

void test4 (std::vector<uint16_t> memory_inputs){
    std::cout << "-----------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "Beginning Test 4: 128 byte 8-way associative cache with 16 bytes per line (fully associative)";
    std::cout << std::endl << std::endl;

    cache* myCache = new cache(16, 1, 8);
    struct cache::hit_info info = myCache->memory_requests(memory_inputs);

    std::cout << "Number of Cache Misses: " << info.misses << "\nNumber of Cache Hits: " << info.hits << std::endl;
    std::cout << std::endl << std::endl;

    delete myCache;
}

int main(){
    std::vector<uint16_t> memory_inputs = {0x0000, 0x0004, 0x000c, 0x2200, 0x00d0, 0x00e0, 0x1130, 
                                    0x0028, 0x113c, 0x2204, 0x0010, 0x0020, 0x0004, 0x0040, 
                                    0x2208, 0x0008, 0x00a0, 0x0004, 0x1104, 0x0028, 0x000c, 
                                    0x0084, 0x000c, 0x3390, 0x00b0, 0x1100, 0x0028, 0x0064, 
                                    0x0070, 0x00d0, 0x0008, 0x3394};

    test1(memory_inputs);
    test2(memory_inputs);
    test3(memory_inputs);
    test4(memory_inputs);

    std::cout << "Exiting Program . . . ." << std::endl;
    return 0;
}