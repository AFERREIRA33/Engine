#ifndef VDE__UTIL__BADGE_H
#define VDE__UTIL__BADGE_H
#pragma once

namespace vde::util
{
    template<typename T>
    class Badge
    {
        friend T;
        T* m_owner;
        explicit Badge(T* owner = nullptr) : m_owner(owner) {}

    public:
        T* Owner() const { return m_owner; }
    };
}

#define vME      vde::util::Badge    { this }
#define vANON(T) vde::util::Badge<T> { nullptr }

#endif /* VDE__UTIL__BADGE_H */
