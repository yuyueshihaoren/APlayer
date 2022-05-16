// Copyright 2022 Yue Yu
// This file is part of APlayer.
// APlayer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// APlayer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with APlayer. If not, see <https://www.gnu.org/licenses/>.
//
// Created by yuyue on 5/1/22.
//

#ifndef APLAYER_CPP_CLION_BLKMANAGER_H
#define APLAYER_CPP_CLION_BLKMANAGER_H

#include <cstdint>

namespace aplayer {
    // Base Class
    class BlkManager {
    public:
        BlkManager(const char *path, uint64_t blk_size):
                m_path(path), m_blk_size(blk_size) {
            blk_buf = new char [m_blk_size];
        }

        virtual ~BlkManager() {
            delete[] blk_buf;
        }

        virtual int get_blk_ind() = 0;
        virtual uint64_t get_blk(int blk_ind) = 0;
        virtual uint64_t get_size() {
            return m_size;
        }
        virtual uint64_t get_blk_size() {
            return m_blk_size;
        }
        virtual uint64_t get_position() = 0;
        virtual int set_position(uint64_t position) = 0;
        virtual void flush_all() = 0;
        // is_open ?
//        virtual uint64_t get_blk_N() {
//            return m_blk_N;
//        }
        char *blk_buf;
        virtual bool seekable() {
            return true;
        }

    protected:
//        uint64_t m_position;
        const char *m_path;
//        bool _open; ?
        uint64_t m_blk_size;
        int m_blk_N;
        uint64_t m_size {0}; // default value = 0
        // TODO: cache lines
    };

} // aplayer

#endif //APLAYER_CPP_CLION_BLKMANAGER_H
