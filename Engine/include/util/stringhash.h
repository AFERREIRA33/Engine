#ifndef VDE__UTIL__STRINGHASH_H
#define VDE__UTIL__STRINGHASH_H

#include <cstddef>
#include <cstdint>
#include <string>

using string_hash_t = std::uint64_t;

namespace vde::util
{
	// String hashing - straight into common for better accessibility
	template<class> struct string_hasher;

	template<>
	struct string_hasher<std::string>
	{
		string_hash_t constexpr operator()(const char* input) const
		{
			return *input ? static_cast<uint32_t>(*input) + 0x21 * (*this)(input + 1) : 0x1505;
		}

		string_hash_t constexpr operator()(const char* input, uint32_t length) const
		{
			return length ? static_cast<uint32_t>(*input) + 0x21 * (*this)(input + 1, length - 1) : 0x1505;
		}

		string_hash_t operator()(const std::string& input) const
		{
			return (*this)(input.c_str());
		}
	};

	string_hash_t constexpr hash_string(const char* s)
	{
		return string_hasher<std::string>()(s);
	}

	string_hash_t constexpr hash_string(const char* s, size_t n)
	{
		return string_hasher<std::string>()(s, uint32_t(n));
	}

	string_hash_t hash_string(const std::string& s);
}

inline namespace string_hash_t_literal_operator
{
	string_hash_t constexpr operator""_h(const char* s, size_t)
	{
		return vde::util::string_hasher<std::string>()(s);
	}
}

using namespace string_hash_t_literal_operator;

#endif /* VDE__UTIL__STRINGHASH_H */