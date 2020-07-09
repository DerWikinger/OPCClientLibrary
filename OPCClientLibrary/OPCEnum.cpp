#include "pch.h"
#include "OPCEnum.h"
#include "OPCServer.h"
#include <algorithm>
#include "ServerException.h"

using namespace OPCClientLibrary;

list<OPCServer*> OPCEnum::BrowseOPCServers(const string& host, const string& username, const string& password, const string& domain) {

	//Идентификатор интерфейса IOPCServerList
	IID IID_IOPCServerList = __uuidof(IOPCServerList);

	CLSID clsid;
	CLSID clsidcat;
	HRESULT hRes;
	IOPCServerList* pServerList = 0;

	// Идентификатор компонента просмотра списка серверов
	hRes = CLSIDFromProgID(L"OPC.ServerList", &clsid);

	COAUTHINFO* pAuthInfo = NULL;
	COAUTHIDENTITY* pAuthIdentity = NULL;
	if (username != "") {
		pAuthIdentity = OPCEnum::GetAuthIdentity(username, password, domain);

		pAuthInfo = new COAUTHINFO();
		pAuthInfo->pAuthIdentityData = pAuthIdentity;
		pAuthInfo->dwAuthnSvc = RPC_C_AUTHN_WINNT;
		pAuthInfo->pwszServerPrincName = NULL;
		pAuthInfo->dwAuthnLevel = RPC_C_AUTHN_LEVEL_CONNECT;
		pAuthInfo->dwImpersonationLevel = RPC_C_IMP_LEVEL_IMPERSONATE;
		pAuthInfo->dwCapabilities = EOAC_NONE;
		pAuthInfo->dwAuthzSvc = RPC_C_AUTHZ_NONE;
	}

	COSERVERINFO* pHostInfo = new COSERVERINFO();
	pHostInfo->dwReserved1 = 0;
	pHostInfo->dwReserved2 = 0;
	wstring ws(host.begin(), host.end());
	pHostInfo->pwszName = (LPWSTR)ws.c_str();
	pHostInfo->pAuthInfo = pAuthInfo;

	MULTI_QI* pResults = new MULTI_QI();
	pResults->pIID = &IID_IOPCServerList;

	DWORD clsCTX = CLSCTX_LOCAL_SERVER;
	if (host != "localhost" && host != "127.0.0.1") {
		clsCTX = CLSCTX_INPROC;
	}
	IUnknown* punkOuter = NULL;
	hRes = CoCreateInstanceEx(clsid, NULL, clsCTX, pHostInfo, 1, pResults);
	if (FAILED(hRes)) {
		//throw new std::exception("Ошибка сервера", hRes);
		throw new ServerException("Ошибка сервера", hRes);
	}
	pServerList = (IOPCServerList*)pResults->pItf;
	IEnumGUID* pIOPCEnumGuid;
	unsigned long ccomp = 1;
	// Идентификатор категории ОРС DA 2.0
	hRes = CLSIDFromString(L"{63D5F432-CFE4-11D1-B2C8-0060083BA1FB}", &clsidcat);
	if (FAILED(hRes)) {
		throw new exception("Ошибка сервера", hRes);
	}
	pServerList->EnumClassesOfCategories(ccomp, &clsidcat, 0, NULL, &pIOPCEnumGuid);

	list<OLECHAR*> lst;

	OLECHAR* pszProgID; // буфер для записи ProgID серверов
	OLECHAR* pszUserType; // буфер для записи описания серверов

	GUID guid; // Сюда будет записывать идентификатор текущего сервера
	int nServerCnt = 0; // общее количество доступных серверов
	unsigned long iRetSvr; // количество серверов, предоставленных запросом
	// получение первого доступного идентификатора сервера

	list<OPCServer*> result;
	((IOPCEnumGUID*)pIOPCEnumGuid)->Next(1, &guid, &iRetSvr);
	while (iRetSvr != 0)
	{
		nServerCnt++;
		pServerList->GetClassDetails(&guid, &pszProgID, &pszUserType);

		wstring ws = pszProgID;
		string name = string(ws.begin(), ws.end());
		OPCServer* server = new OPCServer(name);
		result.push_back(server);
		GUID* pGuid = new GUID;
		//создаем область памяти, чтобы хранить идентификатор в привязке к строке списка
		memcpy(pGuid, &guid, sizeof(guid));
		server->guid(pGuid);
		//связываем элемент списка и указатель на идентификатор
		((IOPCEnumGUID*)pIOPCEnumGuid)->Next(1, &guid, &iRetSvr); // получаем следующий сервер
	}

	return result;
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

OPCServer* OPCEnum::GetOPCServerByName(const string& name, const list<OPCServer*>& lst) {
	OPCServer* pResult = 0;

	for_each(lst.begin(), lst.end(), [&](OPCServer* srv) mutable {
		if (~srv->name().find(name)) pResult = srv;
		});

	return pResult;
}