import stk.entity;
import stk.log;

using namespace stk::entity;
using namespace NStk::NLog;

int main()
{
	class foo
	{
	public:
		void go_foo()
		{
			Log("go foo!\n");
		}
	};
    node entity;
	foo f;
	entity.add_aspect(f);
	foo* fp = entity.get_aspect<foo>();
	if (fp != nullptr)
	{
		fp->go_foo();
	}
    return 0;
}
