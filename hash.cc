#include "anqi.hh"
#include <cmath>
#include <random>

using namespace std;

void initZobrist(){
    // unsigned int zob[15][32];
    unsigned int random_max = pow(2, 32)-1;
    random_device seed;
    default_random_engine e1(seed());
    uniform_int_distribution<unsigned int> dist(0, random_max);
    
    zob_player = dist(e1);
    for(int i=0; i<15; i++)
        for(int j=0; j<32; j++){
            unsigned int r = dist(e1);
            zob[i][j] = r;
        }
    
    hash_max_index = pow(2, 20);
    printf("create index = %u\n", hash_max_index);
    hash_table = (HASH_ENTRY*)malloc(hash_max_index*sizeof(HASH_ENTRY));
    for(int i=0; i<hash_max_index; i++){
        hash_table[i].used = false;
        hash_table[i].exact = false;
    }
}

void printZob(){
    printf("zob_player = %u\n", zob_player);
    for(int i=14; i<15; i++)
        for(int j=0; j<32; j++){
            printf("%d %d %u\n", i, j, zob[i][j]);
        }
    //for(int i=0; i<hash_max_index; i++)
    //    printf("%d exact = %d\n", i, hash_table[i].exact);
}
