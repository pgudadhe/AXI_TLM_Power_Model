#include "axi_target.h"

tlm_sync_enum AXITarget::nb_transport_fw(tlm_generic_payload& trans, tlm_phase& phase, sc_time& delay) 
{
    if (phase == BEGIN_REQ) 
    {
        // Process transaction
        tlm_sync_enum retVal = process_transaction(trans, phase, delay);
        return retVal;
    } 
    else if (phase == END_RESP) 
    {
        return TLM_COMPLETED;
    }
    return TLM_ACCEPTED;
}

tlm_sync_enum AXITarget::process_transaction(tlm_generic_payload& trans, tlm_phase& phase, sc_time& delay) 
{
    // Set response status
    trans.set_response_status(TLM_OK_RESPONSE);

    // Is it a READ or WRITE command?
    if(trans.is_write()) 
    {
        //m_wr_req_fifo.write(trans);
       DEBUG_PRINT(2,   "AXITarget: WRITE request received at time: " << sc_time_stamp() );
        m_stat_wr_delay += delay; // Update statistics for write delay
        phase = END_REQ; 
        this->powerModelPort->reportEvent(m_nbtransportWriteEventId);   
        return TLM_COMPLETED; // For WRITE commands, we complete immediately
    }
    else 
    {
        // How many beats based of the read bus width
        //m_rd_req_fifo.write(trans);
        uint32_t beats = trans.get_data_length() / ((m_cfg_axi_read_bus_width + 1) << 2);
        delay += sc_time(AXI_CLK_PERIOD) * beats * 2; // Add delay based on the number of beats
        phase = BEGIN_RESP; 
        this->powerModelPort->reportEvent(m_nbtransportReadEventId);
        return TLM_UPDATED; // For READ commands, we may need to update later
    }
} 

void AXITarget::end_of_elaboration(){
  // The events are assigned in a fixed order from array (in case of having many events). 
  // If only a few events are defined, could probably have one parameter for each event.
  this->m_nbtransportReadEventId = this->powerModelPort->registerEvent(
          this->name(), 
          std::unique_ptr<ConstantEnergyEvent>(new ConstantEnergyEvent("m_btransportReadEventId", read_energy_consumption))); // unique_ptr instead of make_unique due to c++ version
  this->m_nbtransportWriteEventId = this->powerModelPort->registerEvent(
          this->name(), 
          std::unique_ptr<ConstantEnergyEvent>(new ConstantEnergyEvent("m_btransportWriteEventId", write_energy_consumption)));
  this->m_State1Id = this->powerModelPort->registerState(
          this->name(), 
          std::unique_ptr<ConstantCurrentState>(new ConstantCurrentState("m_State1Id", state1_current)));
  this->m_State2Id = this->powerModelPort->registerState(
          this->name(), 
          std::unique_ptr<ConstantCurrentState>(new ConstantCurrentState("m_State2Id", state2_current)));
}