// Copyright (c) 2020 Pantor. All rights reserved.

#ifndef INCLUDE_INJA_NODE_HPP_
#define INCLUDE_INJA_NODE_HPP_

#include <string>
#include <utility>

#include "function_storage.hpp"
#include "nlohmann/json.hpp"

namespace inja {

class NodeVisitor;
class BlockNode;
class TextNode;
class ExpressionNode;
class LiteralNode;
class JsonNode;
class FunctionNode;
class ExpressionListNode;
class StatementNode;
class ForStatementNode;
class ForArrayStatementNode;
class ForObjectStatementNode;
class IfStatementNode;
class IncludeStatementNode;


class NodeVisitor {
public:
  virtual void visit(const BlockNode& node) = 0;
  virtual void visit(const TextNode& node) = 0;
  virtual void visit(const ExpressionNode& node) = 0;
  virtual void visit(const LiteralNode& node) = 0;
  virtual void visit(const JsonNode& node) = 0;
  virtual void visit(const FunctionNode& node) = 0;
  virtual void visit(const ExpressionListNode& node) = 0;
  virtual void visit(const StatementNode& node) = 0;
  virtual void visit(const ForStatementNode& node) = 0;
  virtual void visit(const ForArrayStatementNode& node) = 0;
  virtual void visit(const ForObjectStatementNode& node) = 0;
  virtual void visit(const IfStatementNode& node) = 0;
  virtual void visit(const IncludeStatementNode& node) = 0;
};

/*!
 * \brief Base node class for the abstract syntax tree (AST).
 */
class AstNode {
public:
  virtual void accept(NodeVisitor& v) const = 0;

  size_t pos;

  AstNode(size_t pos) : pos(pos) { }
  virtual ~AstNode() { };
};


class BlockNode : public AstNode {
public:
  std::vector<std::shared_ptr<AstNode>> nodes;

  explicit BlockNode() : AstNode(0) {}

  void accept(NodeVisitor& v) const {
    v.visit(*this);
  }
};

class TextNode : public AstNode {
public:
  std::string content;

  explicit TextNode(acc::StringPiece content, size_t pos): AstNode(pos), content(content.str()) { }

  void accept(NodeVisitor& v) const {
    v.visit(*this);
  }
};

class ExpressionNode : public AstNode {
public:
  explicit ExpressionNode(size_t pos) : AstNode(pos) {}

  void accept(NodeVisitor& v) const {
    v.visit(*this);
  }
};

class LiteralNode : public ExpressionNode {
public:
  nlohmann::json value;

  explicit LiteralNode(const nlohmann::json& value, size_t pos) : ExpressionNode(pos), value(value) { }

  void accept(NodeVisitor& v) const {
    v.visit(*this);
  }
};

class JsonNode : public ExpressionNode {
public:
  std::string name;
  std::string ptr {""};

  explicit JsonNode(acc::StringPiece ptr_name, size_t pos) : ExpressionNode(pos), name(ptr_name.str()) {
    // Convert dot notation to json pointer notation
    do {
      acc::StringPiece part = ptr_name.split_step('.');
      ptr.push_back('/');
      ptr.append(part.begin(), part.end());
    } while (!ptr_name.empty());
  }

  void accept(NodeVisitor& v) const {
    v.visit(*this);
  }
};

class FunctionNode : public ExpressionNode {
  using Op = FunctionStorage::Operation;

public:
  enum class Associativity {
    Left,
    Right,
  };

  unsigned int precedence;
  Associativity associativity;

  Op operation;

  std::string name;
  size_t number_args;
  CallbackFunction callback;

  explicit FunctionNode(acc::StringPiece name, size_t pos) : ExpressionNode(pos), precedence(5), associativity(Associativity::Left), operation(Op::Callback), name(name.str()), number_args(1) { }
  explicit FunctionNode(Op operation, size_t pos) : ExpressionNode(pos), operation(operation), number_args(1) {
    switch (operation) {
      case Op::Not: {
        precedence = 4;
        associativity = Associativity::Left;
      } break;
      case Op::And: {
        precedence = 1;
        associativity = Associativity::Left;
      } break;
      case Op::Or: {
        precedence = 1;
        associativity = Associativity::Left;
      } break;
      case Op::In: {
        precedence = 2;
        associativity = Associativity::Left;
      } break;
      case Op::Equal: {
        precedence = 2;
        associativity = Associativity::Left;
      } break;
      case Op::NotEqual: {
        precedence = 2;
        associativity = Associativity::Left;
      } break;
      case Op::Greater: {
        precedence = 2;
        associativity = Associativity::Left;
      } break;
      case Op::GreaterEqual: {
        precedence = 2;
        associativity = Associativity::Left;
      } break;
      case Op::Less: {
        precedence = 2;
        associativity = Associativity::Left;
      } break;
      case Op::LessEqual: {
        precedence = 2;
        associativity = Associativity::Left;
      } break;
      case Op::Add: {
        precedence = 3;
        associativity = Associativity::Left;
      } break;
      case Op::Subtract: {
        precedence = 3;
        associativity = Associativity::Left;
      } break;
      case Op::Multiplication: {
        precedence = 4;
        associativity = Associativity::Left;
      } break;
      case Op::Division: {
        precedence = 4;
        associativity = Associativity::Left;
      } break;
      case Op::Power: {
        precedence = 5;
        associativity = Associativity::Right;
      } break;
      case Op::Modulo: {
        precedence = 4;
        associativity = Associativity::Left;
      } break;
      default: {
        precedence = 1;
        associativity = Associativity::Left;
      }
    }
  }

  void accept(NodeVisitor& v) const {
    v.visit(*this);
  }
};

class ExpressionListNode : public AstNode {
public:
  std::vector<std::shared_ptr<ExpressionNode>> rpn_output;

  explicit ExpressionListNode() : AstNode(0) { }
  explicit ExpressionListNode(size_t pos) : AstNode(pos) { }

  void accept(NodeVisitor& v) const {
    v.visit(*this);
  }
};

class StatementNode : public AstNode {
public:
  StatementNode(size_t pos) : AstNode(pos) { }

  virtual void accept(NodeVisitor& v) const = 0;
};

class ForStatementNode : public StatementNode {
public:
  ExpressionListNode condition;
  BlockNode body;
  BlockNode *parent;

  ForStatementNode(size_t pos) : StatementNode(pos) { }

  virtual void accept(NodeVisitor& v) const = 0;
};

class ForArrayStatementNode : public ForStatementNode {
public:
  acc::StringPiece value;

  explicit ForArrayStatementNode(acc::StringPiece value, size_t pos) : ForStatementNode(pos), value(value) { }

  void accept(NodeVisitor& v) const {
    v.visit(*this);
  }
};

class ForObjectStatementNode : public ForStatementNode {
public:
  acc::StringPiece key;
  acc::StringPiece value;

  explicit ForObjectStatementNode(acc::StringPiece key, acc::StringPiece value, size_t pos) : ForStatementNode(pos), key(key), value(value) { }

  void accept(NodeVisitor& v) const {
    v.visit(*this);
  }
};

class IfStatementNode : public StatementNode {
public:
  ExpressionListNode condition;
  BlockNode true_statement;
  BlockNode false_statement;
  BlockNode *parent;

  bool is_nested;
  bool has_false_statement {false};

  explicit IfStatementNode(size_t pos) : StatementNode(pos), is_nested(false) { }
  explicit IfStatementNode(bool is_nested, size_t pos) : StatementNode(pos), is_nested(is_nested) { }

  void accept(NodeVisitor& v) const {
    v.visit(*this);
  }
};

class IncludeStatementNode : public StatementNode {
public:
  std::string file;

  explicit IncludeStatementNode(const std::string& file, size_t pos) : StatementNode(pos), file(file) { }

  void accept(NodeVisitor& v) const {
    v.visit(*this);
  };
};

} // namespace inja

#endif // INCLUDE_INJA_NODE_HPP_
