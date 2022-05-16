// Copyright 2022 Yue Yu
// This file is part of APlayer.
// APlayer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// APlayer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with APlayer. If not, see <https://www.gnu.org/licenses/>.
//
// Created by yuyue on 5/1/22.
//

#ifndef APLAYER_CPP_CLION_BLKNONSEEKPOPENMANAGER_H
#define APLAYER_CPP_CLION_BLKNONSEEKPOPENMANAGER_H

#include "BlkManager.h"
#include <cstdio>

namespace aplayer {
    class BlkNonseekPopenManager : public BlkManager {
    public:
        BlkNonseekPopenManager(const char *path, uint64_t blk_size);
        BlkNonseekPopenManager(const char *path, const char *command, uint64_t blk_size);
        ~BlkNonseekPopenManager();
        int get_blk_ind() override;
        uint64_t get_blk(int blk_ind) override;
        uint64_t get_position() override;
        int set_position(uint64_t position) override;
        void flush_all() override;

        bool seekable() override {
            return false;
        }

    private:
        void dummy_seek(uint64_t offset);
        char m_dummy_buf[64];
        FILE * m_p_in; // read-only, one-way, non-back-seekable, ftell() doesn't work
        uint64_t m_position;
        int last_blk_ind {-1}; // last time get_blk
        int last_blk_size {-1};
    };
}


#endif //APLAYER_CPP_CLION_BLKNONSEEKPOPENMANAGER_H
