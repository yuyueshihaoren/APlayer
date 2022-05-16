// Copyright 2022 Yue Yu
// This file is part of APlayer.
// APlayer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// APlayer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along with APlayer. If not, see <https://www.gnu.org/licenses/>.
#include <mpv/stream_cb.h>
#include <mpv/client.h>
#include <string>
#include <stdexcept>

#include "Logger.h"
#include "argument_parsing.h"
#include "BlkIOMiddleware.h"
#include "BlkManager.h"
#include "BlkSplitManager.h"
#include "BlkFileManager.h"
#include "BlkNonseekPopenManager.h"
#include "BlkSeekPopenManager.h"

using namespace aplayer;

static int64_t size_fn(void *cookie) {
    BlkIOMiddleware *b_io = (BlkIOMiddleware *) cookie;
    int64_t stream_size = b_io->size();
    if (stream_size <= 0)
    {
        return MPV_ERROR_UNSUPPORTED;
    }
    Logger::log(aplayer::LogLevel::INFO, "size_fn()", "stream_size = %ld\n", stream_size);
    return stream_size;
}

static int64_t read_fn(void *cookie, char *buf, uint64_t nbytes)
{
    BlkIOMiddleware *b_io = (BlkIOMiddleware *) cookie;
    Logger::log(LogLevel::INFO, "read_fn()", ",,%lu\n", nbytes);
    try {
        int64_t ret = b_io->read(buf, nbytes); // TODO: Error handling
        return ret;
    } catch (std::out_of_range& e)
    {
        return 0; // EOF
    }
}

static int64_t seek_fn(void *cookie, int64_t offset)
{
    BlkIOMiddleware *b_io = (BlkIOMiddleware *) cookie;

    Logger::log(LogLevel::INFO, "seek_fn()", "offset=%ld\n", offset);

    int r = b_io->seek(offset);
    return r < 0 ? MPV_ERROR_GENERIC : r;
}

// We should not call it. Called by mpv
static void close_fn(void *cookie)
{
    BlkIOMiddleware *b_io = (BlkIOMiddleware *) cookie;

    delete b_io;
}

static int open_fn(void *user_data, char *uri, mpv_stream_cb_info *info)
{
    BlkIOMiddleware *b_io = (BlkIOMiddleware *) user_data;

    info->cookie = (void *) b_io;
    info->size_fn = size_fn;
    info->read_fn = read_fn;
    info->seek_fn = seek_fn;
    info->close_fn = close_fn;

//    return (errcode == 0) ? 0 : MPV_ERROR_LOADING_FAILED;
    return 0;
}

static inline void check_error(int status) {
    if (status < 0) {
        Logger::log(aplayer::LogLevel::FATAL, "check_error()", "mpv API error: %s\n", mpv_error_string(status));
        exit(1);
    }
}

int main(int argc, char **argv) {
    struct arguments arguments = args_init();
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    // Convert arguments to correct C++ types
    std::string mode{arguments.mode};
    LogLevel level = static_cast<LogLevel>(arguments.log_level);

    Logger::init(level);
    // Exit early if dangerous is not properly set
    if ((mode == "custom-popen-command" || mode == "binary-split") && !arguments.dangerous) {
        Logger::log(aplayer::LogLevel::FATAL, "main()", "Call these with --dangerous on");
        return 3;
    }

    if ((mode == "binary-split") && arguments.blk_size == 0) {
        Logger::log(aplayer::LogLevel::FATAL, "main()", "Have to set blk-size (or with -b) when using binary-split mode");
        return 2;
    }
    uint64_t blk_size = 10 * 1024 * 1024; // primary default blk size
    if (arguments.blk_size != 0) {
        blk_size = arguments.blk_size;
    }

    //  mpv stuff
    mpv_handle *mpv_instance = mpv_create();
    if (!mpv_instance) {
        Logger::log(aplayer::LogLevel::FATAL, "main()", "failed creating mpv context");
        return 1;
    }
    // set options
    // default key bindings
    check_error(
            mpv_set_option_string(mpv_instance, "input-default-bindings", "yes"));
    check_error(
            mpv_set_option_string(mpv_instance, "input-vo-keyboard", "yes"));
    // Set properties (before or after initialize (mpv >= 0.21.0))
    int val = 1;
    check_error(
            mpv_set_property(mpv_instance, "osc", MPV_FORMAT_FLAG, &val));
    int back_cache_size = 512 * 1024 * 1024;
    check_error(
//            mpv_set_property(mpv_instance, "demuxer-max-back-bytes", MPV_FORMAT_FLAG, &back_cache_size));
            mpv_set_option_string(mpv_instance, "demuxer-max-back-bytes", std::to_string(back_cache_size).c_str()));
    // Initialize
    check_error(
            mpv_initialize(mpv_instance));
    // mpv log level
    if (arguments.verbose) {
        check_error(
                mpv_request_log_messages(mpv_instance, "v"));
    }

    // Custom stream stuff
    std::string protocol_name = "blkstreamsplit";
    std::string stream_path = arguments.args[0];

    BlkManager *blk_man;
    if (mode == "popen-gpg") {
//        blk_man = new BlkNonseekPopenManager(stream_path.c_str(), "gpg -d", blk_size);
        blk_man = new BlkSeekPopenManager(stream_path.c_str(), "gpg -d", blk_size);
    } else if (mode == "popen-cat") {
//        blk_man = new BlkNonseekPopenManager(stream_path.c_str(), "cat", blk_size);
        blk_man = new BlkSeekPopenManager(stream_path.c_str(), "cat", blk_size);
    } else if (mode == "regular-file") {
        blk_man = new BlkFileManager(stream_path.c_str(), blk_size);
    } else if (mode == "custom-popen-command") {
        // e.g. aplayer --dangerous --mode=custom-popen-command --custom-command=cat ../Videos/BBB22.mp4
//        blk_man = new BlkNonseekPopenManager(stream_path.c_str(), arguments.custom_command, blk_size);
        blk_man = new BlkSeekPopenManager(stream_path.c_str(), arguments.custom_command, blk_size);
    } else if (mode == "binary-split") {
        blk_man = new BlkSplitManager(stream_path.c_str(), blk_size);
    } else {
        Logger::log(aplayer::LogLevel::FATAL, "main()", "invalid mode, run --help to see available modes");
        return 4;
    }

    BlkIOMiddleware *b_io = new BlkIOMiddleware(blk_man);


    std::string s_url = protocol_name + "://" +  stream_path;
    const char* url = s_url.c_str();

    check_error(
            mpv_stream_cb_add_ro(mpv_instance, protocol_name.c_str(), (void *) b_io, open_fn));

    const char *cmd[] = {"loadfile", url, NULL};
    check_error(
            mpv_command(mpv_instance, cmd));

    // Let it play, and wait until the user quits.
    while (1)
    {
        // Wait for an mpv event, set timeout to 10000 seconds (27 hours, basically infinity)
        mpv_event *event = mpv_wait_event(mpv_instance, 10000);

        // Print out the log messages via event
        if (event->event_id == MPV_EVENT_LOG_MESSAGE)
        {
            struct mpv_event_log_message *msg = (struct mpv_event_log_message *)event->data;
            Logger::log(LogLevel::INFO, "main(): mpv event while loop", "[%s] %s: %s", msg->prefix, msg->level, msg->text);
            continue;
        }
        Logger::log(LogLevel::INFO, "main(): mpv event while loop", "event: %s\n", mpv_event_name(event->event_id));

        if (event->event_id == MPV_EVENT_SHUTDOWN)
            break;
    }

    mpv_terminate_destroy(mpv_instance);
    return 0;
}