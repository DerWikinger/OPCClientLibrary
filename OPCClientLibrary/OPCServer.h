#pragma once

#include <vector>
#include "OPCItem.h"
#include "OPCGroup.h"
#include "OPCSecurity.h"

#ifdef OPCCLIENTLIBRARY_EXPORTS
#define OPCCLIENTLIBRARY_API __declspec(dllexport)
#else
#define OPCCLIENTLIBRARY_API __declspec(dllimport)
#endif

namespace OPCClientLibrary {

	using namespace std;
	using namespace OpcEnumLib;
	using namespace OPCDA;

	class OPCCLIENTLIBRARY_API OPCServer
	{
	private:
		IOPCServer* _server = 0;
		string _name = "Nonamed OPCServer";
		COSERVERINFO* _serverInfo = NULL;
		GUID* _guid = 0;
		DWORD _clsCTX = CLSCTX_LOCAL_SERVER;
		OPCSecurity _security;

		void itemsChildren(vector<OPCItem*>*, IOPCBrowseServerAddressSpace*, LPWSTR, OPCItem* pParentItem = 0);
	public:
		OPCServer() {}
		explicit OPCServer(const string &name) : _name(name) {}
		
		const string Name();
		const string Name(const string&);
		const GUID* Guid();
		const GUID* Guid(GUID*);
		const DWORD ClsCTX();
		const DWORD ClsCTX(DWORD);
		const COSERVERINFO* ServerInfo();
		const COSERVERINFO* ServerInfo(COSERVERINFO*);
		virtual ~OPCServer() {
			Disconnect();
		}

		void Connect();
		void Disconnect();
		vector<OPCItem*>* GetItems();

		const ULONG AddGroup(OPCGroup&, ULONG updateRate = 1000);
		const void RemoveGroup(OPCGroup&);

		const VARENUM GetItemDataType(const string&);

		const string ToString();
	};
}