export module stk.entity;
import stk.ds;
import std.core;
import <cassert>;

using namespace stk::ds;

namespace stk::entity
{
	class node_id
	{
	public:
		uint16_t idx() const
		{
			return m_idx;
		}

		uint16_t gen() const
		{
			return m_gen;
		}

		uint16_t kind() const
		{
			return m_kind;
		}

		void invalidate()
		{
			m_idx += 1;
		}

		bool operator==(node_id const& rhs) const
		{
			return m_idx == rhs.m_idx && m_gen == rhs.m_gen && m_kind == rhs.m_kind;
		}

	private:
		uint16_t m_idx;
		uint16_t m_gen : 14;
		uint16_t m_kind : 2;
	};

	export class node
	{
	public:
		node(node_id id)
			: m_id(id)
		{
		}

		node_id id() const
		{
			return m_id;
		}

		uint16_t idx() const
		{
			return m_id.idx();
		}

		uint16_t gen() const
		{
			return m_id.gen();
		}

		uint16_t kind() const
		{
			return m_id.kind();
		}

		void invalidate()
		{
			m_id.invalidate();
		}

		template<class T>
		bool add_child(node_id id, node_id& out_id)
		{
			bool success = m_children.append(id);
			out_id = m_children.count() - 1;
			return success;
		}

	private:
		node_id m_id;
		fixed_vector<uint16_t, 8> m_children;
	};

	template<size_t N>
	class sized_node : public node
	{
	public:
		sized_node(node_id id)
			: node(id)
		{
		}

		uint16_t idx() const
		{
			return m_id.idx();
		}

		uint16_t gen() const
		{
			return m_id.gen();
		}

		uint16_t kind() const
		{
			return m_id.kind();
		}

		uint8_t* data()
		{
			return data;
		}

		uint8_t const* data() const
		{
			return data;
		}

		template<class T>
		T* as()
		{
			return reinterpret_cast<T*>(data);
		}

		template<class T>
		T const* as() const
		{
			return reinterpret_cast<T const*>(data);
		}

	private:
		uint8_t m_data[N];
	};

	class dynamic_node : public node
	{
	public:
		dynamic_node(node_id id)
			: node(id)
		{
		}

		uint8_t* data()
		{
			return m_data;
		}

		uint8_t const* data() const
		{
			return m_data;
		}

		template<class T>
		T* as()
		{
			return reinterpret_cast<T*>(m_data);
		}

		template<class T>
		T const* as() const
		{
			return reinterpret_cast<T const*>(m_data);
		}

		template<class T, typename... Args>
		T* alloc(Args&&... args)
		{
			T* obj = new T(std::forward<Args>(args)...);
			m_data = reinterpret_cast<uint8_t*>(obj);
			return as<T>();
		}

		template<class T>
		void dealloc()
		{
			delete as<T>();
			m_data = nullptr;
		}

	private:
		uint8_t* m_data;
	};

	class node_pool
	{
	public:
		bool is_valid(node_id id) const
		{
			uint16_t kind = id.kind();
			switch(kind)
			{
				case 0:
				{
					return id.idx() < small_nodes.count() && id == small_nodes[id.idx()].id();
					break;
				}
				case 2:
				{
					return id.idx() < med_nodes.count() && id == med_nodes[id.idx()].id();
					break;
				}
				case 3:
				{
					return id.idx() < large_nodes.count() && id == large_nodes[id.idx()].id();
					break;
				}
				case 4:
				{
					return id.idx() < dynamic_nodes.count() && id == dynamic_nodes[id.idx()].id();
					break;
				}
				default:
					assert(false);
			}
		}

		node* get_node(node_id id)
		{
			if (!is_valid(id))
			{
				return nullptr;
			}

			uint16_t idx = id.idx();
			uint16_t kind = id.kind();

			switch (kind)
			{
				case 0:
				{
					return &small_nodes[idx];
				}
				case 1:
				{
					return &med_nodes[idx];
				}
				case 2:
				{
					return &large_nodes[idx];
				}
				case 3:
				{
					return &dynamic_nodes[idx];
				}
				default:
					assert(false);
			}
		}

		template<class T, typename... Args>
		node* alloc(Args&&... args)
		{
			if constexpr (sizeof(T) <= 64)
			{
				for (size_t i = 0; i < small_nodes.count(); ++i)
				{
					if (!is_valid(small_nodes[i].id()))
					{
						small_nodes[i] = T(std::forward<Args>(args)...);
						return &small_nodes[i];
					}
				}

				if (small_nodes.append(sized_node<64>(node_id(small_nodes.count(), 0, 0))))
				{
					small_nodes[small_nodes.count() - 1] = T(std::forward<Args>(args)...);
					return &small_nodes[small_nodes.count() - 1];
				}
				return nullptr;
			}

			if constexpr (sizeof(T) <= 256)
			{
				for (size_t i = 0; i < med_nodes.count(); ++i)
				{
					if (!is_valid(med_nodes[i].id()))
					{
						med_nodes[i] = T(std::forward<Args>(args)...);
						return &med_nodes[i];
					}
				}
				
				if (med_nodes.append(sized_node<256>(node_id(med_nodes.count(), 0, 1))))
				{
					med_nodes[med_nodes.count() - 1] = T(std::forward<Args>(args)...);
					return &med_nodes[med_nodes.count() - 1];
				}
				return nullptr;
			}
		}

	private:
		fixed_vector<sized_node<64>, 65536> small_nodes;
		fixed_vector<sized_node<256>, 65536> med_nodes;
		fixed_vector<sized_node<1024>, 65536> large_nodes;
		fixed_vector<dynamic_node, 65536> dynamic_nodes;
	};
}