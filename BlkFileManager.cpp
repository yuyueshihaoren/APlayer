// Copyright 2022 Yue Yu
// This file is part of APlayer.
// APlayer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// APlayer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with APlayer. If not, see <https://www.gnu.org/licenses/>.
//
// Created by yuyue on 5/1/22.
//

#include <cstdio>
#include <filesystem>

#include "BlkFileManager.h"


namespace aplayer {
    BlkFileManager::BlkFileManager(const char *path, uint64_t blk_size) : BlkManager(path, blk_size) {
        using namespace std;
        m_file = fopen(m_path, "rb");
        m_size = filesystem::file_size(m_path);
        // m_blk_N is calculated this time
        m_blk_N = m_size / m_blk_size;
        if (m_size % m_blk_size != 0) {
            m_blk_N += 1;
        }
    }

    BlkFileManager::~BlkFileManager() {
        std::fclose(m_file);
    }

    int BlkFileManager::get_blk_ind() {
        return (int) (get_position() / m_blk_size);
    }

    // Should not affect get_position before & after
    uint64_t BlkFileManager::get_blk(int blk_ind) {
        uint64_t  size_to_read = m_blk_size;
        if (blk_ind == m_blk_N - 1) {
            size_to_read = m_size - m_blk_size * (m_blk_N - 1);
        }
        uint64_t block_begin_position = blk_ind * m_blk_size;
        uint64_t orignal_position = std::ftell(m_file);
        // Flush and seek the internal file (not to confuse with the virtual stream in BlkIOMiddleware
        std::fflush(m_file);
        std::fseek(m_file, block_begin_position, SEEK_SET); // TODO: use fseeko or istream, blah
        // Read an entire block
        uint64_t retsize = std::fread(blk_buf, 1, size_to_read, m_file);
        std::fflush(m_file);
        std::fseek(m_file, orignal_position, SEEK_SET); // Go back to original position
        return retsize;
    }

    uint64_t BlkFileManager::get_position() {
        return (uint64_t) std::ftell(m_file); // TODO: use fello or istream, blah
    }

    int BlkFileManager::set_position(uint64_t position) {
        // Corresponds to position at the file
        std::fflush(m_file);
        std::fseek(m_file, position, SEEK_SET);
        return 0;
    }

    void BlkFileManager::flush_all() {
        std::fflush(m_file);
    }


} // aplayer