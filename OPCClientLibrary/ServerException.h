#pragma once
#include "pch.h"
#include <exception>

#ifdef OPCCLIENTLIBRARY_EXPORTS
#define OPCCLIENTLIBRARY_API __declspec(dllexport)
#else
#define OPCCLIENTLIBRARY_API __declspec(dllimport)
#endif

namespace OPCClientLibrary {

	class OPCCLIENTLIBRARY_API ServerException : public std::exception
	{		
		HRESULT _hResult;
		const char* _message = 0;
	public:
		explicit ServerException(const char* message, const HRESULT hRes) : _message(message), _hResult(hRes) {}
		virtual ~ServerException() {}

		inline HRESULT hResult() const { return _hResult; }
		inline const char* message() const { return _message;  }
		inline operator HRESULT() const { return _hResult; }
		inline int operator== (HRESULT hRes) const { return _hResult == hRes; }
	};
}

