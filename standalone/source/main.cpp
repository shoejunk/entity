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

class foo_node : public node
{
private:
	foo m_foo;
};

int main()
{
    node entity;
	foo f;
	entity.add_aspect<bar>(&f);
	entity.add_aspect<baz>(&f);
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

	foo_node* fn = entity.make_child<foo_node>();

    return 0;
}
