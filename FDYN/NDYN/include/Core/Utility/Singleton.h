#pragma once

template<typename T> class Q_DECL_EXPORT Singleton
{
public:
    inline static T& Instance()
    {
        static T instance;
        return instance;
    }
};
