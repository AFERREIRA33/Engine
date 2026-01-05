#ifndef VDE__UTIL__GLOBALINSTANCE_H
#define VDE__UTIL__GLOBALINSTANCE_H
#pragma once

namespace vde::util
{
	template<typename T>
	class GlobalInstance
	{
	public:
		static T& Global()
		{
			static T s_instance;
			return s_instance;
		}
	};
}

#endif /* VDE__UTIL__GLOBALINSTANCE_H */
