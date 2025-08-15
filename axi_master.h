#ifndef AXI_MASTER_H
#define AXI_MASTER_H

#include <random>
#include "axi_common.h"

class AXIMaster : public sc_module {
public:
    tlm_utils::simple_initiator_socket<AXIMaster> socket;
    tlm_utils::peq_with_cb_and_phase<AXIMaster> peq;


    //SC_HAS_PROCESS(AXIMaster);  

    AXIMaster(  sc_module_name name, 
                uint32_t debug_level = 0,
                uint32_t total_transactions = 10,
                uint32_t req_size = 0, // 0: 4B to 4096B randomized size, else: Absolute fixed size
                float wr_to_rd_ratio = 0.5, // Default 50% Write, 50% Read 
                uint32_t write_bus_width = 0,
                double state1_current, 
                double state2_current, 
                double load_energy,
                double store_energy) : 
        sc_module(name),
        m_cfg_debug_level(debug_level),
        m_cfg_req_size(req_size),
        m_cfg_total_transactions(total_transactions),
        m_cfg_wr_to_rd_ratio(wr_to_rd_ratio),
        m_cfg_axi_write_bus_width(write_bus_width),
        m_state1_current(state1_current),
        m_state2_current(state2_current),
        m_load_energy(load_energy),
        m_store_energy(store_energy),
        socket("socket"), 
        peq(this, &AXIMaster::peq_cb)
    {
        SC_METHOD(traffic_generator_method);
        sensitive << m_traffic_generator_event;
    }   

    void end_of_simulation() override {
        cout << "AXIMaster: Total Read Delay: " << m_stat_rd_delay << " " << endl;
    }

    void traffic_generator_method();

    // TLM functions
    tlm_sync_enum nb_transport_bw(tlm_generic_payload& trans, tlm_phase& phase, sc_time& delay);
    void peq_cb(tlm_generic_payload& trans, const tlm_phase& phase);
    uint32_t generate_data_len();

    // Power Modeling related members and functions
    PowerModelEventOutPort powerModelPort{"powerModelPort"};

    virtual void end_of_elaboration() override;

    double m_state1_current;
    double m_state2_current;
    double m_load_energy;
    double m_store_energy;

    int c_State1Id{-1};
    int c_State2Id{-1};
    int c_loadEventId{-1};
    int c_storeEventId{-1};


private:
    tlm::tlm_command generate_tlm_command();

    sc_event m_traffic_generator_event;
    uint32_t m_transaction_count = 0;
    uint32_t m_cfg_total_transactions = 100;
    float m_cfg_wr_to_rd_ratio = 0.5; // Default 50% Write, 50% Read
    uint32_t m_cfg_axi_write_bus_width = 0; // 0:4B, 1:8B, 2:16B, 3:32B, 4:64B
    uint32_t m_cfg_debug_level = 0; // Debug level
    uint32_t m_cfg_req_size = 0; // Request size in bytes, 0: 4B to 4096B randomized size, else: Absolute fixed size

    sc_time m_stat_rd_delay = SC_ZERO_TIME; // Statistics for read delay
};

#endif // AXI_MASTER_H