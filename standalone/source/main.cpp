import stk.entity;
import stk.log;
import stk.hash;

using namespace stk;

// bar interface
class bar
{
public:
	virtual void go_bar() = 0;
};

template<>
constexpr hash stk::hash_of<bar>()
{
	return "bar"_h;
}

// baz interface
class baz
{
public:
	virtual void go_baz() = 0;
};

template<>
constexpr hash stk::hash_of<baz>()
{
	return "baz"_h;
}

// Foo implements both bar and baz
class foo : public bar, public baz
{
public:
	void go_bar() override
	{
		debugln("go bar!");
	}

	void go_baz() override
	{
		debugln("go baz!");
	}
};

class just_baz : public baz
{
public:
	void go_baz() override
	{
		debugln("just baz!");
	}
};

class foo_node : public node
{
public:
	bool attach(node& parent)
	{
		if (!parent.add_aspect<bar>(&m_foo))
		{
			return false;
		}

		if (!parent.add_aspect<baz>(&m_foo))
		{
			parent.remove_aspect<bar>();
			return false;
		}

		return true;
	}

	void detach(node& parent)
	{
		parent.remove_aspect<bar>();
		parent.remove_aspect<baz>();
	}

private:
	foo m_foo;
};

class just_baz_node : public node
{
public:
	bool attach(node& parent)
	{
		if (!parent.add_aspect<baz>(&m_just_baz))
		{
			return false;
		}

		parent.add_handler("dance"_h, this, [this]() { this->dance(); });

		return true;
	}

	void detach(node& parent)
	{
		parent.remove_aspect<baz>();
		parent.remove_handler("dance"_h, *this);
	}

	void dance()
	{
		debugln("just baz dance!");
	}

private:
	just_baz m_just_baz;
};

int main()
{
    node entity;
	just_baz_node* child2 = entity.make_child<just_baz_node>();
	if (child2 == nullptr)
	{
		debugln("child2 not created");
	}
	else
	{
		debugln("child2 created");
	}

	foo_node* child = entity.make_child<foo_node>();
	if (child == nullptr)
	{
		debugln("child not created");
	}
	else
	{
		debugln("child created");
	}

	bar* bar_p = entity.get_aspect<bar>();
	if (bar_p != nullptr)
	{
		bar_p->go_bar();
	}

	baz* baz_p = entity.get_aspect<baz>();
	if (baz_p != nullptr)
	{
		baz_p->go_baz();
	}

	entity.send("dance"_h);

	entity.remove_child(child2);
	baz_p = entity.get_aspect<baz>();
	if (baz_p != nullptr)
	{
		baz_p->go_baz();
	}

	entity.send("dance"_h);

    return 0;
}
