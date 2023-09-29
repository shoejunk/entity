import stk.entity;
import stk.log;

using namespace stk::entity;
using namespace NStk::NLog;

int main()
{
	// bar interface
	class bar
	{
	public:
		virtual void go_bar() = 0;
	};

	// baz interface
	class baz
	{
	public:
		virtual void go_baz() = 0;
	};

	// Foo implements both bar and baz
	class foo : public bar, public baz
	{
	public:
		void go_bar() override
		{
			Log("go bar!\n");
		}

		void go_baz() override
		{
			Log("go baz!\n");
		}
	};
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

    return 0;
}
