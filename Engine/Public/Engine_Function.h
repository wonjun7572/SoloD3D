#pragma once

namespace Engine
{
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