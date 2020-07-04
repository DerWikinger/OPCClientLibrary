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

const CLSID OPCServer::clsid() {
	return this->_clsid;
}

const CLSID OPCServer::clsid(const CLSID& value) {
	return this->_clsid = value;
}

const CLSID OPCServer::clsidcat() {
	return this->_clsidcat;
}

const CLSID OPCServer::clsidcat(const CLSID& value) {
	return this->_clsidcat = value;
}

const string OPCServer::ToString() {
	return "OPCServerObject";
	return this->_name;
}

