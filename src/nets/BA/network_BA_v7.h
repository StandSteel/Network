//
// Created by shahnoor on 11/1/19.
//

#ifndef NETWORK_NETWORK_BA_V7_H
#define NETWORK_NETWORK_BA_V7_H


#include "../network.h"

// Barabasi Albert network
class NetworkBA_v7 : public Network_v7{
    size_t _link_count;
    size_t  _node_index;
    // constant properties
    uint _m0{3}; // seed
    uint _m{2}; // number of new nodes each node comes with

    // variable properties

    size_t _N_tmp;
    /*
     * Very useful for preferential attachment
     */
    std::vector<uint> _preferentially;
public:
    explicit NetworkBA_v7(uint m0=3, uint m=1); // only seeding
    void clear_preferentially(){_preferentially.clear();}
    uint get_m0()const { return  _m0;}
    uint get_m() const { return  _m;}
    virtual std::string getClassName(){return "Network_BA";}
    void rebuild() override ;
    void initialize(size_t N) override;

    std::string get_signature() override {
        return "???";
    }
    std::vector<std::string> get_signature_array()  override;
    void viewLocal() override; // other properties of a network that is different for different types of network
    bool grow(size_t n);
    void setMaxNetworkSize(size_t N) {
        if(_N_tmp > N){cout << "cannot initiate with smaller size " << __LINE__ << endl;}

    }

    bool addNode();
};

#endif //NETWORK_NETWORK_BA_V7_H