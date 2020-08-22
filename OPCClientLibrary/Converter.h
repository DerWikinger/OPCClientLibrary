#pragma once
#include <string>
#include <OAIdl.h>
#include <regex>


#ifdef OPCCLIENTLIBRARY_EXPORTS
#define OPCCLIENTLIBRARY_API __declspec(dllexport)
#else
#define OPCCLIENTLIBRARY_API __declspec(dllimport)
#endif

namespace OPCClientLibrary {

	using namespace std;
	
	class OPCCLIENTLIBRARY_API Converter
	{
	public:
		static string ConvertToString(VARIANT value);

		static LPWSTR GetQuality(USHORT qty);

		static string FileTimeToString(_FILETIME ft);
	};

}
