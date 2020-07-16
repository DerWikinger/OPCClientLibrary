#include "pch.h"
#include "OPCGroup.h"
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
