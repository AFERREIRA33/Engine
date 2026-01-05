#ifndef VDE__UTIL__PROPERTY_H
#define VDE__UTIL__PROPERTY_H
#pragma once

#include <util/stringhash.h>

#include <glm/glm.hpp>
#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace vde::util
{
	template<typename T>
	struct PropertyTypeId {};

	template<string_hash_t H>
	struct PropertyType {};

	template<string_hash_t H>
	using PropertyTypeV = PropertyType<H>::Type;

	class PropertyBase;

	template<typename T>
	class Property;
	
	class PropertyMap
		: public std::vector<std::pair<std::string, PropertyBase*>>
	{
	protected:
		PropertyMap* m_parent = nullptr;
		std::string  m_name;

	public:
		explicit PropertyMap(PropertyMap* parent = nullptr);
		
		PropertyMap* ParentMap() const;
		virtual std::string Name() const = 0;
		
		void AddReferenceTo(PropertyMap* other);
		void RemoveReferenceTo(PropertyMap* other);

		PropertyBase* operator()(const std::string& k);
	};

	class PropertyBase
	{
	public:
		virtual const std::string& Name()     const = 0;
		virtual PropertyMap*       Owner()    const = 0;
		virtual string_hash_t      TypeId()   const = 0;
		virtual string_hash_t      MetaType() const = 0;

		virtual void OverrideOwner(PropertyMap* ownerOverride) = 0;

		template<typename T>
		Property<T>* As()
		{
			if (TypeId() != PropertyTypeId<T>::Value)
				throw std::bad_cast();

			return reinterpret_cast<Property<T>*>(this);
		}
	};

	template<typename T>
	class Property
		: public PropertyBase
	{
		std::string   m_name;
		PropertyMap*  m_owner;
		PropertyMap*  m_ownerOverride;

		T*            m_valuePtr;
		bool          m_hasOwnership;
		string_hash_t m_metaType;
		std::function<void(const T& /* oldValue */, const T& /* newValue */)> m_onChanged;

	public:
		explicit Property(PropertyMap* owner, const std::string& name, const T& defaultValue, string_hash_t metaType = ""_h)
			: m_name(name)
			, m_owner(owner)
			, m_ownerOverride(nullptr)
			, m_valuePtr(new T)
			, m_hasOwnership(true)
			, m_metaType(metaType)
		{
			*m_valuePtr = defaultValue;
			owner->push_back(std::make_pair(m_name, this));
		}

		explicit Property(PropertyMap* owner, const std::string& name, T* valuePtr, string_hash_t metaType = ""_h)
			: m_name(name)
			, m_owner(owner)
			, m_ownerOverride(nullptr)
			, m_valuePtr(valuePtr)
			, m_hasOwnership(false)
			, m_metaType(metaType)
		{
			owner->push_back(std::make_pair(m_name, this));
		}

		~Property() noexcept
		{
			auto it = std::find(m_owner->begin(), m_owner->end(), std::make_pair(m_name, this));
			if (it != m_owner->end())
				m_owner->erase(it);

			if (m_hasOwnership)
				delete m_valuePtr;
		}

		void               OverrideOwner(PropertyMap* ownerOverride) { m_ownerOverride = ownerOverride; }

		const std::string& Name()     const override { return m_name; }
		PropertyMap*       Owner()    const override { return m_ownerOverride ? m_ownerOverride : m_owner; }
		string_hash_t      TypeId()   const override { return PropertyTypeId<T>::Value; }
		string_hash_t      MetaType() const override { return m_metaType; }

		void               SetOnChanged(const std::function<void(const T& /* oldValue */, const T& /* newValue */)>& onChanged) { m_onChanged = onChanged; }
		void               CallOnChanged(const T& from, const T& to) { if (m_onChanged) m_onChanged(from, to); }
		void               CallOnChanged(const T& to) { if (m_onChanged) m_onChanged(to, to); }

		Property& operator=(const T& value) { if (*m_valuePtr != value) CallOnChanged(*m_valuePtr, value); *m_valuePtr = value; return *this; }

		operator T() const { return *m_valuePtr; }
		const T& operator()() const { return *m_valuePtr; }
		T* Ptr() { return m_valuePtr; }
	};
}

#define VDE_UTIL_DECLARE_PROPERTY_TYPE(T, INTERPOLABLE) \
	template<> struct vde::util::PropertyTypeId<T>    { static constexpr string_hash_t Value = #T##_h; static constexpr bool IsInterpolable = INTERPOLABLE; }; \
	template<> struct vde::util::PropertyType<#T##_h> { using Type = T; }

#define VDE_UTIL_DECLARE_PROPERTY_TYPE_INTERPOLABLE(T)    VDE_UTIL_DECLARE_PROPERTY_TYPE(T, true)
#define VDE_UTIL_DECLARE_PROPERTY_TYPE_NONINTERPOLABLE(T) VDE_UTIL_DECLARE_PROPERTY_TYPE(T, false)

VDE_UTIL_DECLARE_PROPERTY_TYPE_INTERPOLABLE(float);
VDE_UTIL_DECLARE_PROPERTY_TYPE_INTERPOLABLE(glm::vec2);
VDE_UTIL_DECLARE_PROPERTY_TYPE_INTERPOLABLE(glm::vec3);
VDE_UTIL_DECLARE_PROPERTY_TYPE_INTERPOLABLE(glm::vec4);
VDE_UTIL_DECLARE_PROPERTY_TYPE_NONINTERPOLABLE(bool);
VDE_UTIL_DECLARE_PROPERTY_TYPE_NONINTERPOLABLE(int);
VDE_UTIL_DECLARE_PROPERTY_TYPE_NONINTERPOLABLE(std::string);

#endif /* VDE__UTIL__PROPERTY_H */
