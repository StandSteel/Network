//
// Created by shahnoor on 12/19/18.
//

#ifndef NETWORK_TEST_NETWORK_PERCOLATION_H
#define NETWORK_TEST_NETWORK_PERCOLATION_H

#include <iostream>
#include <algorithm>



void network_percolation(int argc, char* argv[]);
void  explosive_percolation_sum(
        uint m, uint network_size, uint M, uint ensemble_size, size_t th_id);
void network_percolation_explosive(int argc, char* argv[]);
void network_percolation_explosive_v2(int argc, char** argv);

void network_percolation_global();

void network_percolationReverse_global();

//void demarcationLine(int argc, char **argv); // for demarcation line
void BA_entropy_jump_ensemble(int argc, char **argv);

//void clusterSizeDistribution(int argc, char **argv);// added 2019.06.22


void test_NetworkBApercolationExplosive_v3(int argc, char **argv);
void test_NetworkBApercolationExplosive_v3_jump(int argc, char **argv);

void test_v5(int argc, char **argv);
void test_v3(int argc, char **argv);
void test_v6(int argc, char **argv); // 2019.10.20
void test_network_v6(int argc, char **argv); // 2019.10.20

void test_NetworkBA_v2(int argc, char **argv);


#endif //NETWORK_NETWORK_PERCOLATION_H
