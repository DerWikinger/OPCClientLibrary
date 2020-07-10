#pragma once

namespace OPCClientLibrary {

	using namespace std;
	
	class __declspec(dllexport) OPCItem
	{
		string _name;
		string _itemID;
		OPCItem* _parent;
		OPCDA::tagOPCBROWSETYPE _itemType;
		bool _enabled;

	public:
		explicit OPCItem(string name) : _name(name) {}
		virtual ~OPCItem() {}
		OPCItem* parent();
		OPCItem* parent(OPCItem* value);
		string* itemID();
		string* itemID(const string& value);
		bool enabled();
		bool enabled(bool value);
		OPCDA::tagOPCBROWSETYPE itemType();
		OPCDA::tagOPCBROWSETYPE itemType(OPCDA::tagOPCBROWSETYPE value);
	};
}


