// Copyright 2022 Yue Yu
// This file is part of APlayer.
// APlayer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// APlayer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with APlayer. If not, see <https://www.gnu.org/licenses/>.
#ifndef APLAYER_CPP_CLION_LOGGER_H
#define APLAYER_CPP_CLION_LOGGER_H

#include <cstdio>
#include <iostream>

namespace aplayer {
    enum class LogLevel {
        NONE = 0,   /// "no"    - disable absolutely all messages
        FATAL = 10, /// "fatal" - critical/aborting errors
        ERROR = 20, /// "error" - simple errors
        WARN = 30,  /// "warn"  - possible problems
        INFO = 40,  /// "info"  - informational message
        V = 50,     /// "v"     - noisy informational message
        DEBUG = 60, /// "debug" - very noisy technical information
        TRACE = 70, /// "trace" - extremely noisy
    };

    class Logger {
    public:
        static Logger &instance();

        static Logger &init(LogLevel level);

        static void log(LogLevel level, const char *prefix, const char *fmt, ...);

        template<typename T>
        static void logcout(LogLevel level, const char *prefix, const T& obj) {
            if (level <= Logger::instance()._level) {
                std::cout << prefix << obj << std::endl;
            }
            std::fflush(stdout);
        }

        template<typename T>
        static void logcout(const char *prefix, const T& obj) {
            LogLevel level = LogLevel::INFO;
            if (level <= Logger::instance()._level) {
                std::cout << prefix << obj << std::endl;
            }
            std::fflush(stdout);
        }
    private:
        Logger();

        LogLevel _level;
    };

} // aplayer

#endif //APLAYER_CPP_CLION_LOGGER_H
