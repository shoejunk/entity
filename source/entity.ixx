export module stk.entity;

import std.core;
import stk.hash;
import stk.log;

using namespace stk;

export namespace stk
{
	template<class T, class U>
	concept can_attach = requires(T t, U u)
	{
		{ t.attach(u) } -> std::same_as<bool>;
	};

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

		template<class T, typename... Args>
		requires std::is_base_of_v<node, T> && can_attach<T, node>
		T* make_child(Args&&... args)
		{
			try
			{
				auto child = std::make_unique<T>(std::forward<Args>(args)...);
				auto& inserted = m_children.emplace_back(std::move(child));
				T* derived = static_cast<T*>(inserted.get());
				if (!derived->attach(*this))
				{
					m_children.pop_back();
					return nullptr;
				}
				return derived;
			}
			catch (const std::bad_alloc& e)
			{
				// Handle std::bad_alloc (thrown by std::make_unique or emplace_back if allocation fails)
				errorln("Memory allocation failed: {}", e.what());
			}
			catch (const std::exception& e)
			{
				// Handle other standard exceptions
				errorln("Exception: {}", e.what());
			}
			catch (...)
			{
				// Catch-all handler: catch any other exceptions not previously caught
				errorln("Unknown exception caught");
			}
			return nullptr;
		}

	private:
		std::unordered_map<hash, std::any, hash_hasher> m_aspects;
		std::vector<std::unique_ptr<node>> m_children;
	};
}
