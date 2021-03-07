
namespace cublog
{
  void load_trantable_snapshot ()
  {
    TR_TABLE_CS_ENTER (thread_p);

    /* allocate memory space for the transaction descriptors */
    tmp_chkpt.ntrans = log_Gl.trantable.num_assigned_indices;
    length_all_chkpt_trans = sizeof (*chkpt_trans) * tmp_chkpt.ntrans;

    chkpt_trans = (LOG_INFO_CHKPT_TRANS *) malloc (length_all_chkpt_trans);
    if (chkpt_trans == NULL)
      {
	TR_TABLE_CS_EXIT (thread_p);
	goto error_cannot_chkpt;
      }

    log_Gl.prior_info.prior_lsa_mutex.lock ();

    /* CHECKPOINT THE TRANSACTION TABLE */

    LSA_SET_NULL (&smallest_lsa);
    for (i = 0, ntrans = 0, ntops = 0; i < log_Gl.trantable.num_total_indices; i++)
      {
	/*
	 * Don't checkpoint current system transaction. That is, the one of
	 * checkpoint process
	 */
	if (i == LOG_SYSTEM_TRAN_INDEX)
	  {
	    continue;
	  }
	act_tdes = LOG_FIND_TDES (i);
	assert (ntrans < tmp_chkpt.ntrans);
	logpb_checkpoint_trans (chkpt_trans, act_tdes, ntrans, ntops, smallest_lsa);
      }

    /*
     * Reset the structure to the correct number of transactions and
     * recalculate the length
     */
    tmp_chkpt.ntrans = ntrans;
    length_all_chkpt_trans = sizeof (*chkpt_trans) * tmp_chkpt.ntrans;

    /*
     * Scan again if there were any top system operations in the process of being committed.
     * NOTE that we checkpoint top system operations only when there are in the
     * process of commit. Not knowledge of top system operations that are not
     * in the process of commit is required since if there is a crash, the system
     * operation is aborted as part of the transaction.
     */

    chkpt_topops = NULL;
    if (ntops > 0)
      {
	tmp_chkpt.ntops = log_Gl.trantable.num_assigned_indices;
	length_all_tops = sizeof (*chkpt_topops) * tmp_chkpt.ntops;
	chkpt_topops = (LOG_INFO_CHKPT_SYSOP *) malloc (length_all_tops);
	if (chkpt_topops == NULL)
	  {
	    free_and_init (chkpt_trans);
	    log_Gl.prior_info.prior_lsa_mutex.unlock ();
	    TR_TABLE_CS_EXIT (thread_p);
	    goto error_cannot_chkpt;
	  }

	/* CHECKPOINTING THE TOP ACTIONS */
	for (i = 0, ntrans = 0, ntops = 0; i < log_Gl.trantable.num_total_indices; i++)
	  {
	    /*
	     * Don't checkpoint current system transaction. That is, the one of
	     * checkpoint process
	     */
	    if (i == LOG_SYSTEM_TRAN_INDEX)
	      {
		continue;
	      }
	    act_tdes = LOG_FIND_TDES (i);
	    error_code =
		    logpb_checkpoint_topops (thread_p, chkpt_topops, chkpt_trans, tmp_chkpt, act_tdes, ntops, length_all_tops);
	    if (error_code != NO_ERROR)
	      {
		goto error_cannot_chkpt;
	      }
	  }
      }
    else
      {
	tmp_chkpt.ntops = 1;
	length_all_tops = sizeof (*chkpt_topops) * tmp_chkpt.ntops;
	chkpt_topops = (LOG_INFO_CHKPT_SYSOP *) malloc (length_all_tops);
	if (chkpt_topops == NULL)
	  {
	    free_and_init (chkpt_trans);
	    log_Gl.prior_info.prior_lsa_mutex.unlock ();
	    TR_TABLE_CS_EXIT (thread_p);
	    goto error_cannot_chkpt;
	  }
      }

    // Checkpoint system transactions' topops
    // *INDENT-OFF*
    mapper = [thread_p, &chkpt_topops, &chkpt_trans, &tmp_chkpt, &ntops, &length_all_tops, &error_code] (log_tdes &tdes)
    {
      error_code =
        logpb_checkpoint_topops (thread_p, chkpt_topops, chkpt_trans, tmp_chkpt, &tdes, ntops, length_all_tops);
    };

    log_system_tdes::map_all_tdes (mapper);
    // *INDENT-ON*
    if (error_code != NO_ERROR)
      {
	goto error_cannot_chkpt;
      }

    assert (sizeof (*chkpt_topops) * ntops <= length_all_tops);
    tmp_chkpt.ntops = ntops;
    length_all_tops = sizeof (*chkpt_topops) * tmp_chkpt.ntops;

    node =
	    prior_lsa_alloc_and_copy_data (thread_p, LOG_END_CHKPT, RV_NOT_DEFINED, NULL, length_all_chkpt_trans,
					   (char *) chkpt_trans, (int) length_all_tops, (char *) chkpt_topops);
    if (node == NULL)
      {
	free_and_init (chkpt_trans);

	if (chkpt_topops != NULL)
	  {
	    free_and_init (chkpt_topops);
	  }
	log_Gl.prior_info.prior_lsa_mutex.unlock ();
	TR_TABLE_CS_EXIT (thread_p);
	goto error_cannot_chkpt;
      }

    chkpt = (LOG_REC_CHKPT *) node->data_header;
    *chkpt = tmp_chkpt;

    prior_lsa_next_record_with_lock (thread_p, node, tdes);

    log_Gl.prior_info.prior_lsa_mutex.unlock ();

    TR_TABLE_CS_EXIT (thread_p);
  }

}

