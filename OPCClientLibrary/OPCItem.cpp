#include "pch.h"
#include "OPCItem.h"

using namespace OPCClientLibrary;

OPCDA::tagOPCBROWSETYPE OPCItem::itemType() 
{ 
	return _itemType; 
}

OPCDA::tagOPCBROWSETYPE OPCItem::itemType(OPCDA::tagOPCBROWSETYPE value)
{ 
	return _itemType = value;
}

OPCItem* OPCItem::parent() { return _parent; }
OPCItem* OPCItem::parent(OPCItem* value) { return _parent = value; }

string* OPCItem::itemID()
{
	return &_itemID;
}
string* OPCItem::itemID(const string& value)
{
	return &(_itemID = value);
}

bool OPCItem::enabled() 
{
	return _enabled;
}
bool OPCItem::enabled(bool value)
{
	return _enabled = value;
}