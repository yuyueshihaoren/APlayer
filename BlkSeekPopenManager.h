// Copyright 2022 Yue Yu
// This file is part of APlayer.
// APlayer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// APlayer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with APlayer. If not, see <https://www.gnu.org/licenses/>.
//
// Created by yuyue on 5/3/22.
//

#ifndef APLAYER_BLKSEEKPOPENMANAGER_H
#define APLAYER_BLKSEEKPOPENMANAGER_H

#include "BlkManager.h"
#include <cstdio>

namespace aplayer {
    // Dumb version of seeking, please do caching in BlkIOMiddleware
    class BlkSeekPopenManager: public BlkManager {
    public:
        BlkSeekPopenManager(const char *path, uint64_t blk_size);
        BlkSeekPopenManager(const char *path, const char *command, uint64_t blk_size);
        ~BlkSeekPopenManager() override;
        int get_blk_ind() override;
        uint64_t get_blk(int blk_ind) override; // don't touch m_position
        uint64_t get_position() override;
        int set_position(uint64_t position) override;
        void flush_all() override;

        bool seekable() override {
            return true;
        }
    private:
        void ftell_dummy_seek_set(uint64_t offset); // don't touch m_position
        char m_dummy_buf[64];
        const char * m_command;
        FILE * m_p_in;
        uint64_t m_position;
        uint64_t m_pos_ftell;
        int last_blk_ind {-1}; // last time get_blk
        int last_blk_size {-1};

        const std::string _escape_path();
    };

} // aplayer

#endif //APLAYER_BLKSEEKPOPENMANAGER_H
