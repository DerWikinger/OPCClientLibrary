#include "pch.h"
#include "OPCGroup.h"
#include "DataCallback.h"
#include <algorithm>

using namespace OPCClientLibrary;
const void OPCGroup::SyncRead() {
	HRESULT hRes;
	tagOPCDATASOURCE source = tagOPCDATASOURCE::OPC_DS_CACHE;
	DWORD dwCount = _items.size();
	IOPCSyncIO* pSyncIO = NULL;
	IID IID_IOPCSYNCIO = __uuidof(IOPCSyncIO);
	hRes = _pItemMgt->QueryInterface(IID_IOPCSYNCIO, (void**)&pSyncIO);
	
	ULONG* pServer = new ULONG[dwCount];
	for (int i = 0; i < dwCount; i++) {
		pServer[i] = _items.at(i)->ServerHandle();
	}
	tagOPCITEMRESULT* pResults = NULL;
	HRESULT* pErrors = NULL;
	tagOPCITEMSTATE* pItemState = NULL;

	hRes = pSyncIO->Read(OPC_DS_CACHE, dwCount, pServer, &pItemState, &pErrors);
	if (FAILED(hRes)) {
		throw hRes;
	}

	auto find = [&](ULONG hClient) -> OPCItem* {
		for (int j = 0; j < dwCount; j++) {
			if (_items.at(j)->ClientItem() == hClient) return _items.at(j);
		}
		return NULL;
	};

	for(int i = 0; i < dwCount; i++) {
		OPCItem* pItem = find(pItemState[i].hClient);
		pItem->Quality(pItemState[i].wQuality);
		pItem->Value(pItemState[i].vDataValue);
		pItem->TimeStamp(pItemState[i].ftTimeStamp);
	}
}

const void OPCGroup::AddItems() {

	if (_pItemMgt == NULL) {
		throw 123;
	}
	
	DWORD dwCount = _items.size();

	tagOPCITEMDEF* pItems = (tagOPCITEMDEF*)CoTaskMemAlloc(dwCount * sizeof(tagOPCITEMDEF));
	for (int i = 0; i < dwCount; i++) {
		OPCItem* item = _items.at(i);
		BSTR itemID = _com_util::ConvertStringToBSTR(item->ItemID().c_str());
		pItems[i].szItemID = itemID;
		SysFreeString(itemID);
		pItems[i].szAccessPath = NULL;
		pItems[i].bActive = item->Enabled();
		pItems[i].hClient = item->ClientItem();
		//pItems[i].hClient = 0;
		pItems[i].vtRequestedDataType = VT_EMPTY;
		pItems[i].dwBlobSize = 0;
		pItems[i].pBlob = NULL;
	}

	HRESULT hRes;
	tagOPCITEMRESULT* pResults = NULL;
	HRESULT* pErrors = NULL;

	hRes = _pItemMgt->AddItems(dwCount, pItems, &pResults, &pErrors);
	if (FAILED(hRes))
	{
		throw hRes;
	}
	if (hRes == S_OK) {
		for (int i = 0; i < dwCount; i++) {
			OPCItem* item = _items.at(i);
			item->AccessRights(pResults[i].dwAccessRights);
			item->CannonicalDataType(pResults[i].vtCanonicalDataType);
			item->ServerHandle(pResults[i].hServer);
		}
	}
	CoTaskMemFree(pItems);
	CoTaskMemFree(pResults);
	CoTaskMemFree(pErrors);
}

const void OPCGroup::RemoveItems() {
	HRESULT* pErrors;
	DWORD dwCount = _items.size();
	ULONG* phServer = (ULONG*)CoTaskMemAlloc(dwCount * sizeof(ULONG));
	for (int i = 0; i < dwCount; i++) {
		phServer[i] = _items.at(i)->ServerHandle();
	}
	_pItemMgt->RemoveItems(dwCount, phServer, &pErrors);
	CoTaskMemFree(phServer);
	CoTaskMemFree(pErrors);
}

const void OPCGroup::Advise() {
	if (_pItemMgt == NULL) {
		throw 123;
	}
	HRESULT hRes;
	IConnectionPointContainer* pCPC;
	IID IID_CPC = __uuidof(IConnectionPointContainer);
	hRes = _pItemMgt->QueryInterface(IID_CPC, (void**)&pCPC);
	DWORD dwCount = _items.size();
	//HRESULT* pErrors;
	//ULONG* phServer = new ULONG[dwCount]();
	//for (int i = 0; i < dwCount; i++) {
	//	phServer[i] = _items.at(i)->ServerHandle();
	//}
	//_pItemMgt->RemoveItems(dwCount, phServer, &pErrors);
	hRes = pCPC->FindConnectionPoint(__uuidof(IOPCDataCallback), &_pDataCallback);
	if (FAILED(hRes)) {
		throw hRes;
	}
	pCPC->Release();
	//DataCallback pDC(_items);
	DataCallback* pDC =  new DataCallback(_items);
	hRes = _pDataCallback->Advise(pDC, &_dwCookie);
	if (FAILED(hRes)) {
		throw hRes;
	}
	//tagOPCITEMDEF* pItems = (tagOPCITEMDEF*)CoTaskMemAlloc(dwCount * sizeof(tagOPCITEMDEF));
	//for (int i = 0; i < dwCount; i++) {
	//	OPCItem* item = _items.at(i);
	//	BSTR itemID = _com_util::ConvertStringToBSTR(item->ItemID().c_str());
	//	pItems[i].szItemID = itemID;
	//	pItems[i].szAccessPath = NULL;
	//	pItems[i].bActive = item->Enabled();
	//	pItems[i].hClient = item->ClientItem();
	//	//pItems[i].hClient = 0;
	//	pItems[i].vtRequestedDataType = VT_EMPTY;
	//	pItems[i].dwBlobSize = 0;
	//	pItems[i].pBlob = NULL;
	//}

	//tagOPCITEMRESULT* pResults = NULL;

	//hRes = _pItemMgt->AddItems(dwCount, pItems, &pResults, &pErrors);
	//if (FAILED(hRes))
	//{
	//	throw hRes;
	//}
}

const void OPCGroup::Unadvise() {
	HRESULT hRes;
	hRes = ((IConnectionPoint*)_pDataCallback)->Unadvise(_dwCookie);
	if (FAILED(hRes)) {
		throw hRes;
	}
	_pDataCallback->Release();
	_dwCookie = 0;
}

