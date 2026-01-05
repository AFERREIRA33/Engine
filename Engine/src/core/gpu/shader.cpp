#include <core/gpu/shader.h>

using namespace vde::core::gpu;

ShaderDataStore::ShaderDataStore(const ShaderDataType& shaderType, size_t size /* = 0 */)
	: type(shaderType)
	, data(std::max(size_t(shaderType.size), size), std::byte(0))
{
}

ShaderDataProxy ShaderDataStore::operator[](const std::string& memberName)
{
    for (const auto& member : type.members)
        if (member.name == memberName)
            return ShaderDataProxy(*this, &member, member.offset, member.arraySize > 0 || member.isRuntimeArray);

    throw std::runtime_error("Member not found: " + memberName);
}

const ShaderDataType& ShaderDataStore::Type() const
{
	return type;
}

const void* ShaderDataStore::Data() const
{
	return data.data();
} 

size_t ShaderDataStore::Size() const
{
	return data.size();
}

// ----------------------------------------------------------------------------

ShaderDataProxy::ShaderDataProxy(ShaderDataStore& buf, const ShaderDataMember* member, size_t offset /*= 0*/, bool isArray /*= false*/)
	: buffer(buf)
	, currentMember(member)
	, isArray(isArray)
	, offset(offset)
{
}

ShaderDataProxy ShaderDataProxy::operator[](const std::string& memberName)
{
	if (!currentMember)
		throw std::runtime_error("Invalid member access");

	for (const auto& member : currentMember->members)
		if (member.name == memberName)
			return ShaderDataProxy(buffer, &member, offset + member.offset, member.arraySize > 0);

	throw std::runtime_error("Member not found: " + memberName);
}

ShaderDataProxy ShaderDataProxy::operator[](size_t index)
{
	if (!currentMember || (!isArray) || ((index >= currentMember->arraySize) && !currentMember->isRuntimeArray))
		throw std::runtime_error("Invalid array access");

	return ShaderDataProxy(buffer, currentMember, offset + currentMember->arrayStride * index, false);
}
