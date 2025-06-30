#pragma once

#include "includes.h"

namespace fs = std::filesystem;

class ScoreStorage {
public:
    explicit ScoreStorage(const fs::path& file) : file_(file) {
        if (!fs::exists(file_.parent_path())) fs::create_directories(file_.parent_path());
        if (!fs::exists(file_)) {
            std::ofstream(file_).close();
        }
    }

    void save(const std::string& name, unsigned score) {
        std::ofstream out(file_, std::ios::app);
        out << name << " " << score << '\n';
    }

    std::vector<std::pair<std::string, unsigned>> loadTop(std::size_t max = 10) const {
        std::ifstream in(file_);
        std::vector<std::pair<std::string, unsigned>> v;
        std::string name;
        unsigned x;
        while (in >> name >> x)
            v.emplace_back(name, x);

        std::ranges::sort(v, std::greater{}, &std::pair<std::string, unsigned>::second);

        if (v.size() > max) v.resize(max);
        return v;
    }

private:
    fs::path file_;
};