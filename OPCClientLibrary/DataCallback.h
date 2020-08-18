#pragma once
#include <vector>
#include "OPCItem.h"

namespace OPCClientLibrary {

	using namespace OPCDA;

	class OPCCLIENTLIBRARY_API DataCallback :
		public IOPCDataCallback
	{
	private:
		ULONG _ulRefs;
		vector<OPCItem*> _items;
	public:
		DataCallback(const vector<OPCItem*>& items) : _items(items) {}
		virtual ~DataCallback() {}

		virtual STDMETHODIMP_ (ULONG) AddRef();
		virtual STDMETHODIMP_ (ULONG) Release();
		virtual STDMETHODIMP QueryInterface(REFIID iid, LPVOID* ppInterface);
		virtual STDMETHODIMP raw_OnDataChange(
			/*[in]*/ unsigned long dwTransid,
			/*[in]*/ unsigned long hGroup,
			/*[in]*/ HRESULT hrMasterquality,
			/*[in]*/ HRESULT hrMastererror,
			/*[in]*/ unsigned long dwCount,
			/*[in]*/ unsigned long* phClientItems,
			/*[in]*/ VARIANT* pvValues,
			/*[in]*/ unsigned short* pwQualities,
			/*[in]*/ struct _FILETIME* pftTimeStamps,
			/*[in]*/ HRESULT* pErrors);
		virtual STDMETHODIMP raw_OnReadComplete(
			/*[in]*/ unsigned long dwTransid,
			/*[in]*/ unsigned long hGroup,
			/*[in]*/ HRESULT hrMasterquality,
			/*[in]*/ HRESULT hrMastererror,
			/*[in]*/ unsigned long dwCount,
			/*[in]*/ unsigned long* phClientItems,
			/*[in]*/ VARIANT* pvValues,
			/*[in]*/ unsigned short* pwQualities,
			/*[in]*/ struct _FILETIME* pftTimeStamps,
			/*[in]*/ HRESULT* pErrors);
		virtual STDMETHODIMP raw_OnWriteComplete(
			/*[in]*/ unsigned long dwTransid,
			/*[in]*/ unsigned long hGroup,
			/*[in]*/ HRESULT hrMastererr,
			/*[in]*/ unsigned long dwCount,
			/*[in]*/ unsigned long* pClienthandles,
			/*[in]*/ HRESULT* pErrors);
		virtual STDMETHODIMP raw_OnCancelComplete(
			/*[in]*/ unsigned long dwTransid,
			/*[in]*/ unsigned long hGroup);
	};
}


