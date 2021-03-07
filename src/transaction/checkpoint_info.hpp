//
// log_checkpoint_info.hpp - the information saved during log checkpoint and used for recovery
//
// Replaces LOG_REC_CHKPT
namespace cublog
{
  using checkpoint_tran_info = log_info_chkpt_trans;	// todo: replace log_info_chkpt_trans
  using checkpoint_sysop_info = log_info_chkpt_sysop;	// todo: replace log_info_chkpt_sysop

  class checkpoint_info : cubpacking::packable_object
  {
    public:
      checkpoint_info () = default;
      checkpoint_info (checkpoint_info &&) = default;
      checkpoint_info (const checkpoint_info &) = default;

      void pack (cubpacking::packer &serializator) const override;
      void unpack (cubpacking::unpacker &deserializator) override;
      size_t get_packed_size (cubpacking::packer &serializator, std::size_t start_offset) const override;

      void load_trantable_snapshot ();		      // with tran table and prior lock, save snapshot LSA and
      // get trans/sysops info from transaction table
      void recovery_analysis (log_lsa &start_redo_lsa) const;	  // restore transaction table based on checkpoint info
      void recovery_2pc_analysis () const;	      // if m_has_2pc, also do 2pc analysis

      const log_lsa &get_snapshot_lsa () const;	      // the LSA of loaded snapshot
      const log_lsa &get_start_redo_lsa ()
      const;     // the LSA of starting redo. min of checkpoint LSA and oldest unflushed LSA.
      void set_start_redo_lsa (const log_lsa &start_redo_lsa);

    private:
      log_lsa m_start_redo_lsa;
      log_lsa m_snapshot_lsa;
      std::vector<checkpoint_tran_info> m_trans;
      std::vector<checkpoint_sysop_info> m_sysops;
      bool m_has_2pc;				      // true if any LOG_ISTRAN_2PC (tdes) is true
  };
}
