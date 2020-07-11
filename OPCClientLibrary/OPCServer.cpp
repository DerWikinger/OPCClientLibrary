#include "pch.h"
#include "OPCServer.h"
#include <algorithm>

using namespace OPCClientLibrary;
//using namespace OPCDA;

IOPCServer* OPCServer::Connect() {

	if (_server != NULL) {
		_server->Release();
	}

	IID IID_IOPCSERVER = __uuidof(IOPCServer);
	HRESULT hRes;
	MULTI_QI* pResults = new MULTI_QI();
	pResults->pIID = &IID_IOPCSERVER;

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
}

const string OPCServer::name() {
	return this->_name;
}

const string OPCServer::name(const string& value) {
	return this->_name = value;
}


const COSERVERINFO* OPCServer::serverInfo() { 
	return _serverInfo; 
};

const COSERVERINFO* OPCServer::serverInfo(COSERVERINFO* value) {
	return _serverInfo = value;
};


const GUID* OPCServer::guid() {
	return this->_guid;
}

const GUID* OPCServer::guid(GUID* value) {
	return this->_guid = value;
}

const DWORD OPCServer::clsCTX() {
	return this->_clsCTX;
}

const DWORD OPCServer::clsCTX(DWORD value) {
	return this->_clsCTX = value;
}

vector<OPCItem*>* OPCServer::items()
{
	vector<OPCItem*>* pItems = new vector<OPCItem*>();

	HRESULT hRes;
	CLSID guid = *_guid;

	// IOPCBrowseServerAddressSpace
	IID IID_IOPCBrowseServerAddressSpace = __uuidof(IOPCBrowseServerAddressSpace);
	
	Connect();

	IOPCBrowseServerAddressSpace* pBrowse;
	hRes = _server->QueryInterface(IID_IOPCBrowseServerAddressSpace, (void**)&pBrowse);
	if (FAILED(hRes)) {
		throw hRes;
	}
	LPWSTR szFilterCriteria = (LPWSTR)L"";
	itemsChildren(pItems, pBrowse, szFilterCriteria, NULL);
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

	int nLeavesCount = 0;
	if (hRes == S_OK) {
		pEnum->Next(1, &strName, &cnt);
		ws = wstring(strName);
		name = string(ws.begin(), ws.end());
		while (cnt != 0)
		{
			pParent->GetItemID(strName, &szItemID); // получает полный идентификатор тега
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




