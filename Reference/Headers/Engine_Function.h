#pragma once

namespace Engine
{
	class CTag_Finder
	{
	public:
		explicit CTag_Finder(const wchar_t* pTag)
			: m_pTargetTag(pTag)
		{
		}
		~CTag_Finder() {		}
	public:
		template<typename T>
		bool operator()(const T& pair)
		{
			if (0 == lstrcmpW(m_pTargetTag, pair.first))
			{
				return true;
			}

			return false;
		}

	private:
		const wchar_t*		m_pTargetTag = nullptr;
	};

	template<typename T>
	DWORD Safe_AddRef(T& pointer)
	{
		DWORD dwRefCnt = 0;

		if (pointer != nullptr)
			dwRefCnt = pointer->AddRef();

		return dwRefCnt;
	}

	template <typename T>
	DWORD Safe_Release(T& pointer)
	{
		DWORD dwRefCnt = 0;

		if (pointer != nullptr)
		{
			dwRefCnt = pointer->Release();

			if (dwRefCnt == 0)
				pointer = nullptr;
		}
		return dwRefCnt;
	}

	template<typename T>
	void Safe_Delete(T& pointer)
	{
		if (pointer != nullptr)
		{
			delete pointer;
			pointer = nullptr;
		}
	}

	template<typename T>
	void Safe_Delete_Array(T& pointer)
	{
		if (pointer != nullptr)
		{
			delete[] pointer;
			pointer = nullptr;
		}
	}
}