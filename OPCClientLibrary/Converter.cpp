#include "pch.h"
#include "Converter.h"


using namespace OPCClientLibrary;

string Converter::ConvertToString(VARIANT value) {
	regex reg("0+$");
	switch (value.vt) {
	case VT_R4: {
		string strVal = std::to_string(value.fltVal);
		return ~strVal.find_first_of(".", 0) ? regex_replace(strVal, reg, "") : strVal;
	}
	case VT_R8: {
		string strVal = std::to_string(value.dblVal);
		return ~strVal.find_first_of(".", 0) ? regex_replace(strVal, reg, "") : strVal;
	}
	case VT_I2:
	case VT_I4:
		return std::to_string(value.intVal);
	case VT_BSTR:
		return _com_util::ConvertBSTRToString(value.bstrVal);
	default:
		return "undefined";
	}
}

LPWSTR Converter::GetQuality(USHORT qty) {
	switch (qty)
	{
	case 0x00: return const_cast<LPWSTR>(L"Bad");
	case 0x04: return const_cast<LPWSTR>(L"Config Error");
	case 0x08: return const_cast<LPWSTR>(L"Not Connected");
	case 0x0C: return const_cast<LPWSTR>(L"Device Failure");
	case 0x10: return const_cast<LPWSTR>(L"Sensor Failure");
	case 0x14: return const_cast<LPWSTR>(L"Last Known");
	case 0x18: return const_cast<LPWSTR>(L"Comm Failure");
	case 0x1C: return const_cast<LPWSTR>(L"Out of Service");
	case 0x20: return const_cast<LPWSTR>(L"Initializing");
	case 0x40: return const_cast<LPWSTR>(L"Uncertain");
	case 0x44: return const_cast<LPWSTR>(L"Last Usable");
	case 0x50: return const_cast<LPWSTR>(L"Sensor Calibration");
	case 0x54: return const_cast<LPWSTR>(L"EGU Exceeded");
	case 0x58: return const_cast<LPWSTR>(L"Sub Normal");
	case 0xC0: return const_cast<LPWSTR>(L"Good");
	case 0xD8: return const_cast<LPWSTR>(L"Local Override");
	default: return const_cast<LPWSTR>(L"Unknown");
	}
}

string Converter::FileTimeToString(_FILETIME ft) {
	//DWORD hDT = ft.dwHighDateTime;
	//DWORD lDT = ft.dwLowDateTime;
	//string result = std::to_string(hDT) + " : " + std::to_string(lDT);
	SYSTEMTIME sysTime;
	FileTimeToSystemTime(&ft, &sysTime);
	string strDT = "";
	strDT = strDT.
		append(sysTime.wDay < 10 ? string("0").append(to_string(sysTime.wDay)) : to_string(sysTime.wDay)).
		append("/").
		append(sysTime.wMonth < 10 ? string("0").append(to_string(sysTime.wMonth)) : to_string(sysTime.wMonth)).
		append("/").
		append(to_string(sysTime.wYear));
	strDT = strDT.append(" ").
		append(sysTime.wHour < 10 ? string("0").append(to_string(sysTime.wHour)) : to_string(sysTime.wHour)).
		append(":").
		append(sysTime.wMinute < 10 ? string("0").append(to_string(sysTime.wMinute)) : to_string(sysTime.wMinute)).
		append(":").
		append(sysTime.wSecond < 10 ? string("0").append(to_string(sysTime.wSecond)) : to_string(sysTime.wSecond));

	return strDT;
}