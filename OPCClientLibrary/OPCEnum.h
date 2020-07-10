#pragma once

#include <list>
#include "OPCServer.h"

namespace OPCClientLibrary {
	
	using namespace std;

	static class __declspec(dllexport) OPCEnum
	{
	private:
		void virtual abstract() = 0;
	public:
		static COAUTHIDENTITY* GetAuthIdentity(const string& username = "", const string& password = "",
			const string& domain = "");

		static list<OPCServer*>* BrowseOPCServers(const string& host, const string& username = "",
			const string& password = "", const string& domain = "");

		static OPCServer* GetOPCServerByName(const string&, const list<OPCServer*>&);

		static COSERVERINFO GetHostInfo(const string& hostname, const string& username = "", const string& password = "",
			const string& domain = "");
	};
}

