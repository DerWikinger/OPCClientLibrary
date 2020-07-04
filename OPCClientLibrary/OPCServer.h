#pragma once

#include "pch.h"

namespace OPCServerLibrary {

	using namespace std;
	using namespace OpcEnumLib;
	using namespace OPCDA;


	class __declspec(dllexport) OPCServer
	{
	private:
		IUnknown* srv;
		IOPCServer* server;
		string _name;
		CLSID _clsid;
		CLSID _clsidcat;
	public:
		OPCServer() : _name("") {}
		explicit OPCServer(const string& name) : _name(name) {}
		
		const string name();
		const string name(const string& value);
		const CLSID clsid();
		const CLSID clsid(const CLSID& value);
		const CLSID clsidcat();
		const CLSID clsidcat(const CLSID& value);
		virtual ~OPCServer() {
			delete srv;
			delete server;
		}
		void Connect();
		void Disconnect();
		const string ToString();
	};
}

