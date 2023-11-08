#pragma once

#define JSON_DIAGNOSTICS 1
#include <nlohmann/json.hpp>

using envvar_t = std::tuple<std::string, std::string>;
using resolve_var_t = std::function<std::string(const std::string&)>;
class ProcessManager
{
    int status;
    pid_t pid;
    size_t log_idx;
    std::string log;
    std::string cmd;
    std::string log_show;
    FILE *log_out_file;
public:
    int _stdin[2];
    int _stdout[2];

    ProcessManager(const std::string &cmd, const char *log_output, resolve_var_t var_tr, std::vector<envvar_t> args);
    ~ProcessManager();

    void DrainPipe();
    bool IsRunning(int &exit_code);
    void Reply(const char *fmt, ...);

    const std::string &GetLines();
    const std::string &GetLogFinal();

    std::vector<nlohmann::json> tags;
};

std::string processCommandLine(const std::string &cmd, resolve_var_t var_tr);
