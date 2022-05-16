// Copyright 2022 Yue Yu
// This file is part of APlayer.
// APlayer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// APlayer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with APlayer. If not, see <https://www.gnu.org/licenses/>.
//
// Created by yuyue on 4/30/22.
//
#include <cstdio>
#include <cstdarg>

#include "Logger.h"

namespace aplayer {
    Logger::Logger() {}

    // don't repease static keyword
    Logger &Logger::instance() {
        static Logger *instance = new Logger();
        return *instance;
    }

    Logger &Logger::init(LogLevel level) {
        Logger &instance = Logger::instance();
        instance._level = level;
        return instance;
    }

    void Logger::log(LogLevel level, const char *prefix, const char *fmt, ...) {
        std::va_list args;
        va_start(args, fmt);

        if (level <= Logger::instance()._level) {
            std::printf("[%s]: ", prefix);
            std::vprintf(fmt, args);
        }
        std::fflush(stdout);
    }


} // aplayer