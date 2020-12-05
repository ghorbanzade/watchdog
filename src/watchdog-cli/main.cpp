/**
 * Copyright 2020 Pejman Ghorbanzade. All rights reserved.
 */

#include "cxxopts.hpp"
#include "fmt/format.h"
#include "spdlog/spdlog.h"
#include "watchdog/named_pipe_message.hpp"
#include "watchdog/named_pipe_writer.hpp"
#include "watchdog/watchdog.hpp"
#include <iostream>
#include <optional>

/**
 *
 */
std::optional<cxxopts::ParseResult> parse_command_line_arguments(int argc, char* argv[])
{
    cxxopts::Options options(argv[0], "Command Line Tool for Monitoring Filesystem Events");

    options
        .positional_help("[optional args]")
        .show_positional_help();

    // clang-format off
    options.add_options()
        ("a,add", "add a given directory to the watch-list", cxxopts::value<std::vector<std::string>>())
        ("c,clear", "clear the watch-list", cxxopts::value<bool>())
        ("l,list", "list directories on the watch-list", cxxopts::value<bool>())
        ("m,monitor", "prints, in real-time, filesystem events occuring in any directory on the watch-list", cxxopts::value<bool>())
        ("r,remove", "remove a given directory from watch-list", cxxopts::value<std::vector<std::string>>())
        ("help", "prints all supported command line options");
    // clang-format on

    // We expect that regular users know all supported options of this tool and
    // use the option `--add` when trying to put contents of a given directory
    // under observation. However, new impatient users may simply pass a list
    // of directories as command line arguments and expect that watchdog starts
    // observing them. To provide a better user experience, make `add` support
    // positional arguments.

    options.parse_positional({ "add" });

    try
    {
        const auto result = options.parse(argc, argv);

        // If user asks for help, print the help message and return `result`
        // instead of `nullopt` to signal to the calling function that we
        // shoud return `EXIT_SUCCESS`.

        if (result.count("help"))
        {
            std::cout << options.help({ "", "watchdog" }) << std::endl;
            return result;
        }

        // Watchdog command line options are all mutually exclusive. We expect
        // user to provide at least one option. In the event that they provide
        // several options at the same time or no option at all, print the help
        // message and exit.

        const auto& opts = { "add", "clear", "list", "monitor", "remove" };
        const auto opt_exists = [&](const auto& opt) { return result.count(opt); };
        const auto opts_count = std::count_if(opts.begin(), opts.end(), opt_exists);
        if (1 < opts_count)
        {
            std::cerr << "Please provide one and only one option a time.\n";
        }
        if (1 != opts_count)
        {
            std::cout << options.help({ "", "watchdog" }) << std::endl;
            return std::nullopt;
        }

        return result;
    }
    catch (const cxxopts::OptionParseException& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        std::cout << options.help({ "", "watchdog" }) << std::endl;
        return std::nullopt;
    }
}

/**
 *
 */
std::vector<std::string> validate_path_arguments(
    const std::vector<std::string>& args)
{
    std::vector<std::string> output;
    for (const auto& arg : args)
    {
        const auto& dir = std::filesystem::canonical(arg).string();
        if (!std::filesystem::exists(dir))
        {
            spdlog::warn("directory {} is missing", arg);
            continue;
        }
        if (!std::filesystem::is_directory(dir))
        {
            spdlog::warn("file {} is not a directory", arg);
            continue;
        }
        output.push_back(dir);
    }
    return output;
}

/**
 *
 */
int main(int argc, char* argv[])
{
    const auto& args = parse_command_line_arguments(argc, argv);

    if (!args)
    {
        return EXIT_FAILURE;
    }

    NamedPipeWriter writer("/tmp/watchdog/sin");

    if (args->count("clear"))
    {
        writer.write("c");
        spdlog::info("cleared the list of directories under watch");
        return EXIT_SUCCESS;
    }

    if (args->count("add"))
    {
        const auto path_args = args.value()["add"].as<std::vector<std::string>>();
        const auto& dirs = validate_path_arguments(path_args);
        for (const auto& dir : dirs)
        {
            writer.write(fmt::format("a,{}", dir));
            spdlog::info("added directory {} to the watch-list", dir);
        }
        return EXIT_SUCCESS;
    }

    if (args->count("remove"))
    {
        const auto& path_args = args.value()["remove"].as<std::vector<std::string>>();
        const auto& dirs = validate_path_arguments(path_args);
        for (const auto& dir : dirs)
        {
            writer.write(fmt::format("r,{}", dir));
            spdlog::info("removed directory {} from the watch-list", dir);
        }
        return EXIT_SUCCESS;
    }

    if (args->count("list"))
    {
        writer.write("l");
        return EXIT_SUCCESS;
    }

    if (args->count("monitor"))
    {
        writer.write("m");
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
