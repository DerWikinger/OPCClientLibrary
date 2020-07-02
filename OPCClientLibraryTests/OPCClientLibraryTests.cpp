#include "pch.h"
#include "CppUnitTest.h"
#include "OPCServer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace OPCClientLibraryTests
{
	TEST_CLASS(OPCClientLibraryTests)
	{
	public:
		
		TEST_METHOD(TestOPCServer)
		{
			string name = "Server";
			OPCServer srv(name);
			Assert::IsNotNull<OPCServer>(&srv);
			Assert::AreEqual(srv.name, name);
		}
	};
}
