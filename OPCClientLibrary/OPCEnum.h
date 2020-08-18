#pragma once

#include <list>
#include "OPCServer.h"

namespace OPCClientLibrary {
	
	using namespace std;

	static class OPCCLIENTLIBRARY_API OPCEnum
	{
	private:
		void virtual abstract() = 0;
	public:
		static list<OPCServer*>* BrowseOPCServers(const string& host, const string& username = "",
			const string& password = "", const string& domain = "");

		static OPCServer* GetOPCServerByName(const string&, const list<OPCServer*>*);
	};
}

