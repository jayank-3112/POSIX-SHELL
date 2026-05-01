#include "ls.cpp"
#include "tokenize.cpp"
#include "execvp.cpp"
#include "search.cpp"
#include "current_dir.h"
#include "echo.cpp"
#include "cd.cpp"
#include "multiple_commands.cpp"

#include <csignal>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

using namespace std;

// ─── Shell History ────────────────────────────────────────────────────────────

class ShellHistory {
public:
    ShellHistory() : history_index(-1) {
        load();
    }

    ~ShellHistory() {
        save();
    }

    void store(const string& input) {
        if (history.size() >= MAX_SIZE)
            history.erase(history.begin());
        history.push_back(input);
        history_index = static_cast<int>(history.size()) - 1;
    }

    void display(int num) const {
        if (num <= 0 || static_cast<size_t>(num) > history.size()) {
            cout << "Invalid history count\n";
            return;
        }
        int start = static_cast<int>(history.size()) - num;
        for (int i = start; i < static_cast<int>(history.size()); ++i)
            cout << history[i] << "\n";
    }

    void previous_command() {
        if (history.empty()) return;
        if (history_index < 0)
            history_index = static_cast<int>(history.size()) - 1;
        else if (history_index > 0)
            --history_index;
        cout << history[history_index] << "\n";
    }

    int history_index;

private:
    static constexpr int MAX_SIZE    = 20;
    static constexpr int MAX_DISPLAY = 10;
    static constexpr const char* FILE_NAME = "history.txt";

    vector<string> history;

    void load() {
        ifstream file(FILE_NAME);
        string line;
        while (getline(file, line))
            history.push_back(line);
    }

    void save() const {
        ofstream file(FILE_NAME);
        for (const auto& entry : history)
            file << entry << "\n";
    }
};

// ─── Helpers ──────────────────────────────────────────────────────────────────

static string get_hostname() {
    char buf[256];
    if (gethostname(buf, sizeof(buf)) == -1) {
        perror("gethostname");
        return "";
    }
    return buf;
}

static void print_prompt(const char* username, const string& home_dir) {
    string path = get_current_dir();
    cout << username << "@" << get_hostname() << ":";
    if (path.size() > home_dir.size())
        cout << "~/" << path.substr(home_dir.size() + 1);
    else
        cout << "~";
    cout << "> ";
}

// ─── Signal Handlers ──────────────────────────────────────────────────────────

static pid_t foreground_pid = -1;

static void handle_sigtstp(int) {
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGSTOP);
        foreground_pid = -1;
    }
}

static void handle_sigint(int) {
    if (foreground_pid > 0) {
        kill(foreground_pid, SIGINT);
        foreground_pid = -1;
    }
}

// ─── Command Dispatch ─────────────────────────────────────────────────────────

static bool dispatch(const vector<string>& arg, const string& raw_input,
                     const string& home_dir, ShellHistory& sh, bool background) {
    if (arg.empty()) return true;

    const string& cmd = arg[0];

    if (cmd == "exit") {
        cout << "Exiting the shell\n";
        return false;
    }
    if (cmd == "ls") {
        my_ls(arg);
    } else if (cmd == "cd") {
        if (arg.size() == 1 || (arg.size() > 1 && arg[1] == ".." &&
                get_current_dir() == home_dir)) {
            change_dir(arg, home_dir);
        } else {
            change_dir(arg, home_dir);
        }
    } else if (cmd == "echo") {
        my_echo(raw_input);
    } else if (cmd == "pwd") {
        cout << get_current_dir() << "\n";
    } else if (cmd == "search") {
        cout << (search(arg) ? "true" : "false") << "\n";
    } else if (cmd == "history") {
        int count = (arg.size() > 1) ? stoi(arg[1]) : 10;
        sh.display(count);
    } else {
        execute_command(arg, background);
    }

    return true;
}

// ─── Main ─────────────────────────────────────────────────────────────────────

int main() {
    const char* username = getenv("USER");
    if (!username) username = "user";

    const string home_dir = get_current_dir();
    ShellHistory sh;

    signal(SIGTSTP, handle_sigtstp);
    signal(SIGINT,  handle_sigint);

    string input;
    while (true) {
        print_prompt(username, home_dir);

        if (!getline(cin, input)) {
            cout << "\nCtrl+D pressed — exiting the shell\n";
            break;
        }

        if (input.empty()) continue;

        sh.store(input);

        vector<string> commands = command(input);
        bool keep_running = true;

        for (const auto& cmd_str : commands) {
            vector<string> arg = my_string_tokenizer(cmd_str.c_str());
            if (arg.empty()) continue;

            bool background = false;
            if (arg.back() == "&") {
                background = true;
                arg.pop_back();
            }

            keep_running = dispatch(arg, input, home_dir, sh, background);
            if (!keep_running) break;
        }

        if (!keep_running) break;
    }

    return 0;
}
