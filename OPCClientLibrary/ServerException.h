#pragma once
#include "pch.h"
#include <exception>

namespace OPCClientLibrary {

	class __declspec(dllexport) ServerException : public std::exception
	{		
		HRESULT _hResult;
		const char* _message = 0;
	public:
		explicit ServerException(const char* message, const HRESULT hRes) : _message(message), _hResult(hRes) {}
		virtual ~ServerException() {}

		inline HRESULT hResult() const { return _hResult; }
		inline const char* message() const { return _message;  }
		inline operator HRESULT() { return _hResult; }
		inline int operator== (HRESULT hRes) { return _hResult == hRes; }
	};
}

