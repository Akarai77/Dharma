#pragma once
#include <string>
#include <vector>
#include <sstream>

class SourceManager {
private:
    std::vector<std::string> sourceLines;

    SourceManager() = default;

public:
    SourceManager(const SourceManager&) = delete;
    SourceManager& operator=(const SourceManager&) = delete;

    static SourceManager& instance() {
        static SourceManager instance;
        return instance;
    }

    void setSource(const std::string& source) {
        sourceLines.clear();
        std::stringstream ss(source);
        std::string line;
        while (std::getline(ss, line)) {
            sourceLines.push_back(line);
        }
    }

    const std::string& getLine(size_t line) const {
        if (line == 0 || line > sourceLines.size())
            throw std::out_of_range("Invalid line number");
        return sourceLines[line - 1];
    }
};

