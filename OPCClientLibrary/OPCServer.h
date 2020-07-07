#pragma once

#include "pch.h"
#include <list>

namespace OPCServerLibrary {

	using namespace std;
	using namespace OpcEnumLib;
	using namespace OPCDA;

	class __declspec(dllexport) OPCServer
	{
	private:
		IUnknown* _server = 0;
		//IOPCServer* server;
		string _name = "Nonamed server";
		CLSID _clsid;
		CLSID _clsidcat;

		//static COSERVERINFO* getServerInfo(const LPWSTR& serverName);
	public:
		OPCServer() {}
		explicit OPCServer(const string& name) : _name(name) {}
		
		const string name();
		const string name(const string& value);
		const CLSID clsid();
		const CLSID clsid(const CLSID& value);
		const CLSID clsidcat();
		const CLSID clsidcat(const CLSID& value);
		//const COSERVERINFO* serverInfo() { return pServerInfo; };
		virtual ~OPCServer() {
			//delete srv;
			//delete server;
		}
		void Connect();
		void Disconnect();
		const string ToString();
		
		static COAUTHIDENTITY* GetAuthIdentity(const string& username = "", const string& password = "", const string& domain = "");
		static list<OPCServer> BrowseOPCServers(const string& host, const string& username = "", const string& password = "", const string& domain = "");
	};
}

