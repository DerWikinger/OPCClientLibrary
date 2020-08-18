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

	OPCSecurity security(host, username, password, domain);
	COSERVERINFO* pHostInfo = security.GetServerInfo();

	MULTI_QI* pResults = new MULTI_QI();
	pResults->pIID = &IID_IOPCServerList;

	DWORD clsCTX = CLSCTX_LOCAL_SERVER;
	if (host != "localhost" && host != "127.0.0.1") {
		clsCTX = CLSCTX_INPROC;
	}
	hRes = CoInitialize(NULL);
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

	CoUninitialize();
	return result;
}

OPCServer* OPCEnum::GetOPCServerByName(const string& name, const list<OPCServer*>* lst) {
	OPCServer* pResult = 0;

	for_each(lst->begin(), lst->end(), [&](OPCServer* srv) mutable {
		if (~srv->Name().find(name)) pResult = srv;
		});

	return pResult;
}