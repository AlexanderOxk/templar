#include "templar/lexer/Lexer.hpp"

#include <cctype>
#include <format>
#include <string>

#include "templar/lexer/Token.hpp"

namespace templar::lexer {

static bool isIdentifierChar(char c) {
    return (isalnum(c) || c == '_' || c == '-');
}

static bool isCharToken(char c) {
    return (c == '(' || c == ')' || c == ':' || c == '!');
}

static bool isWhitespace(char c) {
    return (isspace(c));
}

std::string Lexer::getDiagnostic() const {
    return std::format("Bad token at {}:{}: '{}', expected [a-zA-Z0-9():!%]", m_line, m_column,
                       current());
}

void Lexer::processText() {
    processPossibleStartToken();

    while (!atEnd() && !m_encounteredError) {
        if (m_insideCodeBlock) {
            processCodeToken();
        }
        else {
            processTextToken();
            if (!atEnd()) {
                do {
                    processPossibleStartToken();
                } while (matchEndOfTextBlock());
            }
        }
    }

    makeToken(TokenKind::EndOfFile);
}

void Lexer::processTextToken() {
    do {
        advance();
    } while (!atEnd() && !matchEndOfTextBlock());

    makeToken(TokenKind::TextBlock);
}

void Lexer::processCodeToken() {
    // Consume whitespace
    while (isWhitespace(current())) {
        advance();
    }
    m_lastEnd = m_current;

    if (atEnd())
        return;

    // Handle single char tokens
    if (isCharToken(current())) {
        char c = current();
        advance();
        makeToken(static_cast<TokenKind>(c));
        return;
    }

    // Handle end of code block
    if (matchEndOfCodeBlock()) {
        advance();
        advance();
        makeToken(TokenKind::CodeBlockEnd);
        m_insideCodeBlock = false;
        return;
    }

    // Consume identifier chars
    do {
        advance();
    } while (isIdentifierChar(current()));

    // Exit on invalid characters here
    if (!(isWhitespace(current()) || isCharToken(current()) ||
          (current() == '%' && peak() == '>'))) {
        m_encounteredError = true;
        return;
    }

    // Match against any keywords or tokens, otherwise identifier
    if (auto it = keywordToTokenKind.find(currentText()); it != keywordToTokenKind.end()) {
        makeToken(it->second);
    }
    else {
        makeToken(TokenKind::Identifier);
    }
}

void Lexer::processPossibleStartToken() {
    if (current() == '<' && peak() == '%') {
        advance(); // '<'
        advance(); // '%'
        makeToken(TokenKind::CodeBlockStart);
        m_insideCodeBlock = true;
    }
    else if (current() == '\\' && peak(1) == '<' && peak(2) == '%') {
        advance(); // '\'
        advance(); // '<'
        advance(); // '%'
        makeToken(TokenKind::EscapedCodeBlockStart);
    }
    else if (current() == '\\' && peak() == '\\') {
        advance(); // '\'
        advance(); // '\'
        makeToken(TokenKind::EscapedBackslash);
    }
}

void Lexer::makeToken(TokenKind kind) {
    m_tokens.emplace_back(Token(kind, currentText(), m_line, m_column));
    m_lastEnd = m_current;
}

std::string_view Lexer::currentText() const {
    return m_text.substr(m_lastEnd, m_current - m_lastEnd);
}

void Lexer::advance() {
    if (current() == '\n') {
        m_line++;
        m_column = 0;
    }
    m_column++;
    m_current++;
}

char Lexer::current() const {
    if (m_current < m_text.length())
        return m_text.at(m_current);
    return '\0';
}

char Lexer::peak() const {
    return peak(1);
}

char Lexer::peak(int offset) const {
    if (m_current + offset >= 0 && m_current + offset < m_text.length()) {
        return m_text.at(m_current + offset);
    }
    else {
        return '\0';
    }
}

bool Lexer::atEnd() const {
    return m_current >= m_text.length();
}

bool Lexer::matchEndOfTextBlock() const {
    return (current() == '\\' && peak() == '\\') ||
           (current() == '\\' && peak(1) == '<' && peak(2) == '%') ||
           (current() == '<' && peak() == '%');
}

bool Lexer::matchEndOfCodeBlock() const {
    return (current() == '%' && peak() == '>');
}

} // namespace templar::lexer
