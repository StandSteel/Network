//
// Created by shahnoor on 10/20/19.
//

#include <iostream>
#include <chrono>
#include <climits>
#include "network_percolation_explosive_v6.h"


using namespace std;


/**
 *
 * @param m0
 * @param m
 * @param size
 * @param M
 */
NetworkBApercolationExplosive_v6::NetworkBApercolationExplosive_v6(size_t m0, size_t m, size_t size, uint M)
        : NetworkBApercolation_v6(m0, m, size)
{
    std::cout << "class NetworkBApercolationExplosive_v6" << std::endl;
    if (M > 2) {
        if(M > MAX_M_VALUE){
            cout << "M exceeds maximum allowed value : line " << __LINE__ << endl;
            _M_link = MAX_M_VALUE;
        }
        _M_link = M;
    }
//    M_sum_products.resize(_M_link);
//    M_link_indices.resize(_M_link);
}


/**
 *
 * @param rule : specify sum rule or product rule
 *               values : 's' for sum rule and 'p' for product rule
 * @return
 */
uint NetworkBApercolationExplosive_v6::selectLink_v2() {
    if(occupied_link_count >= _link_count){
        return _link_count+1;
    }
    auto start = std::chrono::system_clock::now(); // time measurement

    uint pos = link_for_min_cluster_sum_product_v2(occupied_link_count);
    auto end = std::chrono::system_clock::now(); // time measurement
    std::chrono::duration<double> elapsed_seconds = end-start; // time measurement
    _time_selectLink += elapsed_seconds.count();
    return pos;
}


/**
 * For minimizing cluster sizes using sum rule
 * version 1:
 *  (a) this function starts to read _M values of _randomized_indices array starting at start_at
 *  (b) both sum rule and product rule is present here but one of them is commented
 *
 *  product of two cluster, each of a million in size, can be very large. for example : 10^6 * 10^6 = 10^12
 *  so use size_t variable type for storing products
 *
 * TODO The link with the smaller value of the products is occupied; in case of products being equal one of the corresponding links is selected randomly.
 *
 * @return value of the link in the _randomized_indices for which cluster size become minimum
 */
uint NetworkBApercolationExplosive_v6::link_for_min_cluster_sum_product_v2(size_t start_at) {
//    auto start = std::chrono::system_clock::now(); // time measurement
    size_t index_randomized_link{0};

    uint tmp_lnk_index;
    int id1{-1}, id2{-1}, root1, root2;
    long n_nodes, prod_sum = LONG_MAX; // so that it is very big before going into the loop
//    cout << LONG_MAX << " vs " << prod_sum << endl;
//    cout << ULONG_MAX << " vs " << prod_sum << endl;
//    size_t limit = start_at + _M_link;
    size_t r{};
//    cout << "randomly between ("<< start_at <<"," << _link_count << ")={";

    for(size_t i{0}; i < _M_link; ++i){


        // >>>>>> START A takes ~24 % of total runtime
        // random number between a and b or rand(a,b) = a + r%(b-a); where r is an arbitrary random number
        // TODO : must select radnomly with uniform probability
        // use std::uniform_int_distribution<int> distribution(0,9);
        r = start_at + _random_generator() % (_link_count-start_at);
        tmp_lnk_index = _randomized_indices[r];
        // <<<<< END A


        // >>>>>> START B takes ~26 % of total runtime
        id1 = _network_map_A[tmp_lnk_index];
        id2 = _network_map_B[tmp_lnk_index];
//        id1 = getNodeA(tmp_lnk_index);
//        id2 = getNodeB(tmp_lnk_index);
//        _network_frame.fromNetworkMapAB(id1, id2, tmp_lnk_index); // slower a bit
        // <<<<< END B

        root1 = findRoot(id1);
        root2 = findRoot(id2);

        n_nodes = long(_cluster_info[root1]) * long(_cluster_info[root2]); // product rule. automatically becomes positive
//            n_nodes = abs(_cluster_info[root1] + _cluster_info[root2]); // sum rule
#ifdef UNIT_TEST
        if(n_nodes < 0){
            cerr << "product cannot be negative " << endl
            << "NetworkPercolationExplosive_v7::link_for_min_cluster_sum_product: line "
            << __LINE__ << endl;
            cout << "possible solution : cast value from int to long in order to disolve this" << endl;
            exit(0);
        }
#endif
        if(n_nodes < prod_sum ) { // since we are minimizing cluster sizes
            prod_sum = n_nodes;
            index_randomized_link = r;
        }
#ifdef DEBUG_FLAG
        cout << "checking link _randomized_indices[" << r << "]=" << tmp_lnk_index << endl;
        cout << " out of  id = " << id1 << " and " << id2 << " prod_sum = " << prod_sum << endl;
#endif
    }

    if(index_randomized_link >= _randomized_indices.size()){
        cout << "out of bound : line " << __LINE__ << endl;
    }
//    cout << "}" << endl;
#ifdef DEBUG_FLAG
    cout << "selected _randomized_indices[" << index_randomized_link << "] = "
         << _randomized_indices[index_randomized_link] << endl;
#endif

    size_t pos = _randomized_indices[index_randomized_link];
    // must assign to a temp variable, otherwise it is replaced before retrurn
    // must erase the used value. // but erasing takes more than 90% of total time
//    _randomized_link_indices.erase(_randomized_link_indices.begin() + r);
    // instead of erasing the index, try swapping it with _number_of_occupied_links index\

    // 2019.05.20 better idea is just to replace the used value of _randomized_link_indices
    // by the first unused value, i.e., value at _number_of_occupied_links

    _randomized_indices[index_randomized_link] = _randomized_indices[occupied_link_count];

    _randomized_indices[occupied_link_count] = 0; // does not affect the result. use only when debugging so that we can identify easily
//    auto end = std::chrono::system_clock::now(); // time measurement
//    std::chrono::duration<double> elapsed_seconds = end-start; // time measurement
//    _time_selectLink += elapsed_seconds.count(); // time measurement
    return pos;
}

/**
 * For minimizing cluster sizes using sum rule
 * version 3:
 *  (a) version 1 is implied
 *  (b) before tc we do explosive (join two cluster for which sum or product is minimum)
 *      and after tc we do inverted explosive (join two cluster for which sum or product is maximum).
 *
 *  product of two cluster, each of a million in size, can be very large. for example : 10^6 * 10^6 = 10^12
 *  so use size_t variable type for storing products
 *
 * @return value of the link in the _randomized_indices for which cluster size become minimum
 */
uint NetworkBApercolationExplosive_v6::link_for_min_cluster_sum_product_v3_adaptive(double tc, size_t start_at) {
    size_t index_randomized_link{0};
    double t = relativeLinkDensity();
    int tmp_lnk_index, root1, root2;
    int id1{-1}, id2{-1};
    long n_nodes, prod_sum = LONG_MAX; // so that it is very big before going into the loop
//    cout << LONG_MAX << " vs " << prod_sum << endl;
    if (t < tc) {
        prod_sum = LONG_MAX; // so that it is very big before going into the loop
    }else{
        prod_sum = 1; // smallest possible value for both sum and product
    }
//    cout << "prod_sum " << prod_sum << endl;
//    size_t limit = start_at + _M_link;
    size_t r{};
//    cout << "randomly between ("<< start_at <<"," << _link_count << ")={";
    for(size_t i{0}; i < _M_link; ++i){
        // what happens when start_at is near to the size of _randomized_link_indices. ???
        // random number between a and b or rand(a,b) = a + r%(b-a); where r is an arbitrary random number
        r = start_at + _random_generator() % (_link_count-start_at);

        tmp_lnk_index = _randomized_indices[r];
//        cout << r << ":" << tmp_lnk_index << ",";

        id1 = _network_map_A[tmp_lnk_index];
        id2 = _network_map_B[tmp_lnk_index];

        root1 = findRoot(id1);
        root2 = findRoot(id2);
        n_nodes = long(_cluster_info[root1]) * long(_cluster_info[root2]); // product rule. automatically becomes positive
//            n_nodes = abs(_cluster_info[root1] + _cluster_info[root2]); // sum rule
#ifdef UNIT_TEST
        if(n_nodes < 0){
            cerr << "product cannot be negative " << endl
            << "NetworkPercolationExplosive_v7::link_for_min_cluster_sum_product: line "
            << __LINE__ << endl;
            cout << "possible solution : cast value from int to long in order to disolve this" << endl;
            exit(0);
        }
#endif
        if(t < tc){
            if(n_nodes < prod_sum){
                prod_sum = n_nodes;
                index_randomized_link = r;
            }
        }else{
            if(n_nodes > prod_sum){
                prod_sum = n_nodes;
                index_randomized_link = r;
            }
        }


//        cout << "checking link " << _network_frame.getLink(_randomized_link_indices[i])
//             << " id = " << id1 << " and " << id2 << " prod_sum= " << prod_sum << endl;
    }
    if(index_randomized_link >= _randomized_indices.size()){
        cout << "out of bound : line " << __LINE__ << endl;
    }
//    cout << "}" << endl;
//    cout << "selected link " << _network_frame.getLink(_randomized_link_indices[index_randomized_link])
//         << " id = " << id1 << " and " << id2 << " prod_sum= " << prod_sum << endl;
//    cout << " at " << index_randomized_link  << " value " << _randomized_link_indices[index_randomized_link] << endl;

    size_t pos = _randomized_indices[index_randomized_link];
    // must assign to a temp variable, otherwise it is replaced before retrurn
    // must erase the used value. // but erasing takes more than 90% of total time
//    _randomized_link_indices.erase(_randomized_link_indices.begin() + r);
    // instead of erasing the index, try swapping it with _number_of_occupied_links index\

    // 2019.05.20 better idea is just to replace the used value of _randomized_link_indices
    // by the first unused value, i.e., value at _number_of_occupied_links

    _randomized_indices[index_randomized_link] = _randomized_indices[occupied_link_count];

    _randomized_indices[occupied_link_count] = 0; // does not affect the result. use only when debugging so that we can identify easily

    return pos;
//    return 0;
}




bool NetworkBApercolationExplosive_v6::occupyLink() {
    if (occupied_link_count >= getLinkCount()){
        return false;
    }
//    uint pos = selectLink_v2();
    uint pos = link_for_min_cluster_sum_product_v2(occupied_link_count);
    return NetworkBApercolation_v6::placeSelectedLink(pos);
}

void NetworkBApercolationExplosive_v6::summary() {
    NetworkBApercolation_v6::summary();
    cout << "_time_selectLink " << _time_selectLink << " sec" << endl;
//    cout << "_time_link_for_min_cluster_sum_rule " << _time_link_for_min_cluster_sum_rule << " sec" << endl;
//    cout << "_count_link_for_min_cluster_sum_rule_a " << _count_link_for_min_cluster_sum_rule_a << " times" << endl;
//    cout << "_count_link_for_min_cluster_sum_rule_b " << _count_link_for_min_cluster_sum_rule_b << " times" << endl;
//    cout << "_count_link_for_min_cluster_sum_rule_c " << _count_link_for_min_cluster_sum_rule_c << " times" << endl;


}

void NetworkBApercolationExplosive_v6::reset(int i) {
    _time_selectLink=0;
    _time_link_for_min_cluster_sum_rule=0;
    NetworkBApercolation_v6::reset(i);
}
/**
 * swap a-th and b-th element of _randomized_indices
 * @param a
 * @param b
 */
void NetworkBApercolationExplosive_v6::swap_randomized_index(size_t a, size_t b) {
    uint tmp = _randomized_indices[a];
    _randomized_indices[a] = _randomized_indices[b];
    _randomized_indices[b] = tmp;
}
/**
 * assign b-th element of _randomized_indices to a-th element and set b-th element to zero
 * @param a
 * @param b
 */
void NetworkBApercolationExplosive_v6::replace_randomized_index(size_t a, size_t b) {
    _randomized_indices[a] = _randomized_indices[b];
    _randomized_indices[b] = 0; // does not affect the result. use only when debugging so that we can identify easily
}


