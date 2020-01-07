//
// Created by shahnoor on 11/2/19.
//

#include "test_v7.h"



#include <chrono>
#include <fstream>
#include "../percolation/network_percolation_v7.h"
#include "../util/time_tracking.h"
#include "../nets/network.h"
#include "../nets/ER/network_ER.h"
#include "../nets/BA/network_BA_v7.h"
#include "../nets/MDA/network_MDA_v7.h"



using namespace std;


void test_v7(int argc, char **argv) {
//    test_ER();
//    test_ER_Percolation();
//    test_ER_PercolationExplosive();
//    run_ER_percolation(argc, argv);
//    test_BA(argc, argv);
//    test_MDA(argc, argv);
//    test_percolation(argc, argv);


    run_v7_percolation(argc, argv);
//    run_v7_percolation_jump(argc, argv);
//    run_v7_percolation_jump_avg(argc, argv);

//    run_v7_percolation_near_tc(argc, argv);
}

void test_MDA(int argc, char *argv[]) {
    if(argc < 5 ){
        cout << "argv[1] == m" << endl;
        cout << "argv[2] == N" << endl;
        cout << "argv[3] == M" << endl;
        cout << "argv[4] == Ensemble" << endl;
        return;
    }
    int m = atoi(argv[1]);
    int N = atoi(argv[2]);
    int M = atoi(argv[3]);

    int En = atoi(argv[4]);
    cout << "m=" << m << ",N="<< N << ",M=" << M << ",En="<<En << endl;


    auto* net = new NetworkMDA_v7(m);
    net->setRandomState(0, true);
    net->initialize(N);
    vector<size_t> counts;
    for(int e{1}; e <= En; ++e){
        auto t0 = std::chrono::system_clock::now();
        net->rebuild();
        auto dd = net->degreeCounts();
        if(counts.size() < dd.size()){
            counts.resize(dd.size());
        }
        for(size_t i{}; i < dd.size(); ++i){
//        if(dd[i] == 0) continue;
//            cout << "k= " << i << " count=" << dd[i] << endl;
            counts[i] += dd[i];
        }
        auto t1 = std::chrono::system_clock::now();
        std::chrono::duration<double> drtion = t1 - t0;
        cout << "Iteration " << e << " : time " << drtion.count() << " sec" << endl;
    }
//    net->view();
//    net->viewAdjacencyList();
//    net->viewLocal();
    cout << "Done" << endl;
//    auto dd = net->degreeCounts();
//    for(size_t i{}; i < dd.size(); ++i){
//        if(dd[i] == 0) continue;
//        cout << "k= " << i << " count=" << dd[i] << endl;
//    }

    auto arr  = net->get_signature_array();
    string filename = net->getClassName() +  arr[1] + "degree-distribution" + currentTime() + ".txt";
    ofstream fout(filename);
    fout << "#{\"m0\":" << net->get_m0()
         << ",\"m\":" << net->get_m()
         << ",\"N\":" << N
         << ",\"En\":" << En
         << ",\"type\":\"" << arr[0] << "\""
         << "}" << endl;
    fout << "#k = degree" << endl;
    fout << "#P(k) = repetition of k on average" << endl;
    fout << "# from P(k) to get degree distribution divide it by maximum degree" << endl;
    fout << "#<k>,<P(k)>" << endl;
    for (size_t i{1}; i < counts.size(); ++i) {
        if(counts[i] == 0) continue;
        fout << i << "\t" << double(counts[i])/En << endl;
    }

    // normalization
    fout.close();
}


void test_ER() {
    Network_v7* net = new NetworkER_v7(0.8);
    net->setRandomState(0, true);
    net->initialize(100);
    net->view();
    net->viewLocal();

}


void test_ER_Percolation() {
    NetworkER_v7* net=new NetworkER_v7(0.4);
    net->setRandomState(0, true);
    net->initialize(10);
//    net->view();
//    net->viewLocal();
    net->viewAdjacencyList();

    NetworkPercolation_v7 percolation(net);
    percolation.initialize();
    percolation.viewClusters();
    while(percolation.occupyLink()){
//        percolation.viewClusters();
//        cout << percolation.entropy_v1() << " , " << percolation.entropy_v2() << endl;
        cout << percolation.largestClusterSize() << endl;
    }
    percolation.viewClusters();

}

void test_ER_PercolationExplosive() {
    auto * net = new NetworkER_v7(0.4);
    net->setRandomState(0, false);
    net->initialize(10);
//    net->view();
//    net->viewLocal();
    net->viewAdjacencyList();

    NetworkPercolationExplosive_v7 percolation(net, 2);
//    NetworkPercolationInverted_v7 percolation(net, 2);

    percolation.initialize();
    percolation.viewClusters();
    while(percolation.occupyLink()){
//        percolation.viewClusters();
//        cout << percolation.entropy_v1() << " , " << percolation.entropy_v2() << endl;
        cout << percolation.largestClusterSize() << endl;
    }
    percolation.viewClusters();

}

void run_ER_percolation(int argc, char **argv){
    if(argc < 5 ){
        cout << "argv[1] == N" << endl;
        cout << "argv[2] == p" << endl;
        cout << "argv[3] == M" << endl;
        cout << "argv[4] == Ensemble" << endl;
        return;
    }
    int N = atoi(argv[1]);
    double p = atof(argv[2]);
    int M = atoi(argv[3]);
    int ensemble_size = atoi(argv[4]);
    cout << "N="<< N << ",p=" << p << ",M=" << M << ",En="<<ensemble_size << endl;

    auto * net=new NetworkER_v7(p);
    net->setRandomState(0, true);
    net->initialize(N);

//    NetworkPercolation_v7 percolation(net);
//    NetworkPercolationInverted_v7 percolation(net, M);
    NetworkPercolationExplosive_v7 percolation(net, M);

    percolation.initialize();
    size_t linkCount = net->getLinkCount();
    size_t nodeCount = net->getNodeCount();
    cout << nodeCount << ", " << linkCount << ", " << endl;
//    net.viewNetwork();
//    net.viewListOfLinkIndices();
    double entropy_jump{}, order_jump{};
    vector<double> entropy(linkCount), order_param(linkCount); // entropy and order parameter
    for (int k{0}; k < ensemble_size; ++k) {
        auto t_start= chrono::_V2::system_clock::now();
//        net.viewListOfLinkIndices();
        percolation.reset(0); // every 25 step. reset the network
//        cout << net->getLinkCount() << endl;
        size_t i{};
//        net.viewClusters();
//        net.viewListOfLinkIndices();
//        net.viewNetwork();
//        cout << "entering to while" << endl;
        while (percolation.occupyLink()) {
//            cout << "i " << i  << endl;
            entropy[i] += percolation.entropy();
            order_param[i] += percolation.largestClusterSize();
//            net.viewClusters();
//            cout << net.entropy_v1()  << "\t";
//            cout << net.entropy_v2() << endl;
//            cout << net.largestClusterSize() << endl;
            percolation.jump();
//            _network_frame.viewClusterExtended();
            ++i;
            if (i >= linkCount) {
                cout << "breaking at " << i <<endl;
                break;
            }
        }
        entropy_jump += percolation.largestEntropyJump();
        order_jump += percolation.largestOrderJump();

//        cout << entropy_jump[k] << " at " << entropy_jump_pc[k] << endl;
        auto t_end= chrono::_V2::system_clock::now();
        chrono::duration<double> drtion = t_end - t_start;
        cout << "iteration " << k << " : time elapsed " << drtion.count() << " sec" << endl;
    }

    auto tm = currentTime();
//    string signature = percolation.get_signature();
    string signature = net->get_signature_array()[0] +"_"+ percolation.getClassName()  + "_N_" + to_string(N) + "_p_";
    std::ostringstream out;
    out.precision(3);
    out << std::fixed << p;
    signature +=  out.str() + "_M_" + to_string(M);
    stringstream ss;
    ss << "{"
       << R"*("signature":")*" << signature << "\""
       << R"*(,"network_class":")*" << net->getClassName() << "\""
       << R"*(,"percolation_class":")*" << percolation.getClassName() << "\""
       << R"*(,"p":)*" << p
       << R"*(,"N":)*" << N
       << R"*(,"number_of_links":)*" << linkCount
       << R"*(,"number_of_nodes":)*" << nodeCount
       << R"*(,"M":)*" << M
       << R"*(,"ensemble_size":)*" << ensemble_size
       << R"*(,"date":")*" << tm << "\""
       << "}";



    string filename = signature + "_entropy-order_" + tm + ".txt";
    ofstream fout(filename);
    fout << '#' << ss.str() << endl;
    fout << "# t=relative link density" << endl;
    fout << "#<t>\t<H>\t<P>" << endl;
    for(size_t k{}; k < entropy.size() ; ++k){
        auto t = (k+1)/double(N);
        if (t > 5) {
            cout << "breaking at t=" << t << endl;
            break;
        }
        fout << t
             << "\t" << entropy[k]/ensemble_size
             << "\t" << order_param[k]/(ensemble_size*double(N)) << endl;
    }
    fout.close();
}





void test_percolation(int argc, char **argv) {
    if(argc < 5 ){
        cout << "argv[1] == m" << endl;
        cout << "argv[2] == N" << endl;
        cout << "argv[3] == M" << endl;
        cout << "argv[4] == Ensemble" << endl;
        return;
    }
    int m = atoi(argv[1]);
    int N = atoi(argv[2]);
    int M = atoi(argv[3]);

    int En = atoi(argv[4]);
    cout << "m=" << m << ",N="<< N << ",M=" << M << ",En="<<En << endl;


    auto* net = new NetworkBA_v7(m);
    net->setRandomState(0, true);
    net->initialize(N);
    net->clearAdjacency();
//    net->view();
//    net->viewAdjacencyList();
//    net->viewLocal();
//    cout << "Done" << endl;
//    auto dd = net->degreeDistribution();
//    for(size_t i{}; i < dd.size(); ++i){
//        cout << "k= " << i << " count=" << dd[i] << endl;
//    }

//
//      NetworkPercolation_v7 percolation(net);
//    NetworkPercolationInverted_v7 percolation(net, M);
    NetworkPercolationExplosive_v7 percolation(net, M);

    percolation.initialize();
    size_t linkCount = net->getLinkCount();
    size_t nodeCount = net->getNodeCount();
    cout << nodeCount << ", " << linkCount << ", " << endl;
    size_t limit = nodeCount * 2;
//    percolation.viewNetwork();
//    percolation.viewListOfLinkIndices();
    double entropy_jump{}, order_jump{};
    vector<double> entropy(linkCount), order_param(linkCount); // entropy and order parameter
    for (int k{1}; k <= En; ++k) {
        auto t_start= chrono::_V2::system_clock::now();
//        net.viewListOfLinkIndices();
        percolation.reset(k % 25 == 0); // every 100 step. reset the network
//        cout << net->getLinkCount() << endl;
        size_t i{};
//        percolation.viewClusters();
//        percolation.viewListOfLinkIndices();
//        percolation.viewNetwork();
//        cout << "entering to while" << endl;
        while (percolation.occupyLink()) {
//            cout << "i " << i  << endl;
            entropy[i] += percolation.entropy();
            order_param[i] += percolation.largestClusterSize();
            percolation.jump();
//            percolation.viewClusters();
//            percolation.viewListOfLinkIndices();
//            double H1 = percolation.entropy_v1();
//            double H2 = percolation.entropy_v2();
//            cout << H1 << "\t";
//            cout << H2 << "\t";
//            cout << percolation.largestClusterSize() << endl;

//            _network_frame.viewClusterExtended();
//            if(abs(H1 - H2) > 1e-5){
//                cerr << "problem : line " << __LINE__ << endl;
//            }

            ++i;
            if (i >= limit) {
                cout << "breaking at " << i <<endl;
                break;
            }
        }
        entropy_jump += percolation.largestEntropyJump();
        order_jump += percolation.largestOrderJump();

//        cout << entropy_jump[k] << " at " << entropy_jump_pc[k] << endl;
        auto t_end= chrono::_V2::system_clock::now();
        chrono::duration<double> drtion = t_end - t_start;
        cout << "iteration " << k << " : time elapsed " << drtion.count() << " sec" << endl;
    }

    auto tm = currentTime();
//    string signature = percolation.get_signature();
    string signature = net->get_signature_array()[0] +"_"+ percolation.getClassName()
                       + "_N_" + to_string(N)
                       + "_m_" + to_string(m)
                       + "_M_" + to_string(M);
    stringstream ss;
    ss << "{"
       << R"*("signature":")*" << signature << "\""
       << R"*(,"network_class":")*" << net->getClassName() << "\""
       << R"*(,"percolation_class":")*" << percolation.getClassName() << "\""
       << R"*(,"m":)*" << m
       << R"*(,"N":)*" << N
       << R"*(,"number_of_links":)*" << linkCount
       << R"*(,"number_of_nodes":)*" << nodeCount
       << R"*(,"M":)*" << M
       << R"*(,"ensemble_size":)*" << En
       << R"*(,"date":")*" << tm << "\""
       << "}";



    string filename = signature + "_entropy-order_" + tm + ".txt";
    ofstream fout(filename);
    fout << '#' << ss.str() << endl;
    fout << "# t=relative link density" << endl;
    fout << "#<t>\t<H>\t<P>" << endl;
    for(size_t k{}; k < limit ; ++k){
        auto t = (k+1)/double(N);
        fout << t
             << "\t" << entropy[k]/En
             << "\t" << order_param[k]/(En*double(N)) << endl;
    }
    fout.close();

    string filename_jump = signature + "_jump_" + tm + ".txt";

    ofstream fout_jump(filename_jump);
    fout_jump << '#' << ss.str() << endl;
    fout_jump << "#<m><N><M><En><largest entropy jump><largest order parameter jump>" << endl;
    fout_jump << m << "\t" << N << "\t" << M << "\t" << En
              << "\t" << abs(entropy_jump)/En << '\t' << order_jump/En << endl;
    fout_jump.close();
}

void test_BA(int argc, char* argv[]) {
    if(argc < 5 ){
        cout << "argv[1] == m" << endl;
        cout << "argv[2] == N" << endl;
        cout << "argv[3] == M" << endl;
        cout << "argv[4] == Ensemble" << endl;
        return;
    }
    int m = atoi(argv[1]);
    int N = atoi(argv[2]);
    int M = atoi(argv[3]);

    int En = atoi(argv[4]);
    cout << "m=" << m << ",N="<< N << ",M=" << M << ",En="<<En << endl;


    auto* net = new NetworkBA_v7(m);
    net->setRandomState(0, true);
    net->initialize(N);
    vector<size_t> counts;
    for(int e{1}; e <= En; ++e){
        auto t0 = std::chrono::system_clock::now();
        net->rebuild();
        auto dd = net->degreeCounts();
        if(counts.size() < dd.size()){
            counts.resize(dd.size());
        }
        for(size_t i{}; i < dd.size(); ++i){
//        if(dd[i] == 0) continue;
//            cout << "k= " << i << " count=" << dd[i] << endl;
            counts[i] += dd[i];
        }
        auto t1 = std::chrono::system_clock::now();
        std::chrono::duration<double> drtion = t1 - t0;
        cout << "Iteration " << e << " : time " << drtion.count() << " sec" << endl;
    }
//    net->view();
//    net->viewAdjacencyList();
//    net->viewLocal();
    cout << "Done" << endl;
//    auto dd = net->degreeCounts();
//    for(size_t i{}; i < dd.size(); ++i){
//        if(dd[i] == 0) continue;
//        cout << "k= " << i << " count=" << dd[i] << endl;
//    }

    auto arr  = net->get_signature_array();
    string filename = net->getClassName() +  arr[1] + "degree-distribution" + currentTime() + ".txt";
    ofstream fout(filename);
    fout << "#{\"m0\":" << net->get_m0()
         << ",\"m\":" << net->get_m()
         << ",\"N\":" << N
         << ",\"En\":" << En
         << "}" << endl;
    fout << "#<k>,<P(k)>" << endl;
    for (size_t i{1}; i < counts.size(); ++i) {
        if(counts[i] == 0) continue;
        fout << i << "\t" << double(counts[i])/En << endl;
    }

    // normalization
    fout.close();

}

void run_v7_percolation(int argc, char **argv) {
    if(argc < 5 ){
        cout << "argv[1] == m" << endl;
        cout << "argv[2] == N" << endl;
        cout << "argv[3] == M" << endl;
        cout << "argv[4] == Ensemble" << endl;
        return;
    }
    int m = atoi(argv[1]);
    int N = atoi(argv[2]);
    int M = atoi(argv[3]);

    int En = atoi(argv[4]);
    cout << "m=" << m << ",N="<< N << ",M=" << M << ",En="<<En << endl;
    size_t rebuild_each = 100; // cycle

    auto* net = new NetworkBA_v7(m);
//    auto* net = new NetworkMDA_v7(m);
    net->setRandomState(0, true);
    net->initialize(N);
    net->clearAdjacency();
//
//      NetworkPercolation_v7 percolation(net);
//    NetworkPercolationInverted_v7 percolation(net, M);
    NetworkPercolationExplosive_v7 percolation(net, M);

    percolation.initialize();
    size_t linkCount = net->getLinkCount();
    size_t nodeCount = net->getNodeCount();
    cout << nodeCount << ", " << linkCount << ", " << endl;
    size_t limit = nodeCount * 2;
//    double entropy_jump{}, order_jump{};
    vector<long double> entropy(linkCount), dHs(linkCount);
    vector<double> order_param(linkCount); // entropy and order parameter
    vector<long> dPs(linkCount);
    for (int k{1}; k <= En; ++k) {
        auto t_start= chrono::_V2::system_clock::now();
        percolation.reset(k % rebuild_each == 0); // every nn step. reset the network
        size_t i{};
        while (percolation.occupyLink()) {
//            cout << "i " << i  << endl;
//            auto H = percolation.entropy_v1(); // slow. for debugging purposes
//            percolation.viewClusters();
//            percolation.sumClusters();
            auto H = percolation.entropy();
            entropy[i] += H;
            order_param[i] += percolation.largestClusterSize();
            percolation.jump();

            dHs[i] += percolation.jump_entropy();
            dPs[i] += percolation.jump_largest_cluster();

            ++i;
            if (i >= limit) {
                cout << "breaking at " << i <<endl;
                break;
            }
        }
//        entropy_jump += percolation.largestEntropyJump();
//        order_jump += percolation.largestOrderJump();

        auto t_end= chrono::_V2::system_clock::now();
        chrono::duration<double> drtion = t_end - t_start;
        cout << "iteration " << k << " : time elapsed " << drtion.count() << " sec" << endl;
    }

    auto tm = currentTime();
//    string signature = percolation.get_signature();
    string signature = net->get_signature_array()[0] +"_"+ percolation.getClassName()
                       + "_N_" + to_string(N)
                       + "_m_" + to_string(m)
                       + "_M_" + to_string(M);
    stringstream ss;
    ss << "{"
       << R"*("signature":")*" << signature << "\""
       << R"*(,"network_class":")*" << net->getClassName() << "\""
       << R"*(,"percolation_class":")*" << percolation.getClassName() << "\""
       << R"*(,"random_seed":)*" << net->getRandomState()
       << R"*(,"m":)*" << m
       << R"*(,"N":)*" << N
       << R"*(,"number_of_links":)*" << linkCount
       << R"*(,"number_of_nodes":)*" << nodeCount
       << R"*(,"M":)*" << M
       << R"*(,"ensemble_size":)*" << En
       << R"*(,"En":)*" << En
       << R"*(,"date":")*" << tm << "\""
       << "}";



    string filename_entropy_order = signature + "_entropy-order_" + tm + ".txt";
    ofstream fout(filename_entropy_order);
    fout << '#' << ss.str() << endl;
    fout << "# t=relative link density" << endl;
    fout << "#<t>\t<H>\t<P>" << endl;
//    fout.precision(12);
    fout.precision(numeric_limits<double>::digits10 + 1); // maximum precision
    for(size_t k{}; k < limit ; ++k){
        auto t = (k+1)/double(N);
        fout << t
             << "\t" << entropy[k]/En
             << "\t" << order_param[k]/(En*double(N)) << endl;
    }
    fout.close();


    string filename_specific_heat_susceptibility = signature + "_specific_heat-susceptibility_" + tm + ".txt";
    ofstream fout2(filename_specific_heat_susceptibility);
    fout2 << '#' << ss.str() << endl;
    fout2 << "# t=relative link density" << endl;
    fout2 << "#<t>\t<C>\t<X>" << endl;
//    fout2.precision(12);
    fout2.precision(numeric_limits<double>::digits10 + 1); // maximum precision
    for(size_t k{}; k < limit ; ++k){
        auto t = (k+1)/double(N);
        fout2 << t
             << "\t" << -1.0*(1.0-t)*dHs[k]*N/En
             << "\t" << dPs[k]/double(En) << endl;
    }
    fout2.close();

}

void run_v7_percolation_near_tc(int argc, char **argv) {
    if(argc < 5 ){
        cout << "argv[1] == m" << endl;
        cout << "argv[2] == N" << endl;
        cout << "argv[3] == M" << endl;
        cout << "argv[4] == Ensemble" << endl;
        return;
    }
    int m = atoi(argv[1]);
    int N = atoi(argv[2]);
    int M = atoi(argv[3]);

    int En = atoi(argv[4]);
    cout << "m=" << m << ",N="<< N << ",M=" << M << ",En="<<En << endl;


    auto* net = new NetworkBA_v7(m);
    net->setRandomState(0, true);
    net->initialize(N);
    net->clearAdjacency();
//
//      NetworkPercolation_v7 percolation(net);
//    NetworkPercolationInverted_v7 percolation(net, M);
    NetworkPercolationExplosive_v7 percolation(net, M);

    percolation.initialize();
    size_t linkCount = net->getLinkCount();
    size_t nodeCount = net->getNodeCount();
    cout << nodeCount << ", " << linkCount << ", " << endl;
    size_t limit = nodeCount * 2;
//    double entropy_jump{}, order_jump{};
    vector<size_t> largest_cluster; // entropy and order parameter
    double tc = percolation.get_tc(M);
    cout << "tc = " << tc << endl;
    double tc0 = tc-0.1;
    double tc1 = tc+0.1;

    for (int k{1}; k <= En; ++k) {
        auto t_start= chrono::_V2::system_clock::now();
        percolation.reset(k % 25 == 0); // every 100 step. reset the network
        size_t i{};
        while (percolation.occupyLink()) {
//            cout << "i " << i  << endl;
//            largest_cluster[i++] += percolation.largestClusterSize();

            auto t = percolation.relativeLinkDensity();
//            cout << "relativeLinkDensity " << t <<  " 1/N" << 1.0 / nodeCount << " 1/M" << 1.0/ linkCount << endl;
            auto sz = percolation.largestClusterSize();
            if(t > tc0) {
                if (i < largest_cluster.size()) {
                    largest_cluster[i++] += sz;
                }else{
                    largest_cluster.emplace_back(sz);
                }
//            percolation.jump();

            }
            if(t > tc1){
                break;
            }
            if (i >= limit) {
                cout << "breaking at " << i <<endl;
                break;
            }
        }
//        entropy_jump += percolation.largestEntropyJump();
//        order_jump += percolation.largestOrderJump();

        auto t_end= chrono::_V2::system_clock::now();
        chrono::duration<double> drtion = t_end - t_start;
        cout << "iteration " << k << " : time elapsed " << drtion.count() << " sec" << endl;
    }

    auto tm = currentTime();
//    string signature = percolation.get_signature();
    string signature = net->get_signature_array()[0] +"_"+ percolation.getClassName()
                       + "_N_" + to_string(N)
                       + "_m_" + to_string(m)
                       + "_M_" + to_string(M);
    double dt = 1.0 / nodeCount;
    stringstream ss;
    ss << "{"
       << R"*("signature":")*" << signature << "\""
       << R"*(,"network_class":")*" << net->getClassName() << "\""
       << R"*(,"percolation_class":")*" << percolation.getClassName() << "\""
       << R"*(,"random_seed":)*" << net->getRandomState()
       << R"*(,"m":)*" << m
       << R"*(,"N":)*" << N
       << R"*(,"number_of_links":)*" << linkCount
       << R"*(,"number_of_nodes":)*" << nodeCount
       << R"*(,"M":)*" << M
       << R"*(,"tc":)*" << tc
       << R"*(,"dt":)*" << dt

       << R"*(,"ensemble_size":)*" << En
       << R"*(,"En":)*" << En
       << R"*(,"date":")*" << tm << "\""
       << "}";



    cout << "dt = " << dt << endl;
    string filename = signature + "_largest_cluster_near_tc_" + tm + ".txt";
    ofstream fout(filename);
    fout << '#' << ss.str() << endl;
    fout << "# t=relative link density" << endl;
    fout << "# S_max = largest cluster" << endl;
    fout << "#<t>\t<S_max>" << endl;
//    fout.precision(12);
    fout.precision(numeric_limits<double>::digits10 + 1); // maximum precision
    auto t = tc0;
    for(size_t k{}; k < largest_cluster.size() ; ++k){
        fout << t  << "\t" << largest_cluster[k]/double(En) << endl;
        t += dt;
    }
    fout.close();

}

void run_v7_percolation_jump_avg(int argc, char **argv) {
    if(argc < 5 ){
        cout << "argv[1] == m" << endl;
        cout << "argv[2] == N" << endl;
        cout << "argv[3] == M" << endl;
        cout << "argv[4] == Ensemble" << endl;
        return;
    }
    int m = atoi(argv[1]);
    int N = atoi(argv[2]);
    int M = atoi(argv[3]);

    int En = atoi(argv[4]);
    cout << "m=" << m << ",N="<< N << ",M=" << M << ",En="<<En << endl;


    auto* net = new NetworkBA_v7(m);
    net->setRandomState(0, true);
    net->initialize(N);
    net->clearAdjacency();
//
//      NetworkPercolation_v7 percolation(net);
//    NetworkPercolationInverted_v7 percolation(net, M);
    NetworkPercolationExplosive_v7 percolation(net, M);

    percolation.initialize();
    size_t linkCount = net->getLinkCount();
    size_t nodeCount = net->getNodeCount();
    cout << nodeCount << ", " << linkCount << ", " << endl;
    size_t limit = nodeCount * 2;
    double entropy_jump{}, order_jump{};

    for (int k{1}; k <= En; ++k) {
        auto t_start= chrono::_V2::system_clock::now();
//        net.viewListOfLinkIndices();
        percolation.reset(k % 25 == 0); // every 100 step. reset the network
//        cout << net->getLinkCount() << endl;
        size_t i{};
        while (percolation.occupyLink()) {
//            cout << "i " << i  << endl;
            percolation.entropy();
            percolation.jump();

            ++i;
            if (i >= limit) {
                cout << "breaking at " << i <<endl;
                break;
            }
        }
        entropy_jump += percolation.largestEntropyJump();
        order_jump += percolation.largestOrderJump();

//        cout << entropy_jump[k] << " at " << entropy_jump_pc[k] << endl;
        auto t_end= chrono::_V2::system_clock::now();
        chrono::duration<double> drtion = t_end - t_start;
        cout << "iteration " << k << " : time elapsed " << drtion.count() << " sec" << endl;
    }

    auto tm = currentTime();
//    string signature = percolation.get_signature();
    string signature = net->get_signature_array()[0] +"_"+ percolation.getClassName()
                       + "_N_" + to_string(N)
                       + "_m_" + to_string(m)
                       + "_M_" + to_string(M);
    stringstream ss;
    ss << "{"
       << R"*("signature":")*" << signature << "\""
       << R"*(,"network_class":")*" << net->getClassName() << "\""
       << R"*(,"percolation_class":")*" << percolation.getClassName() << "\""
       << R"*(,"random_seed":)*" << net->getRandomState()
       << R"*(,"m":)*" << m
       << R"*(,"N":)*" << N
       << R"*(,"number_of_links":)*" << linkCount
       << R"*(,"number_of_nodes":)*" << nodeCount
       << R"*(,"M":)*" << M
       << R"*(,"ensemble_size":)*" << En
       << R"*(,"En":)*" << En
       << R"*(,"date":")*" << tm << "\""
       << "}";



    string filename_jump = signature + "_jump_" + tm + ".txt";

    ofstream fout_jump(filename_jump);
    fout_jump << '#' << ss.str() << endl;
    fout_jump << "#<m><N><M><En><largest entropy jump><largest order parameter jump>" << endl;
    fout_jump << m << "\t" << N << "\t" << M << "\t" << En
              << "\t" << abs(entropy_jump)/En << '\t' << order_jump/En << endl;
    fout_jump.close();
}


void run_v7_percolation_jump(int argc, char **argv) {
    if(argc < 5 ){
        cout << "argv[1] == m" << endl;
        cout << "argv[2] == N" << endl;
        cout << "argv[3] == M" << endl;
        cout << "argv[4] == Ensemble" << endl;
        return;
    }
    int m = atoi(argv[1]);
    int N = atoi(argv[2]);
    int M = atoi(argv[3]);

    int En = atoi(argv[4]);
    cout << "m=" << m << ",N="<< N << ",M=" << M << ",En="<<En << endl;
    size_t rebuild_each = 1; // cycle

//    auto* net = new NetworkBA_v7(m, m);
    auto* net = new NetworkMDA_v7(m);
    net->setRandomState(0, true);
    net->initialize(N);
    net->clearAdjacency();
//
//      NetworkPercolation_v7 percolation(net);
//    NetworkPercolationInverted_v7 percolation(net, M);
    NetworkPercolationExplosive_v7 percolation(net, M);

    percolation.initialize();
    size_t linkCount = net->getLinkCount();
    size_t nodeCount = net->getNodeCount();
    cout << nodeCount << ", " << linkCount << ", " << endl;
    size_t limit = nodeCount * 2;
    vector<double> entropy_jump, order_jump;

    for (int k{1}; k <= En; ++k) {
        auto t_start= chrono::_V2::system_clock::now();
//        net.viewListOfLinkIndices();
        percolation.reset(k % rebuild_each == 0); // every 100 step. reset the network
//        cout << net->getLinkCount() << endl;
        size_t i{};
        while (percolation.occupyLink()) {
//            cout << "i " << i  << endl;
            percolation.entropy();
            percolation.jump();

            ++i;
            if (i >= limit) {
                cout << "breaking at " << i <<endl;
                break;
            }
        }
        entropy_jump.emplace_back(percolation.largestEntropyJump());
        order_jump.emplace_back(percolation.largestOrderJump());

//        cout << entropy_jump[k] << " at " << entropy_jump_pc[k] << endl;
        auto t_end= chrono::_V2::system_clock::now();
        chrono::duration<double> drtion = t_end - t_start;
        cout << "iteration " << k << " : time elapsed " << drtion.count() << " sec" << endl;
    }

    auto tm = currentTime();
//    string signature = percolation.get_signature();
    string signature = net->get_signature_array()[0] +"_"+ percolation.getClassName()
                       + "_N_" + to_string(N)
                       + "_m_" + to_string(m)
                       + "_M_" + to_string(M);
    stringstream ss;
    ss << "{"
       << R"*("signature":")*" << signature << "\""
       << R"*(,"network_class":")*" << net->getClassName() << "\""
       << R"*(,"percolation_class":")*" << percolation.getClassName() << "\""
       << R"*(,"random_seed":)*" << net->getRandomState()
       << R"*(,"m":)*" << m
       << R"*(,"N":)*" << N
       << R"*(,"number_of_links":)*" << linkCount
       << R"*(,"number_of_nodes":)*" << nodeCount
       << R"*(,"M":)*" << M
       << R"*(,"ensemble_size":)*" << En
       << R"*(,"En":)*" << En
       << R"*(,"date":")*" << tm << "\""
       << "}";



    string filename_jump = signature + "_jump_" + tm + ".txt";

    ofstream fout_jump(filename_jump);
    fout_jump << '#' << ss.str() << endl;
    fout_jump << "#<entropy jump><order parameter jump>" << endl;
    for(size_t i{}; i < entropy_jump.size(); ++i){
        fout_jump << entropy_jump[i] << "\t" << order_jump[i] << endl;
    }
    fout_jump.close();
}
