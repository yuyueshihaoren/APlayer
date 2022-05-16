// Copyright 2022 Yue Yu
// This file is part of APlayer.
// APlayer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// APlayer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with APlayer. If not, see <https://www.gnu.org/licenses/>.
//
// Created by yuyue on 5/1/22.
//
#include <cstring>
#include <cassert>

#include "BlkIOMiddleware.h"
#include "Logger.h"

namespace aplayer {
    // Constructor, specify the blk manager
    BlkIOMiddleware::BlkIOMiddleware(BlkManager *blk_manager): m_blk_manager(blk_manager) {
        // What else?
    }
    // Destructor
    BlkIOMiddleware::~BlkIOMiddleware() {
        delete m_blk_manager;
    }

    uint64_t BlkIOMiddleware::size() {
        uint64_t retszie = m_blk_manager->get_size();
        Logger::log(LogLevel::INFO, "BlkIOMiddleware::size()", "size = %llu\n", retszie);
        return retszie;
    }

    uint64_t BlkIOMiddleware::read(char *buf, uint64_t nbytes) {
        uint64_t  size_read = 0;
        uint64_t  size_next = 0;
        Logger::log(LogLevel::INFO, "BlkIOMiddleware::read", "access nbytes: %lu\n", nbytes);

        while ((nbytes - size_read) > 0) {
            size_next = std::min(nbytes - size_read, m_blk_manager->get_blk_size() - m_blk_manager->get_position() % m_blk_manager->get_blk_size());
            size_next = std::min(size_next, m_blk_manager->get_size() - m_blk_manager->get_position());
            // Read the block
            int retsize = m_blk_manager->get_blk(m_blk_manager->get_blk_ind());
            if (retsize > 0) { // copy to buffer
                std::memcpy(buf, m_blk_manager->blk_buf + (m_blk_manager->get_position() % m_blk_manager->get_blk_size()), size_next);
                buf += size_next;
                m_blk_manager->set_position(m_blk_manager->get_position() + size_next); // m_position += size_next;
                size_read += size_next;
            } else { // throw what error?
                Logger::log(LogLevel::INFO, "BlkIOMiddleware::read NEGATIVE", "retsize = %d\n", retsize);
                Logger::log(LogLevel::INFO, "BlkIOMiddleware::read NEGATIVE", "size - position = %ld\n", (int64_t) m_blk_manager->get_size() - (int64_t) m_blk_manager->get_position());
                if (m_blk_manager->get_position() == m_blk_manager->get_size()) {
                    Logger::log(LogLevel::INFO, "BlkIOMiddleware::read", "EOF\n");
                    throw std::out_of_range("EOF"); // should be handled by main
                } else {
                    Logger::log(LogLevel::ERROR, "BlkIOMiddleware::read", "Read Error, not EOF\n");
                    throw std::runtime_error("Read Error"); // should be handled by main
                }
            }
        }

        return size_read;
    }

    int64_t BlkIOMiddleware::seek(int64_t offset) {

        if (! m_blk_manager->seekable()) {
            return -1;
        }

        Logger::log(LogLevel::INFO, "BlkIOMiddleware::seek()", "offset:%lu\n", offset);
        // SEEK_SET
        // Edge cases
        if (offset < 0) {
            Logger::log(LogLevel::ERROR, "BlkIOMiddleware::seek()", "Seek past 0\n");
            return -1;
        }
        if (offset > m_blk_manager->get_size()) {
            Logger::log(LogLevel::ERROR, "BlkIOMiddleware::seek()", "Seek past stream\n");
            return -1;
        }
        // Flush the streams
        m_blk_manager->flush_all();
        // Actual seek
        m_blk_manager->set_position(offset);
        return 0;
    }
} // aplayer