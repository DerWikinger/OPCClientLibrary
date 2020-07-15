#pragma once

namespace OPCClientLibrary {

	using namespace std;

	static ULONG hClientItems;
	
	class __declspec(dllexport) OPCItem
	{
		string _name;
		string _itemID;
		OPCItem* _parent;
		ULONG _hClientItem;
		OPCDA::tagOPCBROWSETYPE _itemType;
		bool _enabled;

	public:
		explicit OPCItem(const string& name) : _name(name), _hClientItem(++hClientItems) { }
		virtual ~OPCItem() {}
		OPCItem* Parent();
		OPCItem* Parent(OPCItem* value);
		string Name();
		string Name(const string &value);
		string ItemID();
		string ItemID(const string &value);
		ULONG ClientItem() { return _hClientItem; };
		bool Enabled();
		bool Enabled(bool value);
		
		OPCDA::tagOPCBROWSETYPE ItemType();
		OPCDA::tagOPCBROWSETYPE ItemType(OPCDA::tagOPCBROWSETYPE value);
	};
}


