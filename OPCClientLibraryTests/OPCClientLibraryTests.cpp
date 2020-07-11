#include "pch.h"
#include "CppUnitTest.h"
#include "OPCServer.h"
#include "OPCEnum.h"
#include <list>
#include "ServerException.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace OPCClientLibrary;
using namespace OPCDA;
using namespace OpcEnumLib;

namespace OPCClientLibraryTests
{
	TEST_CLASS(OPCClientLibraryTests)
	{
	public:
		
		TEST_METHOD(TestOPCServer)
		{
			string name = "Server";
			//LPWSTR name = L"Server";
			OPCServer srv(name);
			Assert::IsNotNull<OPCServer>(&srv);
			Assert::AreEqual(srv.name(), name);
			string newName = "NewServer";
			//LPWSTR newName = L"NewServer";
			srv.name(newName);
			Assert::AreEqual(srv.name(), newName);
			string anotherName = "AnotherServer";
			//LPWSTR anotherName = L"AnotherServer";
			//OPCServer srv2 = anotherName = "AnotherServer";
			OPCServer srv2(anotherName);
			Assert::AreEqual(srv2.name(), anotherName);
			OPCServer* pSrv = &srv;
			OPCServer* pSrv2 = &srv2;
			Assert::IsTrue(pSrv == pSrv);
			Assert::IsFalse(pSrv == pSrv2);

			IID IUKNOWN_IID = __uuidof(IUnknown);
			srv.guid(&IUKNOWN_IID);
			Assert::IsTrue(*srv.guid() == IUKNOWN_IID);
			
			Assert::AreEqual(DWORD(CLSCTX_LOCAL_SERVER), srv.clsCTX());
			DWORD clsCTX = CLSCTX_ALL;
			srv.clsCTX(clsCTX);
			Assert::AreNotEqual(DWORD(CLSCTX_LOCAL_SERVER), srv.clsCTX());
			Assert::AreEqual(DWORD(CLSCTX_ALL), srv.clsCTX());
		}

		TEST_METHOD(TestBrowseServers)
		{
			string hostName = "127.0.0.1";
			list<OPCServer*>* lst = OPCEnum::BrowseOPCServers(hostName);
			Assert::IsFalse(lst->size() == 0);

			hostName = "uncorrectAddress";
			//hostName = "192.168.173.250";
			
			auto func = [&] () mutable -> list<OPCServer*>* {
				return OPCEnum::BrowseOPCServers(hostName);
			};

			//Assert::ExpectException<list<OPCServer*>>(func);
			Assert::ExpectException<HRESULT>(func);
			//Assert::ExpectException<ServerException>(func);
		}

		TEST_METHOD(TestServerItems)
		{
			OPCServer* srv = OPCEnum::GetOPCServerByName("InSAT", OPCEnum::BrowseOPCServers("localhost"));
			vector<OPCItem*>* vc = srv->items();
			Assert::IsTrue(vc->size() == 27);
		}

		TEST_METHOD(TestServerException) {
			string message = "Error";
			HRESULT hRes = 123;

			ServerException se(message.c_str(), hRes);
			string msg = string(se.message());

			Assert::AreEqual(message, msg);
			Assert::AreEqual(hRes, se.hResult());

			Assert::AreEqual(hRes, (HRESULT)se);
			Assert::IsTrue(se == hRes);
			Assert::IsTrue(hRes == se);
		}

		TEST_METHOD(TestGetOPCServerByName) {
			list<OPCServer*> lst;
			lst.push_back(new OPCServer("Kepware"));
			lst.push_back(new OPCServer("InSAT.ModbusOPCServer.DA"));
			OPCServer* srv = OPCEnum::GetOPCServerByName("InSAT", &lst);
			Assert::IsNotNull(srv);
			srv = OPCEnum::GetOPCServerByName("Nothing", &lst);
			Assert::IsNull(srv);
		}

		TEST_METHOD(TestGetAuthIdentity)
		{
			COAUTHIDENTITY* pAuthIdentity = OPCEnum::GetAuthIdentity("Brad");
			Assert::IsNotNull(pAuthIdentity->User);
			Assert::IsNull(pAuthIdentity->Password);
			Assert::IsNull(pAuthIdentity->Domain);
			pAuthIdentity = OPCEnum::GetAuthIdentity("Alex", "123");
			Assert::IsNotNull(pAuthIdentity->User);
			Assert::IsNotNull(pAuthIdentity->Password);
			Assert::IsNull(pAuthIdentity->Domain);
			string user = "John";
			string password = "111";
			string domain = "microsoft.com";
			pAuthIdentity = OPCEnum::GetAuthIdentity(user, password, domain);
			Assert::IsNotNull(pAuthIdentity->User);
			Assert::IsNotNull(pAuthIdentity->Password);
			Assert::IsNotNull(pAuthIdentity->Domain);
			Assert::AreEqual((ULONG)user.length(), pAuthIdentity->UserLength);
			Assert::AreEqual((ULONG)password.length(), pAuthIdentity->PasswordLength);
			Assert::AreEqual((ULONG)domain.length(), pAuthIdentity->DomainLength);
			wstring wUsername = wstring((wchar_t*)(pAuthIdentity->User), (size_t)pAuthIdentity->UserLength);
			wstring wPassword = wstring((wchar_t*)(pAuthIdentity->Password), (size_t)pAuthIdentity->PasswordLength);
			wstring wDomain = wstring((wchar_t*)(pAuthIdentity->Domain), (size_t)pAuthIdentity->DomainLength);
			Assert::IsTrue(user == string(wUsername.begin(), wUsername.end()));
			Assert::IsTrue(password == string(wPassword.begin(), wPassword.end()));
			Assert::IsTrue(domain == string(wDomain.begin(), wDomain.end()));
		}
	};
}
