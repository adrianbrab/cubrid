/*
 * Copyright 2008 Search Solution Corporation
 * Copyright 2016 CUBRID Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "checkpoint_info.hpp"
#include "log_lsa.hpp"

#include <algorithm>
#include <array>
#include <random>
#include <cstdlib>

#undef strlen

class test_env_chkpt
{
  public:
    test_env_chkpt ();
    ~test_env_chkpt ();

    LOG_LSA generate_log_lsa();
    std::vector<LOG_LSA> used_logs;

    int max = 32700;

  private:
    static void require_equal (checkpoint_info *before, checkpoint_info *after);

    checkpoint_info *before;
    checkpoint_info *after;
};

TEST_CASE ("Test pack/unpack checkpoint_info class", "")
{
  test_env_chkpt *env = new test_env_chkpt ();

}

test_env_chkpt::test_env_chkpt ()
{
  before = new checkpoint_info ();
  after  = new checkpoint_info ();
}

test_env_chkpt::~test_env_chkpt ()
{
}

LOG_LSA
test_env_chkpt::generate_log_lsa()
{

}

void
test_env_chkpt::require_equal (checkpoint_info *before, checkpoint_info *after)
{

}

//
// Definitions of CUBRID stuff that is not used, but is needed by the linker
//


TDE_CIPHER tde_Cipher;
log_global log_Gl;
pstat_global pstat_Global;
pstat_metadata pstat_Metadata[1];

void
_er_log_debug (const char *file_name, const int line_no, const char *fmt, ...)
{
  assert (false);
}

