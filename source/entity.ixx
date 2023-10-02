export module stk.entity;

import std.core;
import stk.hash;
import stk.log;
import <cassert>;

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
		template<class T>
		bool add_aspect(T* aspect)
		{
			if (m_aspects.find(hash_of<T>()) == m_aspects.end())
			{
				m_aspects[hash_of<T>()] = aspect;
				return true;
			}
			return false;
		}

		template<class T>
		void remove_aspect()
		{
			m_aspects.erase(hash_of<T>());
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

		template<class T>
		void remove_child(T* child)
		{
			assert(child != nullptr);
			child->detach(*this);
			for (auto it = m_children.begin(); it != m_children.end(); ++it)
			{
				if (it->get() == child)
				{
					m_children.erase(it);
					break;
				}
			}
		}

		void add_handler(hash event, node* n, std::function<void()> handler)
		{
			assert(handler != nullptr);
			m_handlers[event].push_back(std::pair<node*, std::function<void()>>(n, handler));
		}

		void remove_handler(hash event, node const& n)
		{
			auto it = m_handlers.find(event);
			if (it != m_handlers.end())
			{
				for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
				{
					if (it2->first == &n)
					{
						it->second.erase(it2);
						return;
					}
				}
			}
		}

		void send(hash event)
		{
			auto it = m_handlers.find(event);
			if (it != m_handlers.end())
			{
				for (auto& node_handler : it->second)
				{
					node_handler.second();
				}
			}
		}

	private:
		std::unordered_map<hash, std::any, hash_hasher> m_aspects;
		std::unordered_map<hash, std::vector<std::pair<node*, std::function<void()>>>, hash_hasher> m_handlers;
		std::vector<std::unique_ptr<node>> m_children;
	};
}
