// Copyright (c) 2020 Pantor. All rights reserved.

#ifndef INCLUDE_INJA_TOKEN_HPP_
#define INCLUDE_INJA_TOKEN_HPP_

#include <string>

#include <accelerator/Range.h>

namespace inja {

/*!
 * \brief Helper-class for the inja Lexer.
 */
struct Token {
  enum class Kind {
    Text,
    ExpressionOpen,     // {{
    ExpressionClose,    // }}
    LineStatementOpen,  // ##
    LineStatementClose, // \n
    StatementOpen,      // {%
    StatementClose,     // %}
    CommentOpen,        // {#
    CommentClose,       // #}
    Id,                 // this, this.foo
    Number,             // 1, 2, -1, 5.2, -5.3
    String,             // "this"
    Plus,               // +
    Minus,              // -
    Times,              // *
    Slash,              // /
    Percent,            // %
    Power,              // ^
    Comma,              // ,
    Colon,              // :
    LeftParen,          // (
    RightParen,         // )
    LeftBracket,        // [
    RightBracket,       // ]
    LeftBrace,          // {
    RightBrace,         // }
    Equal,              // ==
    NotEqual,           // !=
    GreaterThan,        // >
    GreaterEqual,       // >=
    LessThan,           // <
    LessEqual,          // <=
    Unknown,
    Eof,
  };
  
  Kind kind {Kind::Unknown};
  acc::StringPiece text;

  explicit constexpr Token() = default;
  explicit constexpr Token(Kind kind, acc::StringPiece text) : kind(kind), text(text) {}

  std::string describe() const {
    switch (kind) {
    case Kind::Text:
      return "<text>";
    case Kind::LineStatementClose:
      return "<eol>";
    case Kind::Eof:
      return "<eof>";
    default:
      return text.str();
    }
  }
};

} // namespace inja

#endif // INCLUDE_INJA_TOKEN_HPP_
