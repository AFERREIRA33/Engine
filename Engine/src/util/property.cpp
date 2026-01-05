#include <util/property.h>

using namespace vde::util;

PropertyMap::PropertyMap(PropertyMap* parent /* = nullptr */)
	: std::vector<std::pair<std::string, PropertyBase*>>()
	, m_parent(parent)
{}

PropertyMap* PropertyMap::ParentMap() const
{
	return m_parent;
}

void PropertyMap::AddReferenceTo(PropertyMap* other)
{
	for (auto& [k, v] : *other)
	{
		this->push_back({ k, v });
		v->OverrideOwner(this);
	}
}

void PropertyMap::RemoveReferenceTo(PropertyMap* other)
{
	std::vector<std::pair<std::string, PropertyBase*>> keptProps;

	for (auto& [k, v] : *other)
	{
		if (std::find_if(other->begin(), other->end(), [&](const std::pair<std::string, PropertyBase*>& p) { return p.first == k; }) == other->end())
			keptProps.push_back({ k, v });
		else
			v->OverrideOwner(nullptr);
	}

	clear();
	for (auto& p : keptProps)
		push_back(p);
}

PropertyBase* PropertyMap::operator()(const std::string& k)
{
	if (auto it = std::find_if(begin(), end(), [&](const std::pair<std::string, PropertyBase*>& p) { return p.first == k; }); it != end())
		return it->second;

	return nullptr;
}
