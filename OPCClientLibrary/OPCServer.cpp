#include "pch.h"
#include "OPCServer.h"
#include <algorithm>

using namespace OPCClientLibrary;

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

const GUID* OPCServer::guid() {
	return this->_guid;
}

const GUID* OPCServer::guid(GUID* value) {
	return this->_guid = value;
}

const string OPCServer::ToString() {
	//string res = "OPCServerObject";
	return _name;
	//return this->_name;
}

//COSERVERINFO* OPCServer::getServerInfo(const LPWSTR& serverName) {
//	COSERVERINFO* serverInfo = new COSERVERINFO();
//	serverInfo->pwszName = serverName;
//	serverInfo->pAuthInfo = NULL;
//	return serverInfo;
//}

//list<OPCServer*> OPCServer::BrowseOPCServers(const string& host, const string& username, const string& password, const string& domain) {
//
//	//������������� ���������� IOPCServerList
//	IID IID_IOPCServerList = __uuidof(IOPCServerList);
//
//	CLSID clsid;
//	CLSID clsidcat;
//	HRESULT hRes;
//	IOPCServerList* pServerList = 0;
//
//	// ������������� ���������� ��������� ������ ��������
//	hRes = CLSIDFromProgID(L"OPC.ServerList", &clsid);
//	
//	COAUTHINFO* pAuthInfo = NULL;
//	COAUTHIDENTITY* pAuthIdentity = NULL;
//	if (username != "") {
//		pAuthIdentity = OPCServer::GetAuthIdentity(username, password, domain);
//
//		pAuthInfo = new COAUTHINFO();
//		pAuthInfo->pAuthIdentityData = pAuthIdentity;
//		pAuthInfo->dwAuthnSvc = RPC_C_AUTHN_WINNT;
//		pAuthInfo->pwszServerPrincName = NULL;
//		pAuthInfo->dwAuthnLevel = RPC_C_AUTHN_LEVEL_CONNECT;
//		pAuthInfo->dwImpersonationLevel = RPC_C_IMP_LEVEL_IMPERSONATE;
//		pAuthInfo->dwCapabilities = EOAC_NONE;
//		pAuthInfo->dwAuthzSvc = RPC_C_AUTHZ_NONE;
//	}
//
//	COSERVERINFO* pHostInfo = new COSERVERINFO();
//	pHostInfo->dwReserved1 = 0;
//	pHostInfo->dwReserved2 = 0;
//	wstring ws(host.begin(), host.end());
//	pHostInfo->pwszName = (LPWSTR)ws.c_str();
//	pHostInfo->pAuthInfo = pAuthInfo;
//	
//	MULTI_QI* pResults = new MULTI_QI();
//	pResults->pIID = &IID_IOPCServerList;
//
//	DWORD clsCTX = CLSCTX_LOCAL_SERVER;
//	if (host != "localhost" && host != "127.0.0.1") {
//		clsCTX = CLSCTX_INPROC;
//	}
//	IUnknown* punkOuter = NULL;
//	hRes = CoCreateInstanceEx(clsid, NULL, clsCTX, pHostInfo, 1, pResults);
//	if (FAILED(hRes)) {
//		throw new exception("������ �������", hRes);
//	}
//	pServerList = (IOPCServerList*)pResults->pItf;
//	IEnumGUID* pIOPCEnumGuid;
//	unsigned long ccomp = 1;
//	// ������������� ��������� ��� DA 2.0
//	hRes = CLSIDFromString(L"{63D5F432-CFE4-11D1-B2C8-0060083BA1FB}", &clsidcat);
//	if (FAILED(hRes)) {
//		throw new exception("������ �������", hRes);
//	}
//	pServerList->EnumClassesOfCategories(ccomp, &clsidcat, 0, NULL, &pIOPCEnumGuid);
//
//	list<OLECHAR*> lst;
//
//	OLECHAR* pszProgID; // ����� ��� ������ ProgID ��������
//	OLECHAR* pszUserType; // ����� ��� ������ �������� ��������
//
//	GUID guid; // ���� ����� ���������� ������������� �������� �������
//	int nServerCnt = 0; // ����� ���������� ��������� ��������
//	unsigned long iRetSvr; // ���������� ��������, ��������������� ��������
//	// ��������� ������� ���������� �������������� �������
//	
//	list<OPCServer*> result;
//	((IOPCEnumGUID*)pIOPCEnumGuid)->Next(1, &guid, &iRetSvr);
//	while (iRetSvr != 0)
//	{
//		nServerCnt++;
//		pServerList->GetClassDetails(&guid, &pszProgID, &pszUserType);
//		
//		wstring ws = pszProgID;
//		string name = string(ws.begin(), ws.end());
//		OPCServer* server =new OPCServer(name);
//		result.push_back(server);
//		GUID* pGuid = new GUID;
//		//������� ������� ������, ����� ������� ������������� � �������� � ������ ������
//		memcpy(pGuid, &guid, sizeof(guid));
//		server->guid(pGuid);
//		//��������� ������� ������ � ��������� �� �������������
//		((IOPCEnumGUID*)pIOPCEnumGuid)->Next(1, &guid, &iRetSvr); // �������� ��������� ������
//	}
//	
//	return result;
//}
//
//COAUTHIDENTITY* OPCServer::GetAuthIdentity(const string& username, const string& password, const string& domain) {
//	
//	COAUTHIDENTITY* pResult = new COAUTHIDENTITY();
//	pResult->Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;
//	//pResult->Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;
//
//	ULONG usernameLength = username.length();
//	USHORT* pUsername = 0;
//	if (usernameLength > 0) {
//		pUsername = new USHORT[usernameLength];
//		for (int i = 0; i < usernameLength; i++)
//		{
//			pUsername[i] = username.at(i);
//		}		
//		pResult->User = pUsername;
//		pResult->UserLength = usernameLength;
//	}
//
//	ULONG passwordLength = password.length();
//	USHORT* pPassword = 0;
//	if (passwordLength > 0) {
//		pPassword = new USHORT[passwordLength];
//		for (int i = 0; i < passwordLength; i++)
//		{
//			pPassword[i] = password.at(i);
//		}
//		pResult->Password = pPassword;
//		pResult->PasswordLength = passwordLength;
//	}
//
//	ULONG domainLength = domain.length();
//	USHORT* pDomain = 0;
//	if (domainLength > 0) {
//		pDomain = new USHORT[domainLength];
//		for (int i = 0; i < domainLength; i++)
//		{
//			pDomain[i] = domain.at(i);
//		}
//		pResult->Domain = pDomain;
//		pResult->DomainLength = domainLength;
//	}
//
//	return pResult;
//}
//
//OPCServer* OPCServer::GetOPCServerByName(const string& name, const list<OPCServer*> &lst) {
//	OPCServer* pResult = 0;
//
//	for_each(lst.begin(), lst.end(), [&](OPCServer* srv) mutable {
//		if(~srv->name().find(name)) pResult = srv;
//		});
//
//	return pResult;
//}




