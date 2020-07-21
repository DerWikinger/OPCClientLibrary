#include "pch.h"
#include "OPCEnum.h"
#include "OPCServer.h"
#include <algorithm>
#include "ServerException.h"

using namespace OPCClientLibrary;

list<OPCServer*>* OPCEnum::BrowseOPCServers(const string& host, const string& username, const string& password, const string& domain) {

	//Идентификатор интерфейса IOPCServerList
	IID IID_IOPCServerList = __uuidof(IOPCServerList);

	CLSID clsid;
	CLSID clsidcat;
	HRESULT hRes;
	IOPCServerList* pServerList = 0;

	// Идентификатор компонента просмотра списка серверов
	hRes = CLSIDFromProgID(L"OPC.ServerList", &clsid);
	//hRes = CLSIDFromString(L"{B28EEDB1-AC6F-11D1-84D5-00608CB8A7E9}", &clsid);
	//"OPC.ServerList"
	hRes = CLSIDFromString(L"{13486D51-4821-11D2-A494-3CB306C10000}", &clsid);

	COSERVERINFO* pHostInfo = GetHostInfo(host, username, password, domain);

	//CLSID opcEnumID;
	//hRes = CLSIDFromString(L"{13486D50-4821-11D2-A494-3CB306C10000}", &opcEnumID);

	MULTI_QI* pResults = new MULTI_QI();
	pResults->pIID = &IID_IOPCServerList;

	DWORD clsCTX = CLSCTX_LOCAL_SERVER;
	if (host != "localhost" && host != "127.0.0.1") {
		clsCTX = CLSCTX_INPROC;
	}

	hRes = CoCreateInstanceEx(clsid, NULL, clsCTX, pHostInfo, 1, pResults);
	if (FAILED(hRes)) {
		//throw new std::exception("Ошибка сервера", hRes);
		throw hRes;
		//throw new ServerException("Ошибка сервера", hRes);
	}
	pServerList = (IOPCServerList*)pResults->pItf;
	IEnumGUID* pIOPCEnumGuid;
	unsigned long ccomp = 1;
	// Идентификатор категории ОРС DA 2.0
	hRes = CLSIDFromString(L"{63D5F432-CFE4-11D1-B2C8-0060083BA1FB}", &clsidcat);
	if (FAILED(hRes)) {
		//throw new ServerException("Ошибка сервера", hRes);
		throw hRes;
	}
	pServerList->EnumClassesOfCategories(ccomp, &clsidcat, 0, NULL, &pIOPCEnumGuid);

	list<LPWSTR> lst;

	LPWSTR pszProgID;
	LPWSTR pszUserType;

	GUID guid;
	int nServerCnt = 0;
	unsigned long iRetSvr;

	list<OPCServer*>* result = new list<OPCServer*>();
	((IOPCEnumGUID*)pIOPCEnumGuid)->Next(1, &guid, &iRetSvr);
	while (iRetSvr != 0)
	{
		nServerCnt++;
		pServerList->GetClassDetails(&guid, &pszProgID, &pszUserType);

		wstring ws = pszProgID;
		string name = string(ws.begin(), ws.end());
		OPCServer* server = new OPCServer(name);
		result->push_back(server);
		GUID* pGuid = new GUID;
		//создаем область памяти, чтобы хранить идентификатор в привязке к строке списка
		memcpy(pGuid, &guid, sizeof(guid));
		server->Guid(pGuid);
		server->ServerInfo(pHostInfo);
		server->ClsCTX(clsCTX);
		//связываем элемент списка и указатель на идентификатор
		((IOPCEnumGUID*)pIOPCEnumGuid)->Next(1, &guid, &iRetSvr); // получаем следующий сервер
	}

	return result;
}

COSERVERINFO* OPCEnum::GetHostInfo(const string& hostname, const string& username, const string& password,
	const string& domain)
{
	COAUTHINFO* pAuthInfo = NULL;
	COAUTHIDENTITY* pAuthIdentity = NULL;
	if (username != "") {
		pAuthIdentity = OPCEnum::GetAuthIdentity(username, password, domain);

		pAuthInfo = new COAUTHINFO();
		pAuthInfo->pAuthIdentityData = pAuthIdentity;
		//pAuthInfo->dwAuthnSvc = RPC_C_AUTHN_WINNT;
		pAuthInfo->dwAuthnSvc = RPC_C_AUTHN_WINNT;
		pAuthInfo->pwszServerPrincName = NULL;
		pAuthInfo->dwAuthnLevel = RPC_C_AUTHN_LEVEL_CONNECT;
		//pAuthInfo->dwAuthnLevel = RPC_C_AUTHN_DEFAULT;
		pAuthInfo->dwImpersonationLevel = RPC_C_IMP_LEVEL_IMPERSONATE;
		pAuthInfo->dwCapabilities = EOAC_NONE;
		pAuthInfo->dwAuthzSvc = RPC_C_AUTHZ_NONE;
	}

	COSERVERINFO* pHostInfo = new COSERVERINFO();
	pHostInfo->dwReserved1 = 0;
	pHostInfo->dwReserved2 = 0;
	BSTR name = _com_util::ConvertStringToBSTR(hostname.c_str());
	pHostInfo->pwszName = name;
	pHostInfo->pAuthInfo = pAuthInfo;

	return pHostInfo;
}

COAUTHIDENTITY* OPCEnum::GetAuthIdentity(const string& username, const string& password, const string& domain) {

	COAUTHIDENTITY* pResult = new COAUTHIDENTITY();
	pResult->Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;
	//pResult->Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;

	ULONG usernameLength = username.length();
	USHORT* pUsername = 0;
	if (usernameLength > 0) {
		pUsername = new USHORT[usernameLength];
		for (int i = 0; i < usernameLength; i++)
		{
			pUsername[i] = username.at(i);
		}
		pResult->User = pUsername;
		pResult->UserLength = usernameLength;
	}

	ULONG passwordLength = password.length();
	USHORT* pPassword = 0;
	if (passwordLength > 0) {
		pPassword = new USHORT[passwordLength];
		for (int i = 0; i < passwordLength; i++)
		{
			pPassword[i] = password.at(i);
		}
		pResult->Password = pPassword;
		pResult->PasswordLength = passwordLength;
	}

	ULONG domainLength = domain.length();
	USHORT* pDomain = 0;
	if (domainLength > 0) {
		pDomain = new USHORT[domainLength];
		for (int i = 0; i < domainLength; i++)
		{
			pDomain[i] = domain.at(i);
		}
		pResult->Domain = pDomain;
		pResult->DomainLength = domainLength;
	}

	return pResult;
}

OPCServer* OPCEnum::GetOPCServerByName(const string& name, const list<OPCServer*>* lst) {
	OPCServer* pResult = 0;

	for_each(lst->begin(), lst->end(), [&](OPCServer* srv) mutable {
		if (~srv->Name().find(name)) pResult = srv;
		});

	return pResult;
}