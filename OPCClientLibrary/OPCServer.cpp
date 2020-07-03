#include "pch.h"
#include "OPCServer.h"

using namespace OPCServerLibrary;

void OPCServer::Connect() {

}

void OPCServer::Disconnect() {

}

const string OPCServer::name() {
	return this->_name;
}

const string OPCServer::name(const string& value) {
	return this->_name = value;
}
