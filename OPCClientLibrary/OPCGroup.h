#pragma once

#include <vector>
#include "OPCItem.h"
#include <string>

namespace OPCClientLibrary {

	using namespace std;
	using namespace OPCDA;

	static DWORD static_dwTransactionID = 0;
	static DWORD static_hClientGroup = 0;

	class OPCCLIENTLIBRARY_API OPCGroup
	{
		vector<OPCItem*> _items;
		string _name = "Nonamed OPCGroup";
		IOPCItemMgt* _pItemMgt = NULL;
		IConnectionPoint* _pDataCallback;
		DWORD _dwCookie;
		DWORD _dwTransactionID;
		DWORD _hClientGroup;
		ULONG _phServer;

	public:
		explicit OPCGroup(const string& name, vector<OPCItem*>& items) : 
			_name(name), _items(items), _hClientGroup(++static_hClientGroup){}
		virtual ~OPCGroup() {
			if (_pItemMgt) {
				_pItemMgt->Release();
			}
		}

		const vector<OPCItem*>& Items() const {
			return _items;
		}

		const string Name() const {
			return _name;
		}

		const DWORD TransactionID() const {
			return _dwTransactionID;
		}

		const DWORD ClientGroup() const {
			return _hClientGroup;
		}

		IOPCItemMgt* ItemMgt() const {
			return _pItemMgt;
		}

		IOPCItemMgt* ItemMgt(IOPCItemMgt* value) {
			return _pItemMgt = value;
		}

		const ULONG ServerGroup() {
			return _phServer;
		}

		const ULONG ServerGroup(ULONG value) {
			return _phServer = value;
		}

		const void Advise();
		const void Unadvise();
		const void AddItems();
		const void RemoveItems();

		const void SyncRead(bool ds_device = false);
		const void AsyncRead();
	};
}

