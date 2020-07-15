#include "pch.h"
#include "CppUnitTest.h"
#include "OPCServer.h"
#include "OPCEnum.h"
#include "OPCGroup.h"
#include "OPCItem.h"
#include <list>
#include <algorithm>
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
			Assert::AreEqual(srv.Name(), name);
			string newName = "NewServer";
			//LPWSTR newName = L"NewServer";
			srv.Name(newName);
			Assert::AreEqual(srv.Name(), newName);
			string anotherName = "AnotherServer";
			//LPWSTR anotherName = L"AnotherServer";
			//OPCServer srv2 = anotherName = "AnotherServer";
			OPCServer srv2(anotherName);
			Assert::AreEqual(srv2.Name(), anotherName);
			OPCServer* pSrv = &srv;
			OPCServer* pSrv2 = &srv2;
			Assert::IsTrue(pSrv == pSrv);
			Assert::IsFalse(pSrv == pSrv2);

			IID IUKNOWN_IID = __uuidof(IUnknown);
			srv.Guid(&IUKNOWN_IID);
			Assert::IsTrue(*srv.Guid() == IUKNOWN_IID);
			
			Assert::AreEqual(DWORD(CLSCTX_LOCAL_SERVER), srv.ClsCTX());
			DWORD clsCTX = CLSCTX_ALL;
			srv.ClsCTX(clsCTX);
			Assert::AreNotEqual(DWORD(CLSCTX_LOCAL_SERVER), srv.ClsCTX());
			Assert::AreEqual(DWORD(CLSCTX_ALL), srv.ClsCTX());
		}

		TEST_METHOD(TestBrowseServers)
		{
			string hostName = "127.0.0.1";
			list<OPCServer*>* lst = OPCEnum::BrowseOPCServers(hostName);
			Assert::IsFalse(lst->size() == 0);

			hostName = "uncorrectAddress";			
			
			auto func = [&] () mutable -> list<OPCServer*>* {
				return OPCEnum::BrowseOPCServers(hostName);
			};

			//Assert::ExpectException<list<OPCServer*>>(func);
			Assert::ExpectException<HRESULT>(func);
			//Assert::ExpectException<ServerException>(func);
		}

		
		TEST_METHOD(TestBrowseRemoteServers)
		{
			return; //skip the test
			string hostName = "192.168.43.250";
			string username = "ETL";
			string password = "123";
			COSERVERINFO* sInfo = OPCEnum::GetHostInfo(hostName, username, password);
			OPCServer server("InSAT.ModbusOPCServer.DA");
			GUID guid;
			// InSAT.ModbusOPCServer.DA
			CLSIDFromString(L"{F5EB9AFF-96EA-403F-B129-65235F8BB8B8}", &guid);
			server.Guid(&guid);
			server.ClsCTX(CLSCTX_INPROC);
			server.ServerInfo(sInfo);
			server.Connect();
			vector<OPCItem*>* l = server.GetItems();
			server.Disconnect();
			//list<OPCServer*>* lst = OPCEnum::BrowseOPCServers(hostName, username, password);
			//Assert::IsFalse(lst->size() == 0);
		}

		TEST_METHOD(TestServerItems)
		{
			Sleep(3000);
			OPCServer* srv = OPCEnum::GetOPCServerByName("InSAT", OPCEnum::BrowseOPCServers("localhost"));
			srv->Connect();
			vector<OPCItem*>* vc = srv->GetItems();
			Assert::IsTrue(vc->size() > 0);
			srv->Disconnect();
		}

		TEST_METHOD(TestOPCItem)
		{
			string name = "ItemA";
			OPCItem item(name);
			ULONG hItem = item.ClientItem();
			Assert::AreEqual(name, item.Name());
			Assert::IsTrue(item.ClientItem() > 0);
			OPCItem itemB("ItemB");
			Assert::AreEqual(string("ItemB"), itemB.Name());
			Assert::IsTrue(itemB.ClientItem() - 1 == hItem);
		}

		TEST_METHOD(TestOPCGroup)
		{
			string name = "OPCGroup";
			vector<OPCItem*> vec = *new vector<OPCItem*>();
			vec.push_back(new OPCItem("ItemA"));
			vec.push_back(new OPCItem("ItemB"));
			OPCGroup group(name, vec);
			Assert::IsTrue(name == group.Name());
			Assert::IsTrue(vec == group.Items());
			Assert::IsTrue(vec[0]->Name() == "ItemA");
			Assert::IsTrue(vec[1]->Name() == "ItemB");
		}

		TEST_METHOD(TestAddGroup)
		{
			OPCServer* srv = OPCEnum::GetOPCServerByName("InSAT", OPCEnum::BrowseOPCServers("localhost"));
			srv->Connect();
			vector<OPCItem*>* vec = srv->GetItems();
			vector<OPCItem*> filtred;
			vector<OPCGroup*> groups;
			
			for_each(vec->begin(), vec->end(), [&](OPCItem* item) {
				if (item->ItemType() == tagOPCBROWSETYPE::OPC_BRANCH) {
					int count = 0;
					filtred.clear();
					for_each(vec->begin(), vec->end(), [&](OPCItem* it) {
						if (it->ItemType() == tagOPCBROWSETYPE::OPC_LEAF && it->Parent()->ItemID() == item->ItemID()) {
							filtred.push_back(it);
							count++;
						}
						});
					if (count > 0) {
						groups.push_back(new OPCGroup(item->ItemID(), filtred));
					}
				}
				});
			for_each(groups.begin(), groups.end(), [&](OPCGroup* group) {
				ULONG phServerGroup = srv->AddGroup(*group);
				Assert::IsTrue(phServerGroup != 0);
				Assert::IsNotNull(group->ItemMgt());
				srv->RemoveGroup(phServerGroup);
				});
			srv->Disconnect();
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
