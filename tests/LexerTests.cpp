#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <string>

#include "templar/lexer/Lexer.hpp"

using namespace templar::lexer;

TEST_CASE("General", "[Lexer]") {
    const std::string text = R"(
<%for ( var : vars_list )%>
int <%var%>;
<%endfor%>
<%if(text)%>
void function(bool test) {
    <%other-var%> = 1;
}
<%endif%>
)";

    // clang-format off
    constexpr std::array expected = {
        TokenKind::TextBlock,
        TokenKind::CodeBlockStart,
        TokenKind::For,
        TokenKind::OpenParen,
        TokenKind::Identifier,
        TokenKind::Colon,
        TokenKind::Identifier,
        TokenKind::CloseParen,
        TokenKind::CodeBlockEnd,
        TokenKind::TextBlock,
        TokenKind::CodeBlockStart,
        TokenKind::Identifier,
        TokenKind::CodeBlockEnd,
        TokenKind::TextBlock,
        TokenKind::CodeBlockStart,
        TokenKind::EndFor,
        TokenKind::CodeBlockEnd,
        TokenKind::TextBlock,
        TokenKind::CodeBlockStart,
        TokenKind::If,
        TokenKind::OpenParen,
        TokenKind::Identifier,
        TokenKind::CloseParen,
        TokenKind::CodeBlockEnd,
        TokenKind::TextBlock,
        TokenKind::CodeBlockStart,
        TokenKind::Identifier,
        TokenKind::CodeBlockEnd,
        TokenKind::TextBlock,
        TokenKind::CodeBlockStart,
        TokenKind::EndIf,
        TokenKind::CodeBlockEnd,
        TokenKind::TextBlock,
        TokenKind::EndOfFile,
    };
    // clang-format on

    auto lexer = Lexer(text);
    CHECK(lexer.syntaxOk());

    const auto& tokens = lexer.getTokens();
    CHECK(tokens.size() == expected.size());

    for (int i = 0; i < tokens.size() && i < expected.size(); i++) {
        CHECK(tokens.at(i).kind == expected.at(i));
    }
}

TEST_CASE("Start with CodeBlockStart", "[Lexer]") {
    const std::string text = R"(<%if(!var123)%>int a;<%endif%>)";
    // clang-format off
    constexpr std::array expected = {
        TokenKind::CodeBlockStart,
        TokenKind::If,
        TokenKind::OpenParen,
        TokenKind::Exclamation,
        TokenKind::Identifier,
        TokenKind::CloseParen,
        TokenKind::CodeBlockEnd,
        TokenKind::TextBlock,
        TokenKind::CodeBlockStart,
        TokenKind::EndIf,
        TokenKind::CodeBlockEnd,
        TokenKind::EndOfFile,
    };
    // clang-format on

    auto lexer = Lexer(text);
    CHECK(lexer.syntaxOk());

    const auto& tokens = lexer.getTokens();
    CHECK(tokens.size() == expected.size());

    for (int i = 0; i < tokens.size() && i < expected.size(); i++) {
        CHECK(tokens.at(i).kind == expected.at(i));
    }
}

TEST_CASE("Not closed code block", "[Lexer]") {
    const std::string text = R"(<%if(!var-_123)%>int a;<%endif  )";
    // clang-format off
    constexpr std::array expected = {
        TokenKind::CodeBlockStart,
        TokenKind::If,
        TokenKind::OpenParen,
        TokenKind::Exclamation,
        TokenKind::Identifier,
        TokenKind::CloseParen,
        TokenKind::CodeBlockEnd,
        TokenKind::TextBlock,
        TokenKind::CodeBlockStart,
        TokenKind::EndIf,
        TokenKind::EndOfFile,
    };
    // clang-format on

    auto lexer = Lexer(text);
    CHECK(lexer.syntaxOk());

    const auto& tokens = lexer.getTokens();
    CHECK(tokens.size() == expected.size());

    for (int i = 0; i < tokens.size() && i < expected.size(); i++) {
        CHECK(tokens.at(i).kind == expected.at(i));
    }

    CHECK(tokens.at(9).text == "endif");
}

TEST_CASE("Escaped tokens", "[Lexer]") {
    const std::string text = R"(\<%text%>int \\<%var%>; \\)";
    // clang-format off
    constexpr std::array expected = {
        TokenKind::EscapedCodeBlockStart,
        TokenKind::TextBlock,
        TokenKind::EscapedBackslash,
        TokenKind::CodeBlockStart,
        TokenKind::Identifier,
        TokenKind::CodeBlockEnd,
        TokenKind::TextBlock,
        TokenKind::EscapedBackslash,
        TokenKind::EndOfFile,
    };
    // clang-format on

    auto lexer = Lexer(text);
    CHECK(lexer.syntaxOk());

    const auto& tokens = lexer.getTokens();
    CHECK(tokens.size() == expected.size());

    for (int i = 0; i < tokens.size() && i < expected.size(); i++) {
        CHECK(tokens.at(i).kind == expected.at(i));
    }

    CHECK(tokens.at(7).text == "\\\\");
}

TEST_CASE("Bad syntax", "[Lexer]") {
    const std::string text = R"(<%illegal_var$%>)";

    auto lexer = Lexer(text);
    CHECK_FALSE(lexer.syntaxOk());
    CHECK(lexer.getDiagnostic() == "Bad token at 1:14: '$', expected [a-zA-Z0-9():!%]");
}
