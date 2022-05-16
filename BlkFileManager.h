// Copyright 2022 Yue Yu
// This file is part of APlayer.
// APlayer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// APlayer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with APlayer. If not, see <https://www.gnu.org/licenses/>.
//
// Created by yuyue on 5/1/22.
//

#ifndef APLAYER_CPP_CLION_BLKFILEMANAGER_H
#define APLAYER_CPP_CLION_BLKFILEMANAGER_H

#include <cstdio>
#include "BlkManager.h"


namespace aplayer {

    class BlkFileManager: public BlkManager {
    public:
        BlkFileManager(const char *path, uint64_t blk_size);
        ~BlkFileManager();
        int get_blk_ind() override;
        uint64_t get_blk(int blk_ind) override;
        uint64_t get_position() override;
        int set_position(uint64_t position) override;
        void flush_all() override;
    private:
        FILE * m_file;
    };

} // aplayer

#endif //APLAYER_CPP_CLION_BLKFILEMANAGER_H
