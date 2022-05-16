// Copyright 2022 Yue Yu
// This file is part of APlayer.
// APlayer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// APlayer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with APlayer. If not, see <https://www.gnu.org/licenses/>.
//
// Created by yuyue on 5/1/22.
//

#ifndef APLAYER_CPP_CLION_BLKIOMIDDLEWARE_H
#define APLAYER_CPP_CLION_BLKIOMIDDLEWARE_H

#include <cstdint>
#include "BlkManager.h"

namespace aplayer {
    // intended to replace BlkStream class
    class BlkIOMiddleware {
    public:
        BlkIOMiddleware() = delete; // no default constructor
        explicit BlkIOMiddleware(BlkManager *blk_manager); // replaces init, open, implementation in cpp
        ~BlkIOMiddleware(); // destructor, implementation in cpp
//        int close();
        uint64_t size();
        uint64_t read(char *buf, uint64_t nbytes);
        int64_t seek(int64_t offset);
    private:
        BlkManager *m_blk_manager;
    };

} // aplayer

#endif //APLAYER_CPP_CLION_BLKIOMIDDLEWARE_H
