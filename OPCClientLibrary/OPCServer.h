#pragma once

#include <list>
#include "OPCItem.h"
#include <vector>

namespace OPCClientLibrary {

	using namespace std;
	using namespace OpcEnumLib;
	using namespace OPCDA;

	class __declspec(dllexport) OPCServer
	{
	private:
		IOPCServer* _server = 0;
		string _name = "Nonamed OPCServer";
		COSERVERINFO* _serverInfo = NULL;
		GUID* _guid = 0;
		DWORD _clsCTX = CLSCTX_LOCAL_SERVER;

		void itemsChildren(vector<OPCItem*>*, IOPCBrowseServerAddressSpace*, LPWSTR, OPCItem* pParentItem = 0);
	public:
		OPCServer() {}
		explicit OPCServer(const string& name) : _name(name) {}
		
		const string name();
		const string name(const string&);
		const GUID* guid();
		const GUID* guid(GUID*);
		const DWORD clsCTX();
		const DWORD clsCTX(DWORD);
		const COSERVERINFO* serverInfo();
		const COSERVERINFO* serverInfo(COSERVERINFO*);
		virtual ~OPCServer() {
			Disconnect();
		}
		IOPCServer* Connect();
		void Disconnect();
		const string ToString();
		vector<OPCItem*>* items();
		
	};
}

