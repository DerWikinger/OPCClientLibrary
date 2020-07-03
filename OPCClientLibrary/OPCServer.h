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
	public:
		OPCServer() : _name("") {}
		explicit OPCServer(const string& name) : _name(name) {}
		const string name();
		const string name(const string& value);
		virtual ~OPCServer() {
			delete srv;
			delete server;
		}
		void Connect();
		void Disconnect();
	};
}

