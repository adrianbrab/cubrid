
#include "checkpoint_info.hpp"

namespace cublog
{

  void checkpoint_info::load_trantable_snapshot ()
  {
  }

  int
  log_lsa_size (LOG_LSA log, cubpacking::packer &serializator)
  {
    int size = 0;
    size += serializator.get_packed_bigint_size (log.pageid);
    size += serializator.get_packed_bigint_size (log.offset);

    return size;
  }

  void
  log_lsa_pack (LOG_LSA log, cubpacking::packer &serializator)
  {
    serializator.pack_bigint (log.pageid);
    serializator.pack_bigint (log.offset);
  }

  void
  checkpoint_info::pack (cubpacking::packer &serializator) const
  {

    log_lsa_pack (m_start_redo_lsa, serializator);
    log_lsa_pack (m_snapshot_lsa, serializator);

    serializator.pack_bigint (m_trans.size ());
    for (const checkpoint_tran_info tran_info : m_trans)
      {
	serializator.pack_int (tran_info.isloose_end);
	serializator.pack_int (tran_info.trid);
	serializator.pack_int (tran_info.state);
	log_lsa_pack (tran_info.head_lsa, serializator);
	log_lsa_pack (tran_info.tail_lsa, serializator);
	log_lsa_pack (tran_info.undo_nxlsa, serializator);

	log_lsa_pack (tran_info.posp_nxlsa, serializator);
	log_lsa_pack (tran_info.savept_lsa, serializator);
	log_lsa_pack (tran_info.tail_topresult_lsa, serializator);
	log_lsa_pack (tran_info.start_postpone_lsa, serializator);
	serializator.pack_c_string (tran_info.user_name, LOG_USERNAME_MAX);
      }

    serializator.pack_bigint (m_sysops.size ());
    for (const checkpoint_sysop_info sysop_info : m_sysops)
      {
	serializator.pack_int (sysop_info.trid);
	log_lsa_pack (sysop_info.sysop_start_postpone_lsa, serializator);
	log_lsa_pack (sysop_info.atomic_sysop_start_lsa, serializator);
      }

    serializator.pack_bool (m_has_2pc);
  }

  void
  checkpoint_info::unpack (cubpacking::unpacker &deserializator)
  {
    int64_t pageid, offset;

    deserializator.unpack_bigint (pageid);
    deserializator.unpack_bigint (offset);
    m_start_redo_lsa = log_lsa (pageid, offset);

    deserializator.unpack_bigint (pageid);
    deserializator.unpack_bigint (offset);
    m_snapshot_lsa = log_lsa (pageid, offset);

    size_t m_trans_size = 0;
    deserializator.unpack_bigint (m_trans_size);
    for (int i = 0; i < m_trans_size; i++)
      {
	LOG_INFO_CHKPT_TRANS chkpt_trans;

	deserializator.unpack_int (chkpt_trans.isloose_end);
	deserializator.unpack_int (chkpt_trans.trid);

	int tran_state_int;
	deserializator.unpack_int (tran_state_int);
	chkpt_trans.state = static_cast<TRAN_STATE> (tran_state_int);

	deserializator.unpack_bigint (pageid);
	deserializator.unpack_bigint (offset);
	chkpt_trans.head_lsa = log_lsa (pageid, offset);

	deserializator.unpack_bigint (pageid);
	deserializator.unpack_bigint (offset);
	chkpt_trans.tail_lsa = log_lsa (pageid, offset);

	deserializator.unpack_bigint (pageid);
	deserializator.unpack_bigint (offset);
	chkpt_trans.undo_nxlsa = log_lsa (pageid, offset);

	deserializator.unpack_bigint (pageid);
	deserializator.unpack_bigint (offset);
	chkpt_trans.savept_lsa = log_lsa (pageid, offset);

	deserializator.unpack_bigint (pageid);
	deserializator.unpack_bigint (offset);
	chkpt_trans.tail_topresult_lsa = log_lsa (pageid, offset);

	deserializator.unpack_bigint (pageid);
	deserializator.unpack_bigint (offset);
	chkpt_trans.start_postpone_lsa = log_lsa (pageid, offset);
	deserializator.unpack_c_string (chkpt_trans.user_name, LOG_USERNAME_MAX);

	m_trans.push_back (chkpt_trans);

      }

    size_t m_sysop_size = 0;
    deserializator.unpack_bigint (m_sysop_size);
    for (int i = 0; i < m_sysop_size; i++)
      {
	LOG_INFO_CHKPT_SYSOP chkpt_sysop;
	deserializator.unpack_int (chkpt_sysop.trid);

	deserializator.unpack_bigint (pageid);
	deserializator.unpack_bigint (offset);
	chkpt_sysop.sysop_start_postpone_lsa = log_lsa (pageid, offset);

	deserializator.unpack_bigint (pageid);
	deserializator.unpack_bigint (offset);
	chkpt_sysop.atomic_sysop_start_lsa = log_lsa (pageid, offset);

	m_sysops.push_back (chkpt_sysop);
      }

    deserializator.unpack_bool (m_has_2pc);
  }

  size_t
  checkpoint_info::get_packed_size (cubpacking::packer &serializator, std::size_t start_offset) const
  {
    size_t size = log_lsa_size (m_start_redo_lsa, serializator);
    size += log_lsa_size (m_snapshot_lsa, serializator);

    size += serializator.get_packed_bigint_size (m_trans.size ());
    for (const checkpoint_tran_info tran_info : m_trans)
      {
	size += serializator.get_packed_int_size (tran_info.isloose_end);
	size += serializator.get_packed_int_size (tran_info.trid);
	size += serializator.get_packed_int_size (tran_info.state);

	size += log_lsa_size (tran_info.head_lsa, serializator);
	size += log_lsa_size (tran_info.tail_lsa, serializator);
	size += log_lsa_size (tran_info.undo_nxlsa, serializator);

	size += log_lsa_size (tran_info.posp_nxlsa, serializator);
	size += log_lsa_size (tran_info.savept_lsa, serializator);
	size += log_lsa_size (tran_info.tail_topresult_lsa, serializator);
	size += log_lsa_size (tran_info.start_postpone_lsa, serializator);
	size += serializator.get_packed_c_string_size (tran_info.user_name, LOG_USERNAME_MAX, 0);
      }

    size += serializator.get_packed_bigint_size (m_sysops.size ());
    for (const checkpoint_sysop_info sysop_info : m_sysops)
      {
	size += serializator.get_packed_int_size (sysop_info.trid);
	size += log_lsa_size (sysop_info.sysop_start_postpone_lsa, serializator);
	size += log_lsa_size (sysop_info.atomic_sysop_start_lsa, serializator);
      }

    size += serializator.get_packed_bool_size (m_has_2pc);

    return size;
  }

}

