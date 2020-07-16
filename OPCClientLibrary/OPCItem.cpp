#include "pch.h"
#include "OPCItem.h"

using namespace OPCClientLibrary;

OPCDA::tagOPCBROWSETYPE OPCItem::ItemType() 
{ 
	return _itemType; 
}

OPCDA::tagOPCBROWSETYPE OPCItem::ItemType(OPCDA::tagOPCBROWSETYPE value)
{ 
	return _itemType = value;
}

ULONG OPCItem::ClientItem() { return _hClientItem; }
ULONG OPCItem::ClientItem(ULONG value) { return _hClientItem = value; }

OPCItem* OPCItem::Parent() { return _parent; }
OPCItem* OPCItem::Parent(OPCItem* value) { return _parent = value; }

USHORT OPCItem::Quality() { return _quality; }
USHORT OPCItem::Quality(USHORT value) { return _quality = value; }

VARIANT OPCItem::Value() { return _value; }
VARIANT OPCItem::Value(VARIANT value) { return _value = value; }

_FILETIME OPCItem::TimeStamp() { return _ftTimeStamp; }
_FILETIME OPCItem::TimeStamp(_FILETIME value) { return _ftTimeStamp = value; }

string OPCItem::Name()
{
	return _name;
}
string OPCItem::Name(const string &value)
{
	return _name = value;
}

string OPCItem::ItemID()
{
	return _itemID;
}
string OPCItem::ItemID(const string &value)
{
	return _itemID = value;
}

bool OPCItem::Enabled() 
{
	return _enabled;
}
bool OPCItem::Enabled(bool value)
{
	return _enabled = value;
}