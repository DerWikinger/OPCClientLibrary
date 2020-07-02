#pragma once

#include "pch.h"

using namespace std;

using namespace OpcEnumLib;
using namespace OPCDA;


class __declspec(dllexport) OPCServer
{
private:
	IUnknown* srv;
	IOPCServer* server;
public:
	string name;
	OPCServer() : name("") {}
	OPCServer(string name) : name(name) {}
	virtual ~OPCServer() {}
	void Connect();
	void Disconnect();
};

