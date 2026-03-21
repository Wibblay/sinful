#include "Token.hpp"

using namespace Sinful::Exceptions;
using namespace Sinful::Types;

namespace Sinful::Tokens
{
	std::string tokenTypeToString(TokenType type)
	{
		switch (type)
		{
			using enum TokenType;
		case NullToken:		return "nullToken";

		case Variable:		return "variable";
		case IntLiteral:	return "intLiteral";

		case Equals:		return "=";
		case Plus:			return "+";
		case Minus:			return "-";
		case Star:			return "*";
		case FSlash:		return "/";
		case Exclamation:	return "!";
		case ExclEquals:	return "!=";
		case LessThan:		return "<";
		case GreaterThan:	return ">";
		case LeOrEqual:		return "<=";
		case GrOrEqual:		return ">=";
		case DubEquals:		return "==";

		case I32Type:		return "i32";
		case BoolType:		return "bool";

		case SemiColon:		return ";";
		case LBracket:		return "(";
		case RBracket:		return ")";
		case LBrace:		return "{";
		case RBrace:		return "}";

		case Print:			return "print";
		case True:			return "true";
		case False:			return "false";
		default:            return "";
		}
	}

	Type convertTypeToken(TokenType type)
	{
		switch (type)
		{
		case TokenType::I32Type:	return Type::i32();
		case TokenType::BoolType:	return Type::boolean();
		default:
			throw CompilerException(Diagnostic{
				Exceptions::Diagnostic::Level::Error,
				{},
				"Token type " + tokenTypeToString(type) + " is not a type token"
			});
		}
	}

	Token::Token(TokenType type, SourceLocation loc) : _type(type), _location(loc)
	{
		_lexeme = tokenTypeToString(type);
		if (_lexeme == "") throw CompilerException(Diagnostic{
			Exceptions::Diagnostic::Level::Error,
			loc,
			"Unrecognise token type " + tokenTypeToString(type)
		});
		
	}

	const Token& TokenStream::peek(const size_t offset) const
	{
		size_t index = _current + offset;
		if (index >= _tokens.size())
			return NULL_TOKEN;
		return _tokens[index];
	}
}
