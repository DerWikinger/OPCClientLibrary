#include "pch.h"
#include "CppUnitTest.h"
#include "OPCServer.h"
#include "OPCEnum.h"
#include "OPCGroup.h"
#include "OPCItem.h"
#include "Converter.h"

#include <list>
#include <algorithm>
#include "ServerException.h"
#include <thread>
#include <chrono>
#include <regex>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace OPCClientLibrary;
using namespace OPCDA;
using namespace OpcEnumLib;

namespace OPCClientLibraryTests
{
	TEST_CLASS(OPCClientLibraryTests)
	{
	private:
		string _hostname;
		string _username;
		string _password;
		string _domain;
		string _serverName;
		tagEOLE_AUTHENTICATION_CAPABILITIES _capabilities;

	public:		
		TEST_METHOD_INITIALIZE(init) {

			FILE* f = new FILE();
			errno_t err = fopen_s(&f, "opc-server.cfg", "r");

			char* bufPos = (char*)malloc(1024);
			char* bufStart = bufPos;
			int length = 0;
			if (!err) {
				while (!feof(f)) {
					fread_s(bufPos++, 1, 1, 1, f);
					length++;
				}
				fclose(f);
			}
			*(--bufPos) = '\n';

			vector<string> rows;
			string row = "";
			bufPos = bufStart;
			int count = length;
			while (count--) {
				char c = *bufPos++;
				row.append(1, c);
				if (c == '\n') {
					if (row.size() > 1) rows.push_back(row);
					row = "";
				}
			}

			auto find = [&](string row) mutable {
				string value = "";

				regex reg("^#"); // Comment
				bool match = regex_search(row.begin(), row.end(), reg);
				if (match) return;

				reg.assign("^hostname:");
				match = regex_search(row.begin(), row.end(), reg);
				if (match) {
					int index = row.find_first_of(':');
					if (~index) {
						value = row.substr(index + 1);
						value = trim(value);
						_hostname = value;
					}
				}

				reg.assign("^username:");
				match = regex_search(row.begin(), row.end(), reg);
				if (match) {
					int index = row.find_first_of(':');
					if (~index) {
						value = row.substr(index + 1);
						value = trim(value);
						_username = value;
					}
				}

				reg.assign("^password:");
				match = regex_search(row.begin(), row.end(), reg);
				if (match) {
					int index = row.find_first_of(':');
					if (~index) {
						value = row.substr(index + 1);
						value = trim(value);
						_password = value;

					}
				}

				reg.assign("^server-name:");
				match = regex_search(row.begin(), row.end(), reg);
				if (match) {
					int index = row.find_first_of(':');
					if (~index) {
						value = row.substr(index + 1);
						value = trim(value);
						_serverName = value;
					}
				}

				reg.assign("^capabilities:");
				match = regex_search(row.begin(), row.end(), reg);
				if (match) {
					int index = row.find_first_of(':');
					if (~index) {
						value = row.substr(index + 1);
						value = trim(value);
						_capabilities = tagEOLE_AUTHENTICATION_CAPABILITIES(atoi(value.c_str()));
					}
				}
			};

			for_each(rows.begin(), rows.end(), find);
			free(bufPos = bufStart);
		}

		string trim(string str) {
			string value = str;
			regex reg("^\\s+");
			value = regex_replace(value, reg, ""); // ltrim
			reg.assign("\\s+$");
			value = regex_replace(value, reg, ""); // rtrim
			reg.assign("^[\",']");
			value = regex_replace(value, reg, "");
			reg.assign("[\",']$");
			value = regex_replace(value, reg, "");
			return value;
		}

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
		TEST_METHOD(TestBrowseRemoteServers)
		{
			return; //skip the test
			OPCSecurity security(_hostname);
			COSERVERINFO* sInfo = security.GetServerInfo(); //Work only anonymous user !!!
			list<OPCServer*>* lst = OPCEnum::BrowseOPCServers(_hostname);
			Assert::IsFalse(lst->size() == 0);
		}

		TEST_METHOD(TestServerItems)
		{
			Sleep(3000);
			OPCServer* srv = OPCEnum::GetOPCServerByName(_serverName, OPCEnum::BrowseOPCServers("localhost"));
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
			OPCServer* srv = OPCEnum::GetOPCServerByName(_serverName, OPCEnum::BrowseOPCServers("localhost"));
			srv->Connect();
			vector<OPCGroup*>* groups = GetGroups(*srv);

			for_each(groups->begin(), groups->end(), [&](OPCGroup* group) {
				if (group->Items().size() == 8) return; //��� �������, ����� ������ �������� ��������� ������
				ULONG phServerGroup = srv->AddGroup(*group);
				(*group).AddItems();
				Assert::IsTrue(phServerGroup != 0);
				Assert::IsNotNull(group->ItemMgt());
				(*group).RemoveItems();
				srv->RemoveGroup(*group);
				});
			srv->Disconnect();
		}

		TEST_METHOD(TestSecurity)
		{
			OPCSecurity security;
			Assert::IsTrue(security.HostName() == "localhost");
		}

		TEST_METHOD(TestRemoteSyncRead) {
			return; // skip the test
			OPCServer* srv = OPCEnum::GetOPCServerByName(_serverName, OPCEnum::BrowseOPCServers(_hostname));
			srv->Connect();
			vector<OPCGroup*>* groups = GetGroups(*srv);
			if (groups->size() > 0) {
				OPCGroup group = *groups->at(3); // �-3501
				ULONG phServerGroup = srv->AddGroup(group);
				group.AddItems();
				group.SyncRead();
				OPCItem* pItem = group.Items().at(6); // Ia
				USHORT qty = pItem->Quality();
				_FILETIME ftTimeStamp1 = pItem->TimeStamp();
				Assert::IsTrue(OPC_QUALITY_GOOD == qty);
				std::cout << Converter::ConvertToString(pItem->Value()) << std::endl;
				LPWSTR strVal = _com_util::ConvertStringToBSTR(Converter::ConvertToString(pItem->Value()).c_str());
				Logger::WriteMessage(strVal);
				//MessageBox(0, strVal, L"Value1", 0);
				Sleep(5000);
				group.SyncRead();
				_FILETIME ftTimeStamp2 = pItem->TimeStamp();
				std::cout << Converter::ConvertToString(pItem->Value()) << std::endl;
				strVal = _com_util::ConvertStringToBSTR(Converter::ConvertToString(pItem->Value()).c_str());
				Logger::WriteMessage(strVal);
				//MessageBox(0, strVal, L"Value2", 0);
				Assert::IsFalse(ftTimeStamp1.dwLowDateTime == ftTimeStamp2.dwLowDateTime);
				group.RemoveItems();
				srv->RemoveGroup(group);
			}
			srv->Disconnect();
		}

		TEST_METHOD(TestSyncRead) {
			OPCServer* srv = OPCEnum::GetOPCServerByName(_serverName, OPCEnum::BrowseOPCServers("localhost"));
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
			OPCServer* srv = OPCEnum::GetOPCServerByName(_serverName, OPCEnum::BrowseOPCServers("localhost"));
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

					if (qty != OPC_QUALITY_GOOD) Logger::WriteMessage(L"�������� ������ �� GOOD");
					if (ftTimeStamp1.dwHighDateTime == ftTimeStamp2.dwHighDateTime &&
						ftTimeStamp1.dwLowDateTime == ftTimeStamp2.dwLowDateTime) Logger::WriteMessage(L"����� ������ �� ����������");
				}
				group.RemoveItems();
				group.Unadvise();
				srv->RemoveGroup(group);
			}
			srv->Disconnect();

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
			string host = "2.2.2.2";
			string user = "John";
			string password = "111";
			string domain = "microsoft.com";
			OPCSecurity security(host, user, password, domain);
			COAUTHIDENTITY* pAuthIdentity = security.GetAuthIdentity();
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

		TEST_METHOD(TestConverter) {
			VARIANT fVar;
			VariantInit(&fVar);
			FLOAT fVal = 0.5;
			fVar.fltVal = fVal;
			fVar.vt = VT_R4;
			string strVal = Converter::ConvertToString(fVar);
			Assert::AreEqual(strVal.c_str(), "0.5");

			VARIANT dVar;
			VariantInit(&dVar);
			DOUBLE dVal = 0.555;
			dVar.dblVal = dVal;
			dVar.vt = VT_R8;
			strVal = Converter::ConvertToString(dVar);
			Assert::AreEqual(strVal.c_str(), "0.555");
			
			VARIANT sVar;
			VariantInit(&sVar);
			sVar.vt = VT_I2;
			sVar.iVal = 756;
			strVal = Converter::ConvertToString(sVar);
			Assert::AreEqual(strVal.c_str(), "756");
			
			VARIANT iVar;
			VariantInit(&iVar);
			iVar.vt = VT_I4;
			iVar.intVal = 756456;
			strVal = Converter::ConvertToString(iVar);
			Assert::AreEqual(strVal.c_str(), "756456");

			VARIANT bstrVar;
			VariantInit(&bstrVar);
			bstrVar.vt = VT_BSTR;
			bstrVar.bstrVal = _com_util::ConvertStringToBSTR("Hello");
			strVal = Converter::ConvertToString(bstrVar);
			Assert::AreEqual(strVal.c_str(), "Hello");

			_FILETIME ft;
			SYSTEMTIME dt;
			GetSystemTime(&dt);			
			SystemTimeToFileTime(&dt, &ft);
			string strDT = Converter::FileTimeToString(ft);
			regex reg("^\\d\\d\/\\d\\d\/\\d\\d\\d\\d \\d\\d:\\d\\d:\\d\\d$");
			Assert::IsTrue(regex_match(strDT, reg));

			USHORT usVal = 0x00;
			Assert::AreEqual(Converter::GetQuality(usVal), L"Bad");
			usVal = 0xC0;
			Assert::AreEqual(Converter::GetQuality(usVal), L"Good");
			usVal = 0x100;
			Assert::AreEqual(Converter::GetQuality(usVal), L"Unknown");
		}
	};
}
