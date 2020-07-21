#pragma once
#include <shared_mutex>

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
		ULONG _hServerItem;
		VARTYPE _vtCanonicalDataType;
		DWORD _dwAccessRights;
		USHORT _quality;
		VARIANT _value;
		_FILETIME _ftTimeStamp;
		shared_mutex _mtx;

	public:
		explicit OPCItem(const string& name = "Nonamed item") : _name(name), _hClientItem(++hClientItems) { }
		virtual ~OPCItem() {}
		OPCItem* Parent();
		OPCItem* Parent(OPCItem* value);
		string Name();
		string Name(const string &value);
		string ItemID();
		string ItemID(const string &value);
		ULONG ClientItem();
		ULONG ClientItem(ULONG value);
		ULONG ServerHandle() { return _hServerItem; };
		ULONG ServerHandle(ULONG value) { return _hServerItem = value; };
		VARTYPE CannonicalDataType() { return _vtCanonicalDataType; };
		VARTYPE CannonicalDataType(VARTYPE value) { return _vtCanonicalDataType = value; };
		DWORD AccessRights() { return _dwAccessRights; };
		DWORD AccessRights(DWORD value) { return _dwAccessRights = value; };
		bool Enabled();
		bool Enabled(bool value);
		USHORT Quality();
		USHORT Quality(USHORT value);
		VARIANT Value();
		VARIANT Value(VARIANT value);
		_FILETIME TimeStamp();
		_FILETIME TimeStamp(_FILETIME value);
		
		OPCDA::tagOPCBROWSETYPE ItemType();
		OPCDA::tagOPCBROWSETYPE ItemType(OPCDA::tagOPCBROWSETYPE value);
	};
}


