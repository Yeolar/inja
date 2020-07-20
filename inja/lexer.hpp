// Copyright (c) 2020 Pantor. All rights reserved.

#ifndef INCLUDE_INJA_LEXER_HPP_
#define INCLUDE_INJA_LEXER_HPP_

#include <cctype>
#include <locale>

#include "config.hpp"
#include "token.hpp"
#include "utils.hpp"

namespace inja {

/*!
 * \brief Class for lexing an inja Template.
 */
class Lexer {
  enum class State {
    Text,
    ExpressionStart,
    ExpressionBody,
    LineStart,
    LineBody,
    StatementStart,
    StatementStartNoLstrip,
    StatementStartForceLstrip,
    StatementBody,
    CommentStart,
    CommentBody,
  };

  enum class MinusState {
    Operator,
    Number,
  };

  const LexerConfig &config;

  State state;
  MinusState minus_state;
  acc::StringPiece m_in;
  size_t tok_start;
  size_t pos;


  Token scan_body(acc::StringPiece close, Token::Kind closeKind, acc::StringPiece close_trim = acc::StringPiece(), bool trim = false) {
  again:
    // skip whitespace (except for \n as it might be a close)
    if (tok_start >= m_in.size()) {
      return make_token(Token::Kind::Eof);
    }
    char ch = m_in[tok_start];
    if (ch == ' ' || ch == '\t' || ch == '\r') {
      tok_start += 1;
      goto again;
    }

    // check for close
    if (!close_trim.empty() && m_in.subpiece(tok_start).startsWith(close_trim)) {
      state = State::Text;
      pos = tok_start + close_trim.size();
      Token tok = make_token(closeKind);
      skip_whitespaces_and_newlines();
      return tok;
    }

    if (m_in.subpiece(tok_start).startsWith(close)) {
      state = State::Text;
      pos = tok_start + close.size();
      Token tok = make_token(closeKind);
      if (trim) {
        skip_whitespaces_and_first_newline();
      }
      return tok;
    }

    // skip \n
    if (ch == '\n') {
      tok_start += 1;
      goto again;
    }

    pos = tok_start + 1;
    if (std::isalpha(ch)) {
      minus_state = MinusState::Operator;
      return scan_id();
    }

    MinusState current_minus_state = minus_state;
    if (minus_state == MinusState::Operator) {
      minus_state = MinusState::Number;
    }

    switch (ch) {
    case '+':
      return make_token(Token::Kind::Plus);
    case '-':
      if (current_minus_state == MinusState::Operator) {
        return make_token(Token::Kind::Minus);
      }
      return scan_number();
    case '*':
      return make_token(Token::Kind::Times);
    case '/':
      return make_token(Token::Kind::Slash);
    case '^':
      return make_token(Token::Kind::Power);
    case '%':
      return make_token(Token::Kind::Percent);
    case ',':
      return make_token(Token::Kind::Comma);
    case ':':
      return make_token(Token::Kind::Colon);
    case '(':
      return make_token(Token::Kind::LeftParen);
    case ')':
      minus_state = MinusState::Operator;
      return make_token(Token::Kind::RightParen);
    case '[':
      return make_token(Token::Kind::LeftBracket);
    case ']':
      minus_state = MinusState::Operator;
      return make_token(Token::Kind::RightBracket);
    case '{':
      return make_token(Token::Kind::LeftBrace);
    case '}':
      minus_state = MinusState::Operator;
      return make_token(Token::Kind::RightBrace);
    case '>':
      if (pos < m_in.size() && m_in[pos] == '=') {
        pos += 1;
        return make_token(Token::Kind::GreaterEqual);
      }
      return make_token(Token::Kind::GreaterThan);
    case '<':
      if (pos < m_in.size() && m_in[pos] == '=') {
        pos += 1;
        return make_token(Token::Kind::LessEqual);
      }
      return make_token(Token::Kind::LessThan);
    case '=':
      if (pos < m_in.size() && m_in[pos] == '=') {
        pos += 1;
        return make_token(Token::Kind::Equal);
      }
      return make_token(Token::Kind::Unknown);
    case '!':
      if (pos < m_in.size() && m_in[pos] == '=') {
        pos += 1;
        return make_token(Token::Kind::NotEqual);
      }
      return make_token(Token::Kind::Unknown);
    case '\"':
      return scan_string();
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      minus_state = MinusState::Operator;
      return scan_number();
    case '_':
      minus_state = MinusState::Operator;
      return scan_id();
    default:
      return make_token(Token::Kind::Unknown);
    }
  }

  Token scan_id() {
    for (;;) {
      if (pos >= m_in.size()) {
        break;
      }
      char ch = m_in[pos];
      if (!std::isalnum(ch) && ch != '.' && ch != '/' && ch != '_' && ch != '-') {
        break;
      }
      pos += 1;
    }
    return make_token(Token::Kind::Id);
  }

  Token scan_number() {
    for (;;) {
      if (pos >= m_in.size()) {
        break;
      }
      char ch = m_in[pos];
      // be very permissive in lexer (we'll catch errors when conversion happens)
      if (!std::isdigit(ch) && ch != '.' && ch != 'e' && ch != 'E' && ch != '+' && ch != '-') {
        break;
      }
      pos += 1;
    }
    return make_token(Token::Kind::Number);
  }

  Token scan_string() {
    bool escape {false};
    for (;;) {
      if (pos >= m_in.size()) {
        break;
      }
      char ch = m_in[pos++];
      if (ch == '\\') {
        escape = true;
      } else if (!escape && ch == m_in[tok_start]) {
        break;
      } else {
        escape = false;
      }
    }
    return make_token(Token::Kind::String);
  }

  Token make_token(Token::Kind kind) const { return Token(kind, inja::slice(m_in, tok_start, pos)); }

  void skip_whitespaces_and_newlines() {
    if (pos < m_in.size()) {
      while (pos < m_in.size() && (m_in[pos] == ' ' || m_in[pos] == '\t' || m_in[pos] == '\n' || m_in[pos] == '\r')) {
        pos += 1;
      }
    }
  }

  void skip_whitespaces_and_first_newline() {
    if (pos < m_in.size()) {
      while (pos < m_in.size() && (m_in[pos] == ' ' || m_in[pos] == '\t')) {
        pos += 1;
      }
    }

    if (pos < m_in.size()) {
      char ch = m_in[pos];
      if (ch == '\n') {
        pos += 1;
      } else if (ch == '\r') {
        pos += 1;
        if (pos < m_in.size() && m_in[pos] == '\n') {
          pos += 1;
        }
      }
    }
  }

  static acc::StringPiece clear_final_line_if_whitespace(acc::StringPiece text) {
    acc::StringPiece result = text;
    while (!result.empty()) {
      char ch = result.back();
      if (ch == ' ' || ch == '\t') {
        result.removeSuffix(1);
      } else if (ch == '\n' || ch == '\r') {
        break;
      } else {
        return text;
      }
    }
    return result;
  }

public:
  explicit Lexer(const LexerConfig &config) : config(config) {}

  SourceLocation current_position() const {
    return get_source_location(m_in, tok_start);
  }

  void start(acc::StringPiece input) {
    m_in = input;
    tok_start = 0;
    pos = 0;
    state = State::Text;
    minus_state = MinusState::Number;
  }

  Token scan() {
    tok_start = pos;

  again:
    if (tok_start >= m_in.size()) {
      return make_token(Token::Kind::Eof);
    }

    switch (state) {
    default:
    case State::Text: {
      // fast-scan to first open character
      size_t open_start = m_in.subpiece(pos).find_first_of(config.open_chars);
      if (open_start == acc::StringPiece::npos) {
        // didn't find open, return remaining text as text token
        pos = m_in.size();
        return make_token(Token::Kind::Text);
      }
      pos += open_start;

      // try to match one of the opening sequences, and get the close
      acc::StringPiece open_str = m_in.subpiece(pos);
      bool must_lstrip = false;
      if (open_str.startsWith(config.expression_open)) {
        state = State::ExpressionStart;
      } else if (open_str.startsWith(config.statement_open)) {
        if (open_str.startsWith(config.statement_open_no_lstrip)) {
          state = State::StatementStartNoLstrip;
        } else if (open_str.startsWith(config.statement_open_force_lstrip )) {
          state = State::StatementStartForceLstrip;
          must_lstrip = true;
        } else {
          state = State::StatementStart;
          must_lstrip = config.lstrip_blocks;
        }
      } else if (open_str.startsWith(config.comment_open)) {
        state = State::CommentStart;
        must_lstrip = config.lstrip_blocks;
      } else if ((pos == 0 || m_in[pos - 1] == '\n') &&
                 open_str.startsWith(config.line_statement)) {
        state = State::LineStart;
      } else {
        pos += 1; // wasn't actually an opening sequence
        goto again;
      }

      acc::StringPiece text = inja::slice(m_in, tok_start, pos);
      if (must_lstrip) {
        text = clear_final_line_if_whitespace(text);
      }

      if (text.empty()) {
        goto again; // don't generate empty token
      }
      return Token(Token::Kind::Text, text);
    }
    case State::ExpressionStart: {
      state = State::ExpressionBody;
      pos += config.expression_open.size();
      return make_token(Token::Kind::ExpressionOpen);
    }
    case State::LineStart: {
      state = State::LineBody;
      pos += config.line_statement.size();
      return make_token(Token::Kind::LineStatementOpen);
    }
    case State::StatementStart: {
      state = State::StatementBody;
      pos += config.statement_open.size();
      return make_token(Token::Kind::StatementOpen);
    }
    case State::StatementStartNoLstrip: {
      state = State::StatementBody;
      pos += config.statement_open_no_lstrip.size();
      return make_token(Token::Kind::StatementOpen);
    }
    case State::StatementStartForceLstrip: {
      state = State::StatementBody;
      pos += config.statement_open_force_lstrip.size();
      return make_token(Token::Kind::StatementOpen);
    }
    case State::CommentStart: {
      state = State::CommentBody;
      pos += config.comment_open.size();
      return make_token(Token::Kind::CommentOpen);
    }
    case State::ExpressionBody:
      return scan_body(config.expression_close, Token::Kind::ExpressionClose);
    case State::LineBody:
      return scan_body("\n", Token::Kind::LineStatementClose);
    case State::StatementBody:
      return scan_body(config.statement_close, Token::Kind::StatementClose, config.statement_close_force_rstrip, config.trim_blocks);
    case State::CommentBody: {
      // fast-scan to comment close
      size_t end = m_in.subpiece(pos).find(config.comment_close);
      if (end == acc::StringPiece::npos) {
        pos = m_in.size();
        return make_token(Token::Kind::Eof);
      }
      // return the entire comment in the close token
      state = State::Text;
      pos += end + config.comment_close.size();
      Token tok = make_token(Token::Kind::CommentClose);
      if (config.trim_blocks) {
        skip_whitespaces_and_first_newline();
      }
      return tok;
    }
    }
  }

  const LexerConfig &get_config() const {
    return config;
  }
};

} // namespace inja

#endif // INCLUDE_INJA_LEXER_HPP_
