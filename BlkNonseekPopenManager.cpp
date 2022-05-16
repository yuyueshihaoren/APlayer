// Copyright 2022 Yue Yu
// This file is part of APlayer.
// APlayer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// APlayer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with APlayer. If not, see <https://www.gnu.org/licenses/>.
//
// Created by yuyue on 5/1/22.
//

#include <stdio.h>
#include <filesystem>

#include "Logger.h"
#include "BlkNonseekPopenManager.h"

namespace aplayer {
    BlkNonseekPopenManager::BlkNonseekPopenManager(const char *path, const char *command, uint64_t blk_size)  : BlkManager(path, blk_size), m_position(0) {
        using namespace std;
        m_p_in = popen((string{command} + " " + string{path}).c_str(), "r");

        m_size = filesystem::file_size(m_path);
        // m_blk_N is calculated this time
        m_blk_N = m_size / m_blk_size;
        if (m_size % m_blk_size != 0) {
            m_blk_N += 1;
        }
    }

    // Delegates to the other constructor
    BlkNonseekPopenManager::BlkNonseekPopenManager(const char *path, uint64_t blk_size) :
        BlkNonseekPopenManager(path, "cat", blk_size) {
    }

    BlkNonseekPopenManager::~BlkNonseekPopenManager() {
        pclose(m_p_in);
    }

    int BlkNonseekPopenManager::get_blk_ind() {
        return (int) (get_position() / m_blk_size);
    }

    // Should not affect get_position before & after
    uint64_t BlkNonseekPopenManager::get_blk(int blk_ind) {
        if (m_position == m_size) {
//          // EOF
            return 0;
        }
        if (blk_ind >= m_blk_N) {
            Logger::log(LogLevel::ERROR, "ERROR: BlkNonseekPopenManager::get_blk()",
                        "blk = %d, max_blk = %d", blk_ind, m_blk_N - 1);
        }

        if (blk_ind == last_blk_ind) {
            return last_blk_size;
        } else if (blk_ind < last_blk_ind) {
            throw std::runtime_error("Cannot seekback");
        }

        uint64_t  size_to_read = m_blk_size;
        if (blk_ind == m_blk_N - 1) {
            size_to_read = m_size - m_blk_size * (m_blk_N - 1);
        }
        uint64_t block_begin_position = blk_ind * m_blk_size;

        uint64_t seek_from_cur = block_begin_position - m_position;
        dummy_seek(seek_from_cur);

        // Read an entire block
        uint64_t retsize = std::fread(blk_buf, 1, size_to_read, m_p_in);
        last_blk_ind = blk_ind;
        last_blk_size = retsize;
        return retsize;
    }

    uint64_t BlkNonseekPopenManager::get_position() {
        return m_position;
    }

    int BlkNonseekPopenManager::set_position(uint64_t position) {
        if (position > m_size) {
            Logger::log(LogLevel::ERROR, "ERROR: BlkNonseekPopenManager::set_position()",
                        "Set past! position = %llu, size = %llu, new position = %llu\n",
                        m_position, m_size, position);
        }

        if (position < m_position) {
            throw std::runtime_error("Cannot seekback");
        }
        m_position = position;
        return 0;
    }

    void BlkNonseekPopenManager::flush_all() {
        fflush(NULL);
    }

    void BlkNonseekPopenManager::dummy_seek(uint64_t offset) {
        if (offset == 0) {
            return;
        }
        Logger::logcout(LogLevel::INFO, "BlkNonseekPopenManager::dummy_seek", offset);
        uint64_t seeked = 0;
        while (seeked < offset) {
            uint64_t next_seek = offset - seeked < 64 ? offset - seeked : 64;
            fread(m_dummy_buf, 1, next_seek, m_p_in);
            m_position += next_seek;
            seeked += next_seek;
        }
    }

}