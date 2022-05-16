// Copyright 2022 Yue Yu
// This file is part of APlayer.
// APlayer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// APlayer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with APlayer. If not, see <https://www.gnu.org/licenses/>.
#include "BlkSplitManager.h"

#include <filesystem>
#include "Logger.h"
#include <algorithm>
#include <stdexcept>

namespace aplayer {
    // Constructor replaces what open() used to do
    BlkSplitManager::BlkSplitManager(const char *path, uint64_t blk_size) : BlkManager(path, blk_size ), _position(0) {
        using namespace std;
        const filesystem::path dir_path{m_path}; // convert path to C++ path class

        vector<filesystem::directory_entry> dir_ents; // vector storing all the entries
        // Appending everything in
        for (const filesystem::directory_entry &dir_ent:
                filesystem::directory_iterator{dir_path}) {

            Logger::logcout(LogLevel::INFO, "BlkstreamSplit::open() dir_ent=", dir_ent.path().c_str());

            if (dir_ent.path().filename().c_str()[0] == 'x') {
                dir_ents.push_back(dir_ent); // stores the full path
            }
        }

        m_blk_N = dir_ents.size();
        Logger::logcout("BlkstreamSplit::open() m_blk_N = ", m_blk_N);
        if (m_blk_N > 0) {
            // Sort the vector alphabetically
            sort(dir_ents.begin(), dir_ents.end(),
                 [](filesystem::directory_entry &a, filesystem::directory_entry &b) {
//                     return a.path() < b.path(); // C++ 20
                     return a.path().string() < b.path().string();
                 });

            // Open each file, fill _split_files
            for (auto &dir_ent: dir_ents) {
                // update size
                m_size += dir_ent.file_size();

                FILE *temp_file = fopen(dir_ent.path().c_str(), "rb");
                Logger::log(LogLevel::INFO, "BlkstreamSplit::open()", "filename = %s\t\t fileno = %d\n",
                            dir_ent.path().c_str(), fileno(temp_file));

                _split_files.push_back(temp_file);
            }
        } else {
            // Vector size 0, no files found
            Logger::log(LogLevel::FATAL, "BlkstreamSplit::open()", "open %s failed\n", m_path);
            throw std::invalid_argument("Invalid path");
        }
    }

    BlkSplitManager::~BlkSplitManager() {
        // base destructor is always called last
        // close all the files
        for (auto &f: _split_files) {
            std::fclose(f);
        }
        _split_files.clear();
        _split_files.shrink_to_fit();
    }

    int BlkSplitManager::get_blk_ind() {
        return (int) (_position / m_blk_size);
    }

    uint64_t BlkSplitManager::get_blk(int blk_ind) {
        Logger::log(LogLevel::INFO, "get_blk", "blk_ind %d\n", blk_ind);
        uint64_t size_to_read = m_blk_size;
        if (blk_ind == m_blk_N - 1) { // tail
            size_to_read = m_size - m_blk_size * (m_blk_N - 1);
        }

        std::rewind(_split_files[blk_ind]);
        uint64_t retsize = std::fread(blk_buf, sizeof(char), size_to_read, _split_files[blk_ind]);
        return retsize;
    }

    void BlkSplitManager::flush_all() {
        for (auto sfp : _split_files) {
            std::fflush(sfp);
        }
    }

    uint64_t BlkSplitManager::get_position() {
        return _position;
    }

    int BlkSplitManager::set_position(uint64_t position) {
        _position = position;
        return 0;
    }
}