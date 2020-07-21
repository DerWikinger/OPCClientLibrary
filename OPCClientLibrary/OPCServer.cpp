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
	//hRes = CoInitialize(NULL);
	MULTI_QI* pResults = new MULTI_QI();
	pResults->pIID = &IID_IOPCSERVER;
	//pResults->pIID = new IID[2] { IID_IOPCSERVER, IID_IOPCBrowseServerAddressSpace };

	hRes = CoCreateInstanceEx(*_guid, NULL, _clsCTX, _serverInfo, 1, pResults);
	if (FAILED(hRes))
	{
		throw hRes;
	}

	//SOLE_AUTHENTICATION_INFO authInfo;
	//SecureZeroMemory(&authInfo, sizeof(SOLE_AUTHENTICATION_INFO));

	//authInfo.dwAuthnSvc = RPC_C_AUTHN_WINNT;
	//authInfo.dwAuthzSvc = RPC_C_AUTHZ_NONE;
	//authInfo.pAuthInfo = _serverInfo->pAuthInfo;

	//SOLE_AUTHENTICATION_LIST authInfoList;
	//authInfoList.cAuthInfo = 1;
	//authInfoList.aAuthInfo = &authInfo;

	//hRes = CoInitializeSecurity(
	//	NULL,
	//	-1,
	//	NULL,
	//	NULL,
	//	RPC_C_AUTHN_LEVEL_CALL,
	//	RPC_C_IMP_LEVEL_IMPERSONATE,
	//	&authInfoList,
	//	EOAC_NONE,
	//	NULL);
	//
	//if (FAILED(hRes))
	//{
	//	throw hRes;
	//}

	_server = (IOPCServer*)pResults->pItf;
}

void OPCServer::Disconnect() {
	if (_server != NULL) {
		_server->Release();
		_server = NULL;
	}	
	//CoUninitialize();
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

void OPCServer::itemsChildren(vector<OPCItem*>* pItems, IOPCBrowseServerAddressSpace* pBrowse, LPWSTR szFilterCriteria, OPCItem* pParentItem) {

	tagOPCBROWSEELEMENT* tags = new tagOPCBROWSEELEMENT();
	
	IEnumString* pEnum = NULL;
	wchar_t* strName;
	ULONG cnt;
	LPWSTR szItemID;
	wstring ws;
	string itemID, name;
	OPCItem* item;
	HRESULT hRes;
	
	hRes = pBrowse->BrowseOPCItemIDs(tagOPCBROWSETYPE::OPC_LEAF, szFilterCriteria, VARENUM::VT_EMPTY, 0, &pEnum);
	if (FAILED(hRes)) throw hRes;
	int nLeavesCount = 0;
	if (hRes == S_OK) {
		pEnum->Next(1, &strName, &cnt);
		while (cnt != 0)
		{
			pBrowse->GetItemID(strName, &szItemID); // получает полный идентификатор тега
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

	hRes = pBrowse->BrowseOPCItemIDs(tagOPCBROWSETYPE::OPC_BRANCH, szFilterCriteria, VARENUM::VT_EMPTY, 0, &pEnum);
	if (FAILED(hRes)) throw hRes;
	int nBranchesCount = 0;
	if (hRes == S_OK) {
		pEnum->Next(1, &strName, &cnt);
		while (cnt != 0)
		{
			pBrowse->GetItemID(strName, &szItemID);
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
			pBrowse->ChangeBrowsePosition(tagOPCBROWSEDIRECTION::OPC_BROWSE_TO, szItemID);
			OPCServer::itemsChildren(pItems, pBrowse, szFilterCriteria, item);
			pBrowse->ChangeBrowsePosition(tagOPCBROWSEDIRECTION::OPC_BROWSE_UP, szItemID);
			pEnum->Next(1, &strName, &cnt);
			nBranchesCount++;
		}
	}
}

const ULONG OPCServer::AddGroup(OPCGroup &group, ULONG updateRate) {
	if (_server == NULL)
	{
		throw 123; // Сервер не подключен
	}
	HRESULT hRes;
	BSTR name = _com_util::ConvertStringToBSTR(group.Name().c_str());
	ULONG hClientGroup, phServerGroup;
	hClientGroup = group.ClientGroup();
	IID IID_IOPCItemMgt = __uuidof(IOPCItemMgt);
	IOPCItemMgt* pItemMgt = NULL;
	long bActive = 1;

	hRes = _server->AddGroup(name, bActive, updateRate, hClientGroup, NULL, NULL, 0,
		&phServerGroup, &updateRate, &IID_IOPCItemMgt, (IUnknown**)&pItemMgt);
	if (FAILED(hRes))
	{
		throw hRes;
	}

	for_each(group.Items().begin(), group.Items().end(), [&](OPCItem* item) {
		item->CannonicalDataType(GetItemDataType(item->ItemID()));
		});

	group.ItemMgt(pItemMgt);
	group.ServerGroup(phServerGroup);
	return phServerGroup;	
}

const void OPCServer::RemoveGroup(OPCGroup& group)
{
	if (_server == NULL)
	{
		throw 123; // Сервер не подключен
	}
	_server->RemoveGroup(group.ServerGroup(), 1);
}

const VARENUM OPCServer::GetItemDataType(const string& itemID) {
	HRESULT hRes;
	IID IID_ItemProperties = __uuidof(IOPCItemProperties);
	IOPCItemProperties* pItemProperies;
	hRes = _server->QueryInterface(IID_ItemProperties, (void**)&pItemProperies);
	if (FAILED(hRes)) {
		throw hRes;
	}
	BSTR str = _com_util::ConvertStringToBSTR(itemID.c_str());
	LPWSTR szItemID = str;
	
	DWORD dwCount;
	DWORD* pdwPropertyIDs;
	LPWSTR* ppDescription;
	USHORT* pvtDataType;
	VARENUM result = VT_EMPTY;
	try {
		hRes = pItemProperies->QueryAvailableProperties(szItemID, &dwCount, &pdwPropertyIDs, &ppDescription, &pvtDataType);
		if (FAILED(hRes)) {
			throw hRes;
		}
		DWORD dwPropertyID;
		USHORT vtDataType;

		auto find = [&]() -> VARENUM {
			for (int i = 0; i < dwCount; i++) {
				dwPropertyID = pdwPropertyIDs[i];
				vtDataType = pvtDataType[i];
				if (dwPropertyID == 2) // Value DataType
				{
					return VARENUM(vtDataType);
				}
				//description = ppDescription[i];
			}
			return VT_EMPTY;
		};
		result = find();
	}
	catch (HRESULT hresult) {

	}

	SysFreeString(str);
	CoTaskMemFree(pdwPropertyIDs);
	CoTaskMemFree(ppDescription);
	CoTaskMemFree(pvtDataType);

	return result;
}


const string OPCServer::ToString() {
	//string res = "OPCServerObject";
	return _name;
	//return this->_name;
}




