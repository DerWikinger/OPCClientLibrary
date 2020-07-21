#include "pch.h"
#include "DataCallback.h"

using namespace OPCClientLibrary;

STDMETHODIMP DataCallback::QueryInterface(REFIID iid, LPVOID* ppInterface)
{
	//return S_OK;
	if (NULL == ppInterface)
	{
		return E_INVALIDARG;
	}
	if (IID_IUnknown == iid)
	{
		*ppInterface = (IUnknown*)this;
		//*ppInterface = dynamic_cast<IUnknown*>(this);
		AddRef();
		return S_OK;
	}
	if (__uuidof(IOPCDataCallback) == iid)
	{
		*ppInterface = (IOPCDataCallback*)this;
		//*ppInterface = dynamic_cast<IOPCDataCallback*>(this);
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}
//--------------------
STDMETHODIMP_(ULONG) DataCallback::AddRef()
{
	return InterlockedIncrement((LONG*)&_ulRefs);
}
//------------
STDMETHODIMP_(ULONG) DataCallback::Release()
{
	ULONG ulRefs = InterlockedDecrement((LONG*)&_ulRefs);
	if (ulRefs == 0)
	{
		delete this;
		return 0;
	}
	return ulRefs;
}

STDMETHODIMP DataCallback::raw_OnDataChange(
	/*[in]*/ unsigned long dwTransid,
	/*[in]*/ unsigned long hGroup,
	/*[in]*/ HRESULT hrMasterquality,
	/*[in]*/ HRESULT hrMastererror,
	/*[in]*/ unsigned long dwCount,
	/*[in]*/ unsigned long* phClientItems,
	/*[in]*/ VARIANT* pvValues,
	/*[in]*/ unsigned short* pwQualities,
	/*[in]*/ _FILETIME* pftTimeStamps,
	/*[in]*/ HRESULT* pErrors)
{
	return S_OK;
}

STDMETHODIMP DataCallback::raw_OnReadComplete(
	/*[in]*/ unsigned long dwTransid,
	/*[in]*/ unsigned long hGroup,
	/*[in]*/ HRESULT hrMasterquality,
	/*[in]*/ HRESULT hrMastererror,
	/*[in]*/ unsigned long dwCount,
	/*[in]*/ unsigned long* phClientItems,
	/*[in]*/ VARIANT* pvValues,
	/*[in]*/ unsigned short* pwQualities,
	/*[in]*/ _FILETIME* pftTimeStamps,
	/*[in]*/ HRESULT* pErrors) 
{
	auto find = [&](ULONG hClient) -> OPCItem* {
		for (int j = 0; j < dwCount; j++) {
			if (_items.at(j)->ClientItem() == hClient) return _items.at(j);
		}
		return NULL;
	};

	ULONG hClient;
	OPCItem* pItem;
	VARIANT value;
	for (int i = 0; i < dwCount; i++) {
		hClient = phClientItems[i];
		pItem = find(hClient);
		pItem->Quality(pwQualities[i]);
		pItem->TimeStamp(pftTimeStamps[i]);
		value = pvValues[i];
		pItem->Value(value);
		if (pvValues[i].vt == VT_BSTR) SysFreeString(pvValues[i].bstrVal);
	}

	return S_OK;
}
STDMETHODIMP DataCallback::raw_OnWriteComplete(
	/*[in]*/ unsigned long dwTransid,
	/*[in]*/ unsigned long hGroup,
	/*[in]*/ HRESULT hrMastererr,
	/*[in]*/ unsigned long dwCount,
	/*[in]*/ unsigned long* pClienthandles,
	/*[in]*/ HRESULT* pErrors) {
	return S_OK;
}
STDMETHODIMP DataCallback::raw_OnCancelComplete(
	/*[in]*/ unsigned long dwTransid,
	/*[in]*/ unsigned long hGroup) {
	return S_OK;
}

