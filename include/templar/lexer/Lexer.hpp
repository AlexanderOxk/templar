#pragma once

#include <string_view>
#include <vector>

#include "templar/lexer/Token.hpp"

namespace templar::lexer {

class Lexer {
public:
    Lexer(std::string_view text) : m_text(text) { processText(); }

    bool syntaxOk() { return !m_encounteredError; }
    std::string getDiagnostic();
    const std::vector<Token>& getTokens() { return m_tokens; }

private:
    void processText();
    void processTextToken();
    void processCodeToken();
    void processPossibleStartToken();
    void makeToken(TokenKind kind);
    std::string_view currentText();
    void advance();
    char current();
    char peak();
    char peak(int offset);
    bool atEnd();
    bool matchEndOfTextBlock();
    bool matchEndOfCodeBlock();

    std::string_view m_text;
    std::vector<Token> m_tokens;

    size_t m_lastEnd = 0;
    size_t m_current = 0;
    size_t m_line = 1;
    size_t m_column = 1;
    bool m_insideCodeBlock = false;
    bool m_encounteredError = false;
};

} // namespace templar::lexer
