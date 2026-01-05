#include <util/stringhash.h>

string_hash_t vde::util::hash_string(const std::string& s)
{
	return vde::util::string_hasher<std::string>()(s);
}
