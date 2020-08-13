#include "pch.h"
#include "CppUnitTest.h"
#include "OPCServer.h"
#include "OPCEnum.h"
#include "OPCGroup.h"
#include "OPCItem.h"
#include <list>
#include <algorithm>
#include "ServerException.h"
#include <thread>
#include <chrono>

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
		//TEST_METHOD_INITIALIZE(Com_Init) {
		//	com_initialize();
		//}
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

			auto func = [&]() mutable -> list<OPCServer*>* {
				return OPCEnum::BrowseOPCServers(hostName);
			};

			//Assert::ExpectException<list<OPCServer*>>(func);
			Assert::ExpectException<HRESULT>(func);
			//Assert::ExpectException<ServerException>(func);
		}

		void com_initialize() {
			HRESULT hRes;
			hRes = CoInitialize(NULL);
			hRes = CoInitializeSecurity(
				NULL,
				-1,
				NULL,
				NULL,
				RPC_C_AUTHN_LEVEL_CONNECT,
				RPC_C_IMP_LEVEL_IMPERSONATE,
				NULL,
				EOAC_NONE,
				NULL);
		}
		TEST_METHOD(TestBrowseRemoteServers)
		{
			return; //skip the test

			string hostName = "192.168.43.250";
			//string username = "ETL";
			//string password = "123";
			COSERVERINFO* sInfo = OPCEnum::GetHostInfo(hostName); //Work only anonymous user !!!
			//list<OPCServer*>* lst = OPCEnum::BrowseOPCServers(hostName, username, password);
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
			vector<OPCGroup*>* groups = GetGroups(*srv);

			for_each(groups->begin(), groups->end(), [&](OPCGroup* group) {
				if (group->Items().size() == 8) return; //Это костыль, чтобы обойти создание последней группы
				ULONG phServerGroup = srv->AddGroup(*group);
				(*group).AddItems();
				Assert::IsTrue(phServerGroup != 0);
				Assert::IsNotNull(group->ItemMgt());
				(*group).RemoveItems();
				srv->RemoveGroup(*group);
				});
			srv->Disconnect();
		}

		TEST_METHOD(TestGetState)
		{
			return;
		}

		TEST_METHOD(TestRemoteSyncRead) {
			return; //skip the test
			OPCServer* srv = OPCEnum::GetOPCServerByName("InSAT", OPCEnum::BrowseOPCServers("192.168.43.250"));
			srv->Connect();
			vector<OPCGroup*>* groups = GetGroups(*srv);
			if (groups->size() > 0) {
				OPCGroup group = *groups->at(4);
				ULONG phServerGroup = srv->AddGroup(group);
				group.AddItems();
				group.SyncRead();
				OPCItem* pItem = group.Items().at(0);
				USHORT qty = pItem->Quality();
				_FILETIME ftTimeStamp1 = pItem->TimeStamp();
				Assert::IsTrue(OPC_QUALITY_GOOD == qty);
				std::cout << ConvertToString(pItem->Value()) << std::endl;
				LPWSTR strVal = _com_util::ConvertStringToBSTR(ConvertToString(pItem->Value()).c_str());
				MessageBox(0, strVal, L"Value1", 0);
				Sleep(5000);
				group.SyncRead();
				_FILETIME ftTimeStamp2 = pItem->TimeStamp();
				std::cout << ConvertToString(pItem->Value()) << std::endl;
				strVal = _com_util::ConvertStringToBSTR(ConvertToString(pItem->Value()).c_str());
				MessageBox(0, strVal, L"Value2", 0);
				Assert::IsFalse(ftTimeStamp1.dwLowDateTime == ftTimeStamp2.dwLowDateTime);
				group.RemoveItems();
				srv->RemoveGroup(group);
			}
			srv->Disconnect();
		}

		string ConvertToString(VARIANT value) {
			switch (value.vt) {
			case VT_R4:
				return std::to_string(value.fltVal);
			case VT_I2:
			case VT_I4:
				return std::to_string(value.intVal);
			case VT_BSTR:
				return _com_util::ConvertBSTRToString(value.bstrVal);
			default:
				return "";
			}
		}

		TEST_METHOD(TestRemoteSyncRead) {
			OPCServer* srv = OPCEnum::GetOPCServerByName("InSAT", OPCEnum::BrowseOPCServers("192.168.43.250"));
			srv->Connect();
			vector<OPCGroup*>* groups = GetGroups(*srv);
			if (groups->size() > 0) {
				OPCGroup group = *groups->at(4);
				ULONG phServerGroup = srv->AddGroup(group);
				group.AddItems();
				group.SyncRead();
				OPCItem* pItem = group.Items().at(0);
				USHORT qty = pItem->Quality();
				_FILETIME ftTimeStamp1 = pItem->TimeStamp();
				Assert::IsTrue(OPC_QUALITY_GOOD == qty);
				std::cout << ConvertToString(pItem->Value()) << std::endl;
				LPWSTR strVal = _com_util::ConvertStringToBSTR(ConvertToString(pItem->Value()).c_str());
				MessageBox(0, strVal, L"Value1", 0);
				Sleep(5000);
				group.SyncRead();
				_FILETIME ftTimeStamp2 = pItem->TimeStamp();
				std::cout << ConvertToString(pItem->Value()) << std::endl;
				strVal = _com_util::ConvertStringToBSTR(ConvertToString(pItem->Value()).c_str());
				MessageBox(0, strVal, L"Value2", 0);
				Assert::IsFalse(ftTimeStamp1.dwLowDateTime == ftTimeStamp2.dwLowDateTime);
				group.RemoveItems();
				srv->RemoveGroup(group);
			}
			srv->Disconnect();
		}

		string ConvertToString(VARIANT value) {
			switch (value.vt) {
			case VT_R4:
				return std::to_string(value.fltVal);
			case VT_I2:
			case VT_I4:
				return std::to_string(value.intVal);
			case VT_BSTR:
				return _com_util::ConvertBSTRToString(value.bstrVal);
			default:
				return "";
			}
		}

		TEST_METHOD(TestSyncRead) {
			OPCServer* srv = OPCEnum::GetOPCServerByName("InSAT", OPCEnum::BrowseOPCServers("localhost"));
			srv->Connect();
			vector<OPCGroup*>* groups = GetGroups(*srv);
			if (groups->size() > 0) {
				OPCGroup group = *groups->at(0);
				ULONG phServerGroup = srv->AddGroup(group);
				group.AddItems();
				group.SyncRead();
				USHORT qty = group.Items().at(0)->Quality();
				_FILETIME ftTimeStamp1 = group.Items().at(0)->TimeStamp();
				Assert::IsTrue(OPC_QUALITY_GOOD == qty);
				Sleep(1000);
				group.SyncRead();
				_FILETIME ftTimeStamp2 = group.Items().at(0)->TimeStamp();
				Assert::IsFalse(ftTimeStamp1.dwLowDateTime == ftTimeStamp2.dwLowDateTime);
				group.RemoveItems();
				srv->RemoveGroup(group);
			}
			srv->Disconnect();
		}

		TEST_METHOD(TestAsyncRead) {
			OPCServer* srv = OPCEnum::GetOPCServerByName("InSAT", OPCEnum::BrowseOPCServers("localhost"));
			srv->Connect();
			vector<OPCGroup*>* groups = GetGroups(*srv);
			if (groups->size() > 0) {
				OPCGroup group = *groups->at(0);
				ULONG phServerGroup = srv->AddGroup(group, 200);
				group.Advise();
				group.AddItems();
				auto wait = [] {
					Sleep(1000);
				};

				for (int i = 0; i < 5; i++) {
					group.AsyncRead();
					std::this_thread::sleep_for(500ms);
					//wait();
					OPCItem& pItem1 = *group.Items().at(1); //Sin
					_FILETIME ftTimeStamp1(pItem1.TimeStamp());
					float value1(pItem1.Value().fltVal);
					group.AsyncRead();
					//wait();
					std::this_thread::sleep_for(500ms);
					OPCItem& pItem2 = *group.Items().at(1); //Sin
					_FILETIME ftTimeStamp2(pItem2.TimeStamp());
					USHORT qty = pItem2.Quality();
					float value2(pItem2.Value().fltVal);

					if (qty != OPC_QUALITY_GOOD) MessageBox(0, L"Качество чтения не GOOD", L"Ошибка", 0);
					if (ftTimeStamp1.dwHighDateTime == ftTimeStamp2.dwHighDateTime &&
						ftTimeStamp1.dwLowDateTime == ftTimeStamp2.dwLowDateTime) MessageBox(0, L"Метка чтения не поменялась", L"Ошибка", 0);
				}
				group.RemoveItems();
				group.Unadvise();
				srv->RemoveGroup(group);
			}
			srv->Disconnect();

		}

		LPWSTR GetQuality(USHORT qty) {
			switch (qty)
			{
			case 0x00: return L"Bad";
			case 0x04: return L"Config Error";
			case 0x08: return L"Not Connected";
			case 0x0C: return L"Device Failure";
			case 0x10: return L"Sensor Failure";
			case 0x14: return L"Last Known";
			case 0x18: return L"Comm Failure";
			case 0x1C: return L"Out of Service";
			case 0x20: return L"Initializing";
			case 0x40: return L"Uncertain";
			case 0x44: return L"Last Usable";
			case 0x50: return L"Sensor Calibration";
			case 0x54: return L"EGU Exceeded";
			case 0x58: return L"Sub Normal";
			case 0xC0: return L"Good";
			case 0xD8: return L"Local Override"; default: return L"Unknown";
			}
		}

		LPWSTR GetTime(_FILETIME ft) {
			DWORD hDT = ft.dwHighDateTime;
			DWORD lDT = ft.dwLowDateTime;
			string result = std::to_string(hDT) + " : " + std::to_string(lDT);
			BSTR bstr = _com_util::ConvertStringToBSTR(result.c_str());
			return bstr;
			//COleDateTime dt = COleDateTime(ft);
			//return dt.Format(L"%Y-%m-%d %H:%M:%S");
		}

		vector<OPCGroup*>* GetGroups(OPCServer& server) {
			vector<OPCItem*>* vec = server.GetItems();
			vector<OPCItem*> filtred;
			vector<OPCGroup*>* groups = new vector<OPCGroup*>();

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
						groups->push_back(new OPCGroup(item->ItemID(), filtred));
					}
				}
				});
			return groups;
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
