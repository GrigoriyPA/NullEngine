#pragma once

#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "acceleration_context.hpp"

namespace null_engine::multithread::detail {

struct ProgramSource {
    std::string name;
    std::string source;
};

class Program;

class ProgramBuilder {
public:
    static constexpr std::string_view kReplaceOpen = "<|";
    static constexpr std::string_view kReplaceClose = "|>";

    ProgramBuilder(const std::string& module_name, std::string_view source);

    ProgramBuilder& Replace(std::string_view pattern, const std::string& value);

    template <typename T>
    ProgramBuilder& Define(std::string_view name, const T& value) {
        std::stringstream stream;
        stream << value;
        DefineImpl(name, stream.str());
        return *this;
    }

    ProgramBuilder& Include(const Program& program);

    Program Build();

private:
    ProgramBuilder& DefineImpl(std::string_view name, const std::string& value);

    void BuildFinalSource(std::stringstream& final_source) const;

    void ApplyReplaces(std::stringstream& final_source) const;

    std::string module_name_;
    std::string_view source_;
    std::vector<ProgramSource> includes_;
    std::unordered_set<std::string> included_modeles_;
    std::unordered_map<std::string_view, std::string> defines_;
    std::unordered_map<std::string_view, std::string> replaces_;
    bool build_finished_ = false;
};

class Program {
public:
    explicit Program(std::vector<ProgramSource>&& sources);

    Program(const std::string& module_name, std::string_view source);

    const std::vector<ProgramSource>& GetSources() const;

    compute::kernel BuildKernel(const std::string& kernel_name, AccelerationContext context);

private:
    std::vector<ProgramSource> sources_;
    compute::program program_;
};

class ArgsInfo {
    struct Arg {
        std::string_view type;
        std::string_view name;
        bool use_ptr = false;
    };

public:
    ArgsInfo& AddArg(std::string_view type, std::string_view name, bool use_ptr = false);

    std::string GetArgsDefenition() const;

    std::string GetArgsForward() const;

private:
    std::vector<Arg> args_;
};

}  // namespace null_engine::multithread::detail
