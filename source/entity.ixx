export module stk.entity;

#pragma warning(push)
#pragma warning(disable: 5050) // _M_FP_PRECISE is defined in current command line and not in module command line
import std.core;
#pragma warning(pop)

import stk.hash;
import stk.log;

using namespace stk;

export namespace stk
{
	template<class T, class U>
	concept Derived = std::is_base_of_v<U, T>;

	class node
	{
	public:
		node()
		{
			debugln("node created");
		}

		~node()
		{
			debugln("node destroyed");
		}

		template<class T>
		void add_aspect(T* aspect)
		{
			m_aspects[hash_of<T>()] = aspect;
		}

		template<class T>
		T* get_aspect()
		{
			auto it = m_aspects.find(hash_of<T>());
			if (it != m_aspects.end())
			{
				return std::any_cast<T*>(it->second);
			}
			return nullptr;
		}

		template<Derived<node> T, typename... Args>
		T* make_child(Args&&... args)
		{
			auto child = std::make_unique<T>(std::forward<Args>(args)...);
			auto& inserted = m_children.emplace_back(std::move(child));
			return static_cast<T*>(inserted.get());
		}

	private:
		std::unordered_map<hash, std::any, hash_hasher> m_aspects;
		std::vector<std::unique_ptr<node>> m_children;
	};
}
