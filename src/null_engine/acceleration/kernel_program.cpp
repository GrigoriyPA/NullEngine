#include "kernel_program.hpp"

#include <fmt/format.h>

#include <null_engine/util/generic/validation.hpp>
#include <sstream>
#include <string_view>

#include "helpers.hpp"

namespace null_engine::multithread::detail {

ProgramBuilder::ProgramBuilder(const std::string& module_name, std::string_view source)
    : module_name_(module_name)
    , source_(source) {
}

ProgramBuilder& ProgramBuilder::Replace(std::string_view pattern, const std::string& value) {
    assert(!build_finished_ && "Can not add replace after finish build");

    const auto [_, inserted] = replaces_.emplace(pattern, value);
    assert(inserted && "Got duplicated replace pattern");

    return *this;
}

ProgramBuilder& ProgramBuilder::Define(std::string_view name, const std::string& value) {
    assert(!build_finished_ && "Can not add define after finish build");

    const auto [_, inserted] = defines_.emplace(name, value);
    assert(inserted && "Got duplicated define name");

    return *this;
}

ProgramBuilder& ProgramBuilder::Include(const Program& program) {
    assert(!build_finished_ && "Can not add include after finish build");

    for (const auto& [name, source] : program.GetSources()) {
        if (included_modeles_.emplace(name).second) {
            includes_.emplace_back(name, source);
        }
    }

    return *this;
}

Program ProgramBuilder::Build() {
    assert(!build_finished_ && "Can not build program twice");
    build_finished_ = true;

    std::stringstream final_source;
    BuildFinalSource(final_source);
    includes_.emplace_back(module_name_, final_source.str());

    return Program(std::move(includes_));
}

void ProgramBuilder::BuildFinalSource(std::stringstream& final_source) const {
    for (const auto& [name, value] : defines_) {
        final_source << "#define " << name << " " << value << "\n";
    }

    ApplyReplaces(final_source);
    final_source << "\n";

    for (const auto& [name, _] : defines_) {
        final_source << "#undef " << name << "\n";
    }
}

void ProgramBuilder::ApplyReplaces(std::stringstream& final_source) const {
    std::unordered_set<std::string_view> used_patterns;
    used_patterns.reserve(replaces_.size());

    int32_t open_pos = source_.find(kReplaceOpen);
    int32_t close_pos = -static_cast<int32_t>(kReplaceClose.size());
    for (; open_pos != std::string_view::npos; open_pos = source_.find(kReplaceOpen, open_pos + kReplaceOpen.size())) {
        const int32_t last_pos = close_pos + kReplaceClose.size();
        Ensure(open_pos >= last_pos, "Invalid string for replaces, found two opened brackets before close");

        close_pos = source_.find(kReplaceClose, close_pos + kReplaceClose.size());
        Ensure(close_pos != std::string_view::npos, "Invalid string for replaces, can not find close bracket");
        Ensure(open_pos < close_pos, "Invalid string for replaces, found two closed brackets before open");

        const std::string_view pattern =
            source_.substr(open_pos + kReplaceOpen.size(), close_pos - open_pos - kReplaceOpen.size());
        const auto it = replaces_.find(pattern);
        Ensure(it != replaces_.end(), fmt::format("Failed to find substitution for pattern '{}'", pattern));

        used_patterns.emplace(pattern);
        final_source << source_.substr(last_pos, open_pos - last_pos) << it->second;
    }
    Ensure(used_patterns.size() == replaces_.size(), "Found unused replaces");

    if (const int32_t last_pos = close_pos + kReplaceClose.size(); last_pos < source_.size()) {
        final_source << source_.substr(last_pos, source_.size() - last_pos);
    }
}

Program::Program(std::vector<ProgramSource>&& sources)
    : sources_(std::move(sources)) {
}

Program::Program(const std::string& module_name, std::string_view source)
    : sources_(1, {.name = module_name, .source = std::string(source)}) {
}

const std::vector<ProgramSource>& Program::GetSources() const {
    return sources_;
}

compute::kernel Program::BuildKernel(const std::string& kernel_name, AccelerationContext context) {
    std::stringstream composed_source;
    for (const auto& [_, source] : sources_) {
        composed_source << source << "\n";
    }

    program_ = compute::program::create_with_source(composed_source.str(), context.GetContext());
    BuildProgram(program_);

    return compute::kernel(program_, kernel_name);
}

}  // namespace null_engine::multithread::detail
