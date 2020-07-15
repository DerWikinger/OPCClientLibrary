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

OPCItem* OPCItem::Parent() { return _parent; }
OPCItem* OPCItem::Parent(OPCItem* value) { return _parent = value; }

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