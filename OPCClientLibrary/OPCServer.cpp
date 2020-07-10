#include "pch.h"
#include "OPCServer.h"
#include <algorithm>

using namespace OPCClientLibrary;
//using namespace OPCDA;

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

vector<OPCItem*>* OPCServer::items()
{
    vector<OPCItem*>* pResult = new vector<OPCItem*>();
    IUnknown* punkOuter = NULL;

    //MULTI_QI* pResults = new MULTI_QI();
    //pResults->pIID = &IID_IOPCServerList;

    //DWORD clsCTX = CLSCTX_LOCAL_SERVER;
    //if (host != "localhost" && host != "127.0.0.1") {
    //    clsCTX = CLSCTX_INPROC;
    //}

    //HRESULT hRes;
    //hRes = CoCreateInstanceEx(clsid, NULL, clsCTX, pHostInfo, 1, pResults);
    //IOPCBrowseServerAddressSpace pBrowse = IOPCBrowseServerAddressSpace(CoCreateInstanceEx());
    return pResult;
}

void OPCServer::itemsChildren(vector<OPCItem*>* pItems, IOPCBrowseServerAddressSpace* pParent, LPWSTR szFilterCriteria, OPCItem* pParentItem) {

    IEnumString** ppEnum = NULL;
    LPOLESTR* rgelt = NULL;
    ULONG* cnt = NULL;
    LPWSTR* szItemID = NULL;
    wstring ws;
    string itemID;
    OPCItem* item;

    try
    {
        pParent->BrowseOPCItemIDs(tagOPCBROWSETYPE::OPC_LEAF, szFilterCriteria, VARENUM::VT_EMPTY, 0, ppEnum);
        (*ppEnum)->Next(1, rgelt, cnt);
        ws = wstring(*rgelt);
        string name = string(ws.begin(), ws.end());
        int nLeavesCount = 0;
        while (cnt != 0)
        {
            pParent->GetItemID(*rgelt, szItemID); // получает полный идентификатор тега
            ws = wstring(*szItemID);
            itemID = string(ws.begin(), ws.end());
            item = new OPCItem(name);
            item->parent(pParentItem);
            item->itemType(tagOPCBROWSETYPE::OPC_LEAF);
            item->itemID(itemID);
            item->enabled(true);
            pItems->push_back(item);

            (*ppEnum)->Next(1, rgelt, cnt);
            nLeavesCount++;
        }
        pParent->BrowseOPCItemIDs(tagOPCBROWSETYPE::OPC_BRANCH, szFilterCriteria, VARENUM::VT_EMPTY, 0, ppEnum);
        (*ppEnum)->Next(1, rgelt, cnt);
        int nBranchesCount = 0;
        while (cnt != 0)
        {
            pParent->GetItemID(*rgelt, szItemID);
            ws = wstring(*szItemID);
            itemID = string(ws.begin(), ws.end());
            item = new OPCItem(name);
            item->parent(pParentItem);
            item->itemType(tagOPCBROWSETYPE::OPC_LEAF);
            item->itemID(itemID);
            item->enabled(true);
            pItems->push_back(item);
            pParent->ChangeBrowsePosition(tagOPCBROWSEDIRECTION::OPC_BROWSE_TO, *szItemID);
            OPCServer::itemsChildren(pItems, pParent, szFilterCriteria, item);
            pParent->ChangeBrowsePosition(tagOPCBROWSEDIRECTION::OPC_BROWSE_UP, *szItemID);
            (*ppEnum)->Next(1, rgelt, cnt);
            nBranchesCount++;
        }
    }
    catch (exception ex)
    {
        //throw new ServerException();
    }
}

const string OPCServer::ToString() {
	//string res = "OPCServerObject";
	return _name;
	//return this->_name;
}




