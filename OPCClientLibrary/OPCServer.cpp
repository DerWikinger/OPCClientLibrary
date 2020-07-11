#include "pch.h"
#include "OPCServer.h"
#include <algorithm>

using namespace OPCClientLibrary;
//using namespace OPCDA;

IOPCServer* OPCServer::Connect() {

	if (_server != NULL) {
		return _server;
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

	return _server = (IOPCServer*)pResults->pItf;
}

void OPCServer::Disconnect() {
	if (_server != NULL) {
		_server->Release();
	}
	CoUninitialize();
}

const string OPCServer::Name() {
	return _name;
}

const string OPCServer::Name(const string& value) {
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
		ws = wstring(strName);
		name = string(ws.begin(), ws.end());
		while (cnt != 0)
		{
			pParent->GetItemID(strName, &szItemID); // �������� ������ ������������� ����
			ws = wstring(szItemID);
			itemID = string(ws.begin(), ws.end());
			item = new OPCItem(name);
			item->parent(pParentItem);
			item->itemType(tagOPCBROWSETYPE::OPC_LEAF);
			item->itemID(itemID);
			item->enabled(true);
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
			ws = wstring(szItemID);
			itemID = string(ws.begin(), ws.end());
			item = new OPCItem(name);
			item->parent(pParentItem);
			item->itemType(tagOPCBROWSETYPE::OPC_LEAF);
			item->itemID(itemID);
			item->enabled(true);
			pItems->push_back(item);
			pParent->ChangeBrowsePosition(tagOPCBROWSEDIRECTION::OPC_BROWSE_TO, szItemID);
			OPCServer::itemsChildren(pItems, pParent, szFilterCriteria, item);
			pParent->ChangeBrowsePosition(tagOPCBROWSEDIRECTION::OPC_BROWSE_UP, szItemID);
			pEnum->Next(1, &strName, &cnt);
			nBranchesCount++;
		}
	}
}

const string OPCServer::ToString() {
	//string res = "OPCServerObject";
	return _name;
	//return this->_name;
}




