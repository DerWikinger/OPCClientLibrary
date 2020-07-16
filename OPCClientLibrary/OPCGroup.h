#pragma once

#include <vector>
#include "OPCItem.h"
#include <string>

namespace OPCClientLibrary {

	using namespace std;
	using namespace OPCDA;

	class __declspec(dllexport) OPCGroup
	{
		vector<OPCItem*> _items;
		string _name = "Nonamed OPCGroup";
		IOPCItemMgt* _pItemMgt;

	public:
		explicit OPCGroup(const string& name, vector<OPCItem*>& items) : _name(name), _items(items) {}
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

		const IOPCItemMgt* ItemMgt() const {
			return _pItemMgt;
		}

		const IOPCItemMgt* ItemMgt(IOPCItemMgt* value) {
			return _pItemMgt = value;
		}

		const void SyncRead();
	};
}

