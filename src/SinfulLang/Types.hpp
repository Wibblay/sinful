#pragma once

#include <string>
#include <variant>

namespace Sinful::Types
{
	struct NoneType       { bool operator==(const NoneType&)       const = default; };
	struct UnresolvedType { bool operator==(const UnresolvedType&) const = default; };

	struct PrimitiveType
	{
		enum class Kind { I32, Bool } kind;
		bool operator==(const PrimitiveType&) const = default;
	};

	// Future arms added here without touching existing code:
	// struct PointerType { Type* pointee; };
	// struct ArrayType   { Type* element; std::size_t count; };

	using TypeVariant = std::variant<NoneType, UnresolvedType, PrimitiveType>;

	struct Type
	{
		TypeVariant data;

		bool operator==(const Type&) const = default;

		bool isNone()       const { return std::holds_alternative<NoneType>(data); }
		bool isUnresolved() const { return std::holds_alternative<UnresolvedType>(data); }
		bool isConcrete()   const { return !isNone() && !isUnresolved(); }

		static Type none()       { return { NoneType{} }; }
		static Type unresolved() { return { UnresolvedType{} }; }
		static Type i32()        { return { PrimitiveType{ PrimitiveType::Kind::I32 } }; }
		static Type boolean()    { return { PrimitiveType{ PrimitiveType::Kind::Bool } }; }
	};

	inline std::string dataTypeToString(const Type& t)
	{
		return std::visit([](const auto& v) -> std::string {
			using T = std::decay_t<decltype(v)>;
			if constexpr (std::is_same_v<T, PrimitiveType>)
			{
				switch (v.kind)
				{
				case PrimitiveType::Kind::I32:  return "i32";
				case PrimitiveType::Kind::Bool: return "bool";
				}
			}
			return "unknown";
		}, t.data);
	}

	inline int getDataTypeSize(const Type& t)
	{
		return std::visit([](const auto& v) -> int {
			using T = std::decay_t<decltype(v)>;
			if constexpr (std::is_same_v<T, PrimitiveType>)
			{
				switch (v.kind)
				{
				case PrimitiveType::Kind::I32:  return 4;
				case PrimitiveType::Kind::Bool: return 1;
				}
			}
			return 0;
		}, t.data);
	}
}
