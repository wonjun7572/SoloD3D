#pragma once

#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#define D3DCOLOR_RGB(r,g,b) \
    ((D3DCOLOR)((((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#define GET_INSTANCE(CLASSNAME)	[](){											\
	CLASSNAME*	pInstance = CLASSNAME::GetInstance();							\
	if(nullptr == pInstance) {													\
	char	szMessage[MAX_PATH] = "";											\
	strcpy_s(szMessage, typeid(CLASSNAME).name());								\
	strcat_s(szMessage, "is nullptr");											\
	MessageBoxA(0, szMessage, nullptr, MB_OK);}									\
	else {																		\
	pInstance->AddRef();}														\
	return pInstance;															\
	}();

#define RELEASE_INSTANCE(CLASSNAME)	[](){										\
	CLASSNAME*	pInstance = CLASSNAME::GetInstance();							\
	if(nullptr == pInstance) {													\
	char	szMessage[MAX_PATH] = "";											\
	strcpy_s(szMessage, typeid(CLASSNAME).name());								\
	strcat_s(szMessage, "is nullptr");											\
	MessageBoxA(0, szMessage, nullptr, MB_OK);}									\
	else {																		\
	pInstance->Release();}														\
	}();


#define BEGIN(NAMESPACE)  namespace NAMESPACE {
#define END				  }

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport) 
#else
#define ENGINE_DLL _declspec(dllimport) 
#endif

#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)

#define NO_COPY(CLASSNAME)									\
		private:											\
		CLASSNAME(const CLASSNAME&) = delete;				\
		CLASSNAME& operator = (const CLASSNAME&) = delete;		

#define DECLARE_SINGLETON(CLASSNAME)						\
		NO_COPY(CLASSNAME)									\
		private:											\
		static CLASSNAME*	m_pInstance;					\
		public:												\
		static CLASSNAME*	 GetInstance( void );			\
		static unsigned long DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)						\
		CLASSNAME*	CLASSNAME::m_pInstance = nullptr;		\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{		\
			if(nullptr == m_pInstance) {					\
				m_pInstance = new CLASSNAME;				\
			}												\
			return m_pInstance;								\
		}													\
		unsigned long CLASSNAME::DestroyInstance( void ) {			\
			unsigned long dwRefCnt = 0;						\
			if(nullptr != m_pInstance)	{					\
				dwRefCnt = m_pInstance->Release();			\
				if(dwRefCnt == 0)							\
					m_pInstance = nullptr;					\
			}												\
			return dwRefCnt;								\
		}


// assert 활용해도 가능~

// 일부러 크래쉬를 내고싶을 때
#define CRASH(cause)						\
{											\
	unsigned int* crash = nullptr;			\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expr)					\
{											\
	if (!(expr))							\
	{										\
		CRASH("ASSERT_CRASH");				\
		__analysis_assume(expr);			\
	}										\
}
