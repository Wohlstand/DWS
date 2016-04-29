#pragma once
#include <iostream>
#include "DWS_Kernel.h"
#include "DWS_Utility.h"
#include <windows.h>
namespace DWSUtility {

	template < typename > 
	class CallbackHelper;

	template < typename ... Args>
	class CallbackHelper< void (*)(Args...)>
	{
		static VOID
			WINAPI func(
				DWORD dwErrorCode,
				DWORD dwNumberOfBytesTransfered,
				LPOVERLAPPED lpOverlapped
				)
		{
			_F(dwErrorCode, dwNumberOfBytesTransfered, lpOverlapped);
		}
	};


	//Класс запускает комманду в PowerShell
	class PSRunner
	{
	protected:
		PROCESS_INFORMATION m_pi;
		STARTUPINFO m_si;
		SECURITY_ATTRIBUTES m_sa;
		HANDLE g_hChildStd_IN_Rd = NULL;
		HANDLE g_hChildStd_IN_Wr = NULL;
		HANDLE g_hChildStd_OUT_Rd = NULL;
		HANDLE g_hChildStd_OUT_Wr = NULL;

		DWORD m_flags = CREATE_NO_WINDOW;

		String m_command;

	public:
		PSRunner(String _commandLine);
		virtual ~PSRunner();

		BOOL exec();

		bool waitEnd();

		String readAllOutputStream();

	private:
		//заполняем внутренние структуры
		void fillSI(HANDLE _input, HANDLE _output);
		void init();
	};


}