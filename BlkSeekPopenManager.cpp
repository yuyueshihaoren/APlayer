// Copyright 2022 Yue Yu
// This file is part of APlayer.
// APlayer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// APlayer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with APlayer. If not, see <https://www.gnu.org/licenses/>.
//
// Created by yuyue on 5/3/22.
//
#include <stdio.h>
#include <filesystem>

#include "Logger.h"

#include "BlkSeekPopenManager.h"


namespace aplayer {
    const std::string BlkSeekPopenManager::_escape_path()
    {
        return "\"" + std::filesystem::path{m_path}.string() + "\"";
    }

    BlkSeekPopenManager::BlkSeekPopenManager(const char *path, const char *command, uint64_t blk_size)
            : BlkManager(path, blk_size), m_position(0), m_command(command), m_pos_ftell(0) {
        using namespace std;
        m_p_in = popen((string{m_command} + " " + _escape_path()).c_str(), "r");

        m_size = filesystem::file_size(m_path);
        // m_blk_N is calculated this time
        m_blk_N = m_size / m_blk_size;
        if (m_size % m_blk_size != 0) {
            m_blk_N += 1;
        }
    }

    // Delegates to the other constructor
    BlkSeekPopenManager::BlkSeekPopenManager(const char *path, uint64_t blk_size) :
            BlkSeekPopenManager(path, "cat", blk_size) {
    }

    BlkSeekPopenManager::~BlkSeekPopenManager() {
        pclose(m_p_in);
    }

    int BlkSeekPopenManager::get_blk_ind() {
        return (int) (m_position / m_blk_size);
    }

    // Something different
    uint64_t BlkSeekPopenManager::get_blk(int blk_ind) {
        if (m_position == m_size) { //EOF
            return 0;
        }
        if (blk_ind >= m_blk_N) {
            Logger::log(LogLevel::ERROR, "ERROR: BlkSeekPopenManager::get_blk()", "blk = %d, max_blk = %d", blk_ind,
                        m_blk_N - 1);
        }
        if (blk_ind == last_blk_ind) {
            return last_blk_size;
        }

        uint64_t size_to_read = m_blk_size;
        if (blk_ind == m_blk_N - 1) {
            size_to_read = m_size - m_blk_size * (m_blk_N - 1);
        }
        uint64_t block_begin_position = blk_ind * m_blk_size;
        ftell_dummy_seek_set(block_begin_position); // skip, or "rewind" and skip

        uint64_t retsize = std::fread(blk_buf, 1, size_to_read, m_p_in);
        m_pos_ftell += retsize; // position of underlying data

        last_blk_ind = blk_ind;
        last_blk_size = retsize;
        return retsize;
    }

    uint64_t BlkSeekPopenManager::get_position() {
        return m_position;
    }

    // Don't touch m_pos_ftell, don't dummy seek
    // Underlying data not actually seeked until next get_blk
    int BlkSeekPopenManager::set_position(uint64_t position) {
        if (position > m_size) {
            Logger::log(LogLevel::ERROR, "ERROR: BlkNonseekPopenManager::set_position()",
                        "Set past! position = %llu, size = %llu, new position = %llu\n",
                        m_position, m_size, position);
        }
        m_position = position;
        return 0;
    }

    void BlkSeekPopenManager::flush_all() {
        fflush(NULL);
    }

    // Update ftell_pos, only seek to beginning of a block
    void BlkSeekPopenManager::ftell_dummy_seek_set(uint64_t offset) {
        int seek_blk_ind = offset / m_blk_size;
        uint64_t pos_new = seek_blk_ind * m_blk_size; // at a block head

        if (pos_new < m_pos_ftell) {
            // Rewind, (then seek forward in next if block)
            fflush(NULL);
            pclose(m_p_in);
            m_p_in = popen((std::string{m_command} + " " + _escape_path()).c_str(), "r");
            m_pos_ftell = 0;
        }

        if (pos_new > m_pos_ftell) {
            // seek forward
            uint64_t size_to_seek = pos_new - m_pos_ftell;
            uint64_t seeked = 0;
            fflush(NULL);
            while (seeked < size_to_seek) {
                uint64_t next_seek = offset - seeked < 64 ? offset - seeked : 64;
                fread(m_dummy_buf, 1, next_seek, m_p_in);
                m_pos_ftell += next_seek;
                seeked += next_seek;
            }
            fflush(NULL);
        }
    }


} // aplayer