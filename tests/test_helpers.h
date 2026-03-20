#ifndef BANEWFN_TEST_HELPERS_H
#define BANEWFN_TEST_HELPERS_H

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>

namespace fs = std::filesystem;

inline void writeTextFile(const fs::path& path, const std::string& contents) {
    if (!path.parent_path().empty()) {
        std::error_code ec;
        fs::create_directories(path.parent_path(), ec);
    }

    std::ofstream out(path);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot write file: " + path.string());
    }
    out << contents;
}

inline std::string readTextFile(const fs::path& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot read file: " + path.string());
    }

    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

class TempDir {
public:
    TempDir() : oldCwd_(fs::current_path()) {
        const fs::path base = fs::temp_directory_path();

        for (int attempt = 0; attempt < 128; ++attempt) {
            const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
            fs::path candidate = base / ("banewfn_test_" + std::to_string(stamp) + "_" + std::to_string(attempt));

            std::error_code ec;
            if (fs::create_directories(candidate, ec)) {
                path_ = candidate;
                break;
            }
        }

        if (path_.empty()) {
            throw std::runtime_error("Failed to create temporary test directory");
        }
    }

    TempDir(const TempDir&) = delete;
    TempDir& operator=(const TempDir&) = delete;

    ~TempDir() {
        std::error_code ec;
        fs::current_path(oldCwd_, ec);
        fs::remove_all(path_, ec);
    }

    void chdir() const {
        fs::current_path(path_);
    }

    const fs::path& path() const {
        return path_;
    }

private:
    fs::path oldCwd_;
    fs::path path_;
};

class EnvVarGuard {
public:
    EnvVarGuard(const std::string& name, const std::string& value) : name_(name) {
        if (const char* previous = std::getenv(name.c_str())) {
            oldValue_ = std::string(previous);
        }
        set(value);
    }

    EnvVarGuard(const EnvVarGuard&) = delete;
    EnvVarGuard& operator=(const EnvVarGuard&) = delete;

    ~EnvVarGuard() {
        if (oldValue_.has_value()) {
            set(*oldValue_);
        } else {
            unset();
        }
    }

private:
    void set(const std::string& value) {
#ifdef _WIN32
        _putenv_s(name_.c_str(), value.c_str());
#else
        setenv(name_.c_str(), value.c_str(), 1);
#endif
    }

    void unset() {
#ifdef _WIN32
        _putenv_s(name_.c_str(), "");
#else
        unsetenv(name_.c_str());
#endif
    }

    std::string name_;
    std::optional<std::string> oldValue_;
};

#endif // BANEWFN_TEST_HELPERS_H
