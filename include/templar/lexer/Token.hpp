#pragma once

#include <map>
#include <string_view>

namespace templar::lexer {

enum class TokenKind {
    OpenParen = '(',
    CloseParen = ')',
    Colon = ':',
    Exclamation = '!',
    For = 256,
    EndFor = 257,
    If = 258,
    Else = 259,
    EndIf = 260,
    Identifier = 261,
    TextBlock = 262,
    CodeBlockStart = 263,
    CodeBlockEnd = 264,
    EscapedCodeBlockStart = 265,
    EscapedBackslash = 266,
    EndOfFile = 300,
};

const std::map<std::string_view, TokenKind> keywordToTokenKind = {
    {"for", TokenKind::For},   {"endfor", TokenKind::EndFor}, {"if", TokenKind::If},
    {"else", TokenKind::Else}, {"endif", TokenKind::EndIf},
};

struct Token {
    TokenKind kind;
    std::string_view text;
    size_t line;
    size_t column;

    Token(TokenKind kind, std::string_view text, size_t line, size_t column) :
        kind(kind), text(text), line(line), column(column) {}
};

} // namespace templar::lexer
