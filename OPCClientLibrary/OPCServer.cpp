#include "pch.h"
#include "OPCServer.h"
#include <algorithm>

using namespace OPCClientLibrary;
//using namespace OPCDA;

void OPCServer::Connect() {

	if (_server != NULL) {
		return;
	}
	
	IID IID_IOPCSERVER = __uuidof(IOPCServer);
	IID IID_IOPCBrowseServerAddressSpace = __uuidof(IOPCBrowseServerAddressSpace);
	HRESULT hRes;
	DWORD coInt = 0x0;
	hRes = CoInitialize(NULL);
	MULTI_QI* pResults = new MULTI_QI();
	pResults->pIID = &IID_IOPCSERVER;
	//pResults->pIID = new IID[2] { IID_IOPCSERVER, IID_IOPCBrowseServerAddressSpace };

	hRes = CoCreateInstanceEx(*_guid, NULL, _clsCTX, _serverInfo, 1, pResults);
	if (FAILED(hRes))
	{
		throw hRes;
	}

	_server = (IOPCServer*)pResults->pItf;
}

void OPCServer::Disconnect() {
	if (_server != NULL) {
		_server->Release();
		_server = NULL;
	}	
	CoUninitialize();
}

const string OPCServer::Name() {
	return _name;
}

const string OPCServer::Name(const string &value) {
	return _name = value;
}


const COSERVERINFO* OPCServer::ServerInfo() { 
	return _serverInfo; 
};

const COSERVERINFO* OPCServer::ServerInfo(COSERVERINFO* value) {
	return _serverInfo = value;
};


const GUID* OPCServer::Guid() {
	return _guid;
}

const GUID* OPCServer::Guid(GUID* value) {
	return _guid = value;
}

const DWORD OPCServer::ClsCTX() {
	return _clsCTX;
}

const DWORD OPCServer::ClsCTX(DWORD value) {
	return _clsCTX = value;
}

vector<OPCItem*>* OPCServer::GetItems()
{
	vector<OPCItem*>* pItems = new vector<OPCItem*>();

	HRESULT hRes;
	CLSID guid = *_guid;

	// IOPCBrowseServerAddressSpace
	IID IID_IOPCBrowseServerAddressSpace = __uuidof(IOPCBrowseServerAddressSpace);
	GUID sguid = IID_IOPCBrowseServerAddressSpace;
	
	IOPCBrowseServerAddressSpace* pBrowse;
	MULTI_QI* pResult = new MULTI_QI();
	pResult->pIID = &IID_IOPCBrowseServerAddressSpace;

	hRes = CoCreateInstanceEx(*_guid, NULL, _clsCTX, _serverInfo, 1, pResult);	
	//hRes = _server->QueryInterface(IID_IOPCBrowseServerAddressSpace, (void**)&pBrowse);
	if (FAILED(hRes)) {
		throw hRes;
	}
	pBrowse = (IOPCBrowseServerAddressSpace*)pResult->pItf;
	LPWSTR szFilterCriteria = (LPWSTR)L"";
	try {
		itemsChildren(pItems, pBrowse, szFilterCriteria, NULL);
	}
	catch (HRESULT h_result)
	{
		pBrowse->Release();
		throw h_result;
	}
	pBrowse->Release();
	return pItems;
}

void OPCServer::itemsChildren(vector<OPCItem*>* pItems, IOPCBrowseServerAddressSpace* pParent, LPWSTR szFilterCriteria, OPCItem* pParentItem) {

	tagOPCBROWSEELEMENT* tags = new tagOPCBROWSEELEMENT();
	
	IEnumString* pEnum = NULL;
	wchar_t* strName;
	ULONG cnt;
	LPWSTR szItemID;
	wstring ws;
	string itemID, name;
	OPCItem* item;
	HRESULT hRes;
	
	hRes = pParent->BrowseOPCItemIDs(tagOPCBROWSETYPE::OPC_LEAF, szFilterCriteria, VARENUM::VT_EMPTY, 0, &pEnum);
	if (FAILED(hRes)) throw hRes;
	int nLeavesCount = 0;
	if (hRes == S_OK) {
		pEnum->Next(1, &strName, &cnt);
		while (cnt != 0)
		{
			pParent->GetItemID(strName, &szItemID); // получает полный идентификатор тега
			ws = wstring(strName);
			name = string(ws.begin(), ws.end());
			ws = wstring(szItemID);
			itemID = string(ws.begin(), ws.end());
			item = new OPCItem(name);
			item->Parent(pParentItem);
			item->ItemType(tagOPCBROWSETYPE::OPC_LEAF);
			item->ItemID(itemID);
			item->Enabled(true);
			pItems->push_back(item);

			pEnum->Next(1, &strName, &cnt);
			nLeavesCount++;
		}
	}

	hRes = pParent->BrowseOPCItemIDs(tagOPCBROWSETYPE::OPC_BRANCH, szFilterCriteria, VARENUM::VT_EMPTY, 0, &pEnum);
	if (FAILED(hRes)) throw hRes;
	int nBranchesCount = 0;
	if (hRes == S_OK) {
		pEnum->Next(1, &strName, &cnt);
		while (cnt != 0)
		{
			pParent->GetItemID(strName, &szItemID);
			ws = wstring(strName);
			name = string(ws.begin(), ws.end());
			ws = wstring(szItemID);
			itemID = string(ws.begin(), ws.end());
			item = new OPCItem(name);
			item->Parent(pParentItem);
			item->ItemType(tagOPCBROWSETYPE::OPC_BRANCH);
			item->ItemID(itemID);
			item->Enabled(true);
			pItems->push_back(item);
			pParent->ChangeBrowsePosition(tagOPCBROWSEDIRECTION::OPC_BROWSE_TO, szItemID);
			OPCServer::itemsChildren(pItems, pParent, szFilterCriteria, item);
			pParent->ChangeBrowsePosition(tagOPCBROWSEDIRECTION::OPC_BROWSE_UP, szItemID);
			pEnum->Next(1, &strName, &cnt);
			nBranchesCount++;
		}
	}
}

const ULONG OPCServer::AddGroup(OPCGroup &group) {
	if (_server == NULL)
	{
		throw 123; // Сервер не подключен
	}
	HRESULT hRes;
	BSTR name = _com_util::ConvertStringToBSTR(group.Name().c_str());
	ULONG updateRate = 1000, hClientGroup = 1, phServerGroup;
	IID IID_IOPCItemMgt = __uuidof(IOPCItemMgt);
	IOPCItemMgt* pItemMgt = NULL;
	long bActive = 1;

	hRes = _server->AddGroup(name, bActive, updateRate, hClientGroup, NULL, NULL, 0,
		&phServerGroup, &updateRate, &IID_IOPCItemMgt, (IUnknown**)&pItemMgt);
	if (FAILED(hRes))
	{
		throw hRes;
	}
	group.ItemMgt(pItemMgt);
	
	DWORD dwCount = group.Items().size();
	ULONG opcHandle = 1;
	tagOPCITEMDEF* pItems = (tagOPCITEMDEF*)CoTaskMemAlloc(dwCount * sizeof(tagOPCITEMDEF));
	for (int i = 0; i < dwCount; i++) {
		OPCItem* item = group.Items().at(i);
		BSTR itemID = _com_util::ConvertStringToBSTR(item->ItemID().c_str());
		pItems[i].szItemID = itemID;
		pItems[i].szAccessPath = NULL;
		pItems[i].bActive = item->Enabled();
		pItems[i].hClient = item->ClientItem();
		pItems[i].vtRequestedDataType = VT_EMPTY;
		pItems[i].dwBlobSize = 0;
		pItems[i].pBlob = NULL;
	}

	tagOPCITEMRESULT* pResults = NULL;
	HRESULT* pErrors = NULL;

	hRes = pItemMgt->AddItems(dwCount, pItems, &pResults, &pErrors);
	if (FAILED(hRes))
	{
		throw hRes;
	}
	return phServerGroup;	
}

const void OPCServer::RemoveGroup(ULONG phServerGroup)
{
	if (_server == NULL)
	{
		throw 123; // Сервер не подключен
	}
	_server->RemoveGroup(phServerGroup, 1);
}

const string OPCServer::ToString() {
	//string res = "OPCServerObject";
	return _name;
	//return this->_name;
}




