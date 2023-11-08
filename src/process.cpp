#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <tuple>
#include <string_view>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <regex>
#define JSON_DIAGNOSTICS 1
#include <nlohmann/json.hpp>

#include "process.hpp"
#include "base64.h"

std::string processCommandLine(const std::string &cmd, resolve_var_t var_tr)
{
    std::smatch sm;
    std::string ret;
    std::regex re("%[A-Za-z_]+[A-Za-z0-9_]*%");
    std::sregex_token_iterator end, iter(cmd.begin(), cmd.end(), re, {-1, 0});
    for(; iter != end; iter++) {
        std::string tmp = *iter;
        
        // Is this a "%VAR%"" variable?
        if (std::regex_match(tmp, sm, re)) {
            std::string res = "", var = tmp.substr(1, tmp.length()-2);
            // Try specific variables from the lambda
            if (var_tr) {
                res = var_tr(var);
            }

            // Try general global variables
            if (res.empty()) {
                //TODO:: GENERAL VARIABLE RESOLUTION
            }

            ret += res;
        } else {
            // Is not a % variable
            ret += tmp;
        }
    }
    
    return ret;
}

ProcessManager::ProcessManager(const std::string &cmd, const char *log_output, resolve_var_t var_tr, std::vector<envvar_t> args)
: log_idx(0), log("")
{
    if (pipe(_stdout) == -1 || pipe(_stdin) == -1) {
        fprintf(stderr, "One ore more pipes failed. Exiting\n");
        exit(-1);
    }

    std::string p_cmd = processCommandLine(cmd, var_tr);
    this->cmd = p_cmd;
    fprintf(stderr, "Running %s.\n", p_cmd.c_str());

    if (log_output)
        log_out_file = fopen(log_output, "w");
    else
        log_out_file = NULL;

    pid = fork();
    if (pid == 0) {
        // Child Process
        close(_stdin[1]);
        close(_stdout[0]);

        dup2(_stdin[0], STDIN_FILENO);
        dup2(_stdout[1], STDOUT_FILENO);
        dup2(_stdout[1], STDERR_FILENO); // redirect stderr to stdout.

        for (envvar_t &var: args) {
            auto name = std::get<0>(var);
            auto value = std::get<1>(var);
            setenv(name.c_str(), value.c_str(), 1);
        }

        char * const argv[] = { 
            strdup("/bin/bash"), 
            strdup("-c"),
            strdup(p_cmd.c_str()),
            NULL
        };

        // Run the process
        execve("/bin/bash", argv, environ);
        // NO RETURN
    }

    // parent process setup
    close(_stdin[0]);
    close(_stdout[1]);

    // We will be querying this from the UI thread (which isn't great...)
    // so we need to set those as NONBLOCK.
    int flags = fcntl(_stdout[0], F_GETFL, 0);
    fcntl(_stdout[0], F_SETFL, flags | O_NONBLOCK);
}

void ProcessManager::DrainPipe()
{
    // Drain stdout into log
    ssize_t sz;
    char buffer[512];
    while ((sz = read(_stdout[0], buffer, sizeof(buffer))) > 0) {
        log.append(buffer, sz);
        
        // Redirect the output of the process to the log file.
        if (log_out_file)
            fwrite(buffer, sz, 1, log_out_file);
    }

    std::string line;
    auto first = log.begin() + log_idx;
    for (auto cur = first; cur < log.end(); cur++) {
        if (*cur == '\n') {
            line = std::string(first, cur+1);
            first = cur+1;

            // This is a tag - decode request and pile it on the queue
            if (line.starts_with("$<") && line.ends_with(">$\n")) {
                std::string data = std::string(line.begin() + 2, line.end() - 3);
                std::string req = base64::decode(data);
                tags.push_back(nlohmann::json::parse(req));
            } else {
                log_show += line;
            }
        }
    }

    log_idx = first - log.begin();
}

ProcessManager::~ProcessManager()
{
    int _;
    if (pid == 0)
        return;

    if (IsRunning(_))
        kill(pid, SIGTERM);

    DrainPipe();
    if (log_out_file)
        fclose(log_out_file);

    close(_stdin[1]);
    close(_stdout[0]);
}

bool ProcessManager::IsRunning(int &exit_code)
{
    pid = waitpid(pid, &status, WNOHANG);
    if (pid == -1) {
        return false;
    } else if (pid != 0) {
        if (WIFEXITED(status)) {
            exit_code = WEXITSTATUS(status);
            fprintf(stderr, "child exited with code %d.\n", WEXITSTATUS(status));
        } else {
            exit_code = EXIT_FAILURE;
            fprintf(stderr, "child did not exit successfully.\n");
        }

        return false;
    }

    return true;
}

const std::string &ProcessManager::GetLines()
{
    return log_show;
}

const std::string &ProcessManager::GetLogFinal()
{
    return log;
}

void ProcessManager::Reply(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    vdprintf(_stdin[1], fmt, va);
    va_end(va);
}