#pragma once

#include <vector>
#include "OPCItem.h"

namespace OPCClientLibrary {

	using namespace std;

	class __declspec(dllexport) OPCGroup
	{
		vector<OPCItem*> _items;
	public:
		OPCGroup() {}
		virtual ~OPCGroup() {}
	};
}

