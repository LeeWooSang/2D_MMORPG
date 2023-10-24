#pragma once

#define SAFE_DELETE(p)				if(p != nullptr)	{ delete p; p = nullptr; }
#define SAFE_DELETE_ARRAY(p)	if(p != nullptr)	{ delete[] p; p = nullptr; }

#define SAFE_DELETE_VECTOR(p) \
for(auto iter = p.begin(); iter != p.end(); ) \
{ \
    if((*iter) != nullptr)  delete (*iter); \
    iter = p.erase(iter); \
} \
p.clear(); \

#define SAFE_DELETE_LIST(p) \
for(auto iter = p.begin(); iter != p.end(); ) \
{ \
    if((*iter) != nullptr)  delete (*iter); \
    iter = p.erase(iter); \
} \
p.clear(); \

#define SAFE_DELETE_MAP(p) \
for(auto iter = p.begin(); iter != p.end(); ) \
{ \
    if((*iter).second != nullptr)   delete (*iter).second; \
    iter = p.erase(iter); \
} \
p.clear(); \

#define SINGLE_TONE(T)	\
    static T* m_Instance; \
public: \
            T(); \
            ~T(); \
            static T* GetInstance() \
            { \
                if (m_Instance == nullptr)  m_Instance = new T; \
                return m_Instance; \
            } \
            static void Release() \
            { \
                SAFE_DELETE(m_Instance); \
            } \

#define INIT_INSTACNE(T)    T* T::m_Instance = nullptr;

#define GET_INSTANCE(T)	T::GetInstance()