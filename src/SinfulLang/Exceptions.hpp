#pragma once

#include "sinful/pch.hpp"

namespace Sinful::Exceptions
{
    struct SourceLocation
    {
        std::string filename;
        size_t line;
        size_t column;

        bool operator==(SourceLocation const& rhs) const { return filename == rhs.filename 
            && line == rhs.line 
            && column == rhs.column; }
    };

    struct Diagnostic
    {
        enum class Level { Error, Warning, Note };

        Level level;
        SourceLocation location;
        std::string message;
    };

    class CompilerException : public std::exception
    {
    public:

        CompilerException(Diagnostic diag) : _diag(std::move(diag)) {}

        const Diagnostic& diagnostic() const { return _diag; }
        const char* what() const noexcept override { return _diag.message.c_str(); }
        void setLocation(SourceLocation location) { _diag.location = location; }

    private:

        Diagnostic _diag;
    };

    class ErrorReporter
    {
    public:

        void cacheLine(std::string filename, std::string_view source)
        {
            if (!_fileBuffers.contains(filename))
                _fileBuffers[filename] = std::vector<std::string>{};
            _fileBuffers[filename].emplace_back(source);
        }

        void handleException(const CompilerException& e)
        {
            const auto& diag = e.diagnostic();
            const auto& loc = diag.location;

            // Print header: file.sin:12:5: error: message
            fprintf(stderr, "%s: line %zu: col %zu: \033[1;31merror:\033[0;1m %s\033[0m\n",
                loc.filename.c_str(), loc.line, loc.column, diag.message.c_str());

            // Print the source line and the "caret" (^)
            if (_fileBuffers.contains(loc.filename))
            {
                const std::string& lineText = _fileBuffers[loc.filename][loc.line];
                fprintf(stderr, " %5zu | %s\n", loc.line, lineText.c_str());
                fprintf(stderr, "       | %*s\033[1;32m^\033[0m\n", (int)loc.column - 1, "");
            }
        }

        void report(const CompilerException& e)
        {
            handleException(e);
            ++_errorCount;
        }

        bool hasErrors() const { return _errorCount > 0; }

    private:

        std::unordered_map<std::string, std::vector<std::string>> _fileBuffers;
        int _errorCount = 0;
    };
}