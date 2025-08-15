#ifndef AXI_TARGET_H
#define AXI_TARGET_H   

#include "axi_common.h"

class AXITarget : public sc_module {
public:
    tlm_utils::simple_target_socket<AXITarget> socket;
    PowerModelEventOutPort powerModelPort{"powerModelPort"};

    //SC_HAS_PROCESS(AXITarget); 

    AXITarget(  sc_module_name name, 
                uint32_t debug_level, 
                uint32_t read_bus_width, 
                double read_energy_consumption, 
                double write_energy_consumption, 
                double state1_current, 
                double state2_current) : 
            sc_module(name),
            m_cfg_debug_level(debug_level),
            m_read_energy_consumption(read_energy_consumption),
            m_write_energy_consumption(write_energy_consumption),
            m_state1_current(state1_current),
            m_state2_current(state2_current),
            m_cfg_axi_read_bus_width(read_bus_width),
            socket("socket")
    {
        socket.register_nb_transport_fw(this, &AXITarget::nb_transport_fw);
    }   

    void end_of_simulation() override {
        cout << "AXITarget: Total Write Delay: " << m_stat_wr_delay << " " << endl;
    }
    tlm_sync_enum nb_transport_fw(tlm_generic_payload& trans, tlm_phase& phase, sc_time& delay) ;
    tlm_sync_enum process_transaction(tlm_generic_payload& trans, tlm_phase& phase, sc_time& delay) ;

    // Power Modeling related methods and variables
    double m_read_energy_consumption;
    double m_write_energy_consumption;
    double m_state1_current;
    double m_state2_current;

    int m_nbtransportReadEventId = {-1}; // Dummy power event
    int m_nbtransportWriteEventId = {-1}; // Dummy power event
    int m_State1Id = {-1}; // Leakage power
    int m_State2Id = {-1}; // Leakage power

  virtual void end_of_elaboration() override;

private:

    uint32_t m_cfg_axi_read_bus_width = 0; // 0:4B, 1:8B, 2:16B, 3:32B, 4:64B
    uint32_t m_cfg_debug_level = 0; // Debug level

    sc_time m_stat_wr_delay = SC_ZERO_TIME; // Statistics for write delay
};

#endif // AXI_TARGET_H
