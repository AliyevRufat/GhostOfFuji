#pragma once

template<class T>
class Singleton
{
public:

	static T* GetInstance()
	{
		if (!m_Instance)
		{
			m_Instance = new T();
		}

		return m_Instance;
	}

	static void DestroyInstance()
	{
		if (m_Instance != nullptr)
		{
			delete(m_Instance);
			m_Instance = nullptr;
		}
	}

	static void SetInstanceNull()
	{
		m_Instance = nullptr;
	}

private:
	static T* m_Instance;
};

template<class T>
T* Singleton<T>::m_Instance = nullptr;