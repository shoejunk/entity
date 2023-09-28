export module stk.entity;

#pragma warning(push)
#pragma warning(disable: 5050) // _M_FP_PRECISE is defined in current command line and not in module command line
import std.core;
#pragma warning(pop)

import stk.hash;

using namespace stk::hash_ns;

export namespace stk::entity
{
	class node
	{
	public:
		template<class T>
		void add_aspect(T&& aspect)
		{
			m_aspects[hash_of<T>()] = std::forward<T>(aspect);
		}

		template<class T>
		T* get_aspect()
		{
			auto it = m_aspects.find(hash_of<T>());
			if (it != m_aspects.end())
			{
				return std::any_cast<T>(&it->second);
			}
			return nullptr;
		}

	private:
		std::unordered_map<hash, std::any, hash_hasher> m_aspects;
	};
}
