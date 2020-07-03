#include "pch.h"
#include "CppUnitTest.h"
#include "OPCServer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace OPCServerLibrary;

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
			Assert::AreEqual(srv.name(), name);
			string newName = "NewServer";
			srv.name(newName);
			Assert::AreEqual(srv.name(), newName);
			string anotherName;
			//OPCServer srv2 = anotherName = "AnotherServer";
			OPCServer srv2(anotherName);
			Assert::AreEqual(srv2.name(), anotherName);
		}
	};
}
