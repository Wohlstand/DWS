#pragma once


#ifndef DWSKERNEL
#define DWSKERNEL
#endif

#include <windows.h>
#include <iostream>
#include <sstream>

#include "Singleton.h"

#ifdef UNICODE
typedef std::wstring		String;
typedef std::wstringstream	StringStream;
typedef wchar_t				STR;
#else
typedef std::string			String;
typedef std::stringstream	StringStream;
typedef char				STR;
#endif

template< typename OStreamable >
String to_string(const OStreamable& obj)
{
	StringStream woss;
	woss << obj;
	if (!woss) throw "error convert";
	return woss.str();
}

DWORD to_DWORD(String str);

#ifdef _DEBUG
#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )
#else
#define DllImport
#define DllExport
#endif
#include <functional>
#include <mutex>
#include <thread>
#include <list>
#include "LockedStream.h"
#include "BaseFixer.h"


#define HOSTS_NULL_IP TEXT("0.0.0.0")

// DWS Kernel functions
DllExport void _main();

DllExport void RedirectIOToConsole();

namespace DWSKernel {
	DllExport void ShowMessageBox(HWND hWnd, String Message, String Caption);

	DllExport void initKernel();

	typedef std::function<void(String)> GrabberCallback;

	//замени обработчик и получишь весь output!
	DllExport void addOutputGrabber(GrabberCallback _callback);
	//создаем в нашем namespace аналоги из std
	DllExport LockedStream<std::wostream> &wcout();
	DllExport LockedStream<std::wistream> &wcin();
	DllExport LockedStream<std::wostream> &wcerr();
}