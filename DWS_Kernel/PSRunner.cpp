#include "PSRunner.h"

//see https://gist.github.com/konstantint/d49ab683b978b3d74172

namespace NULLPTR {
	namespace DWS {
		namespace Utility {

			PSRunner::PSRunner(String _command) :
				m_command(_command)
			{
			}

			PSRunner::~PSRunner()
			{
				CloseHandle(m_pi.hThread);
				CloseHandle(m_pi.hProcess);
				CloseHandle(g_hChildStd_IN_Rd);
				CloseHandle(g_hChildStd_IN_Wr);
				CloseHandle(g_hChildStd_OUT_Rd);
				CloseHandle(g_hChildStd_OUT_Wr);
			}
			BOOL PSRunner::exec()
			{
				init();
				return CreateProcess(
					NULL,
					const_cast<typename String::traits_type::char_type *>(m_command.c_str()), //НЕ ТРОГАЙ!!! Я медленно снимаю const с результата...
					NULL,
					NULL,
					TRUE,
					m_flags,
					NULL,
					NULL,
					&m_si,
					&m_pi);
				;
			}
			bool PSRunner::waitEnd()
			{
				return false;
			}

			String PSRunner::readAllOutputStream()
			{
				//read default 10*1024 bytes
				DWORD count = 10 * 1024;
				DWORD readed = 0;
				String result;
				result.reserve(count / sizeof(String::traits_type::char_type));
				//read buffer
				BOOL success = ReadFile(
					g_hChildStd_OUT_Rd,
					reinterpret_cast<void*>(
						const_cast<typename String::traits_type::char_type*>(result.c_str())
						),
					count,
					&readed,
					NULL);
				if (!success)
				{
					UC_ERROR << "Read file Error\r\n";
				}
				return result;
			}
			void PSRunner::fillSI(HANDLE _input, HANDLE _output)
			{

				//see http://stackoverflow.com/questions/7018228/how-do-i-redirect-output-to-a-file-with-createprocess
				m_si.cb = sizeof(STARTUPINFO);
				m_si.dwFlags |= STARTF_USESTDHANDLES;
				m_si.hStdInput = _input;
				m_si.hStdError = _output;
				m_si.hStdOutput = _output;
			}
			void PSRunner::init()
			{
				ZeroMemory(&m_pi, sizeof(m_pi));
				ZeroMemory(&m_si, sizeof(m_si));
				//see http://stackoverflow.com/questions/7018228/how-do-i-redirect-output-to-a-file-with-createprocess
				m_sa.nLength = sizeof(STARTUPINFO);
				m_sa.lpSecurityDescriptor = NULL;
				m_sa.bInheritHandle = TRUE;

				//see https://msdn.microsoft.com/ru-ru/library/windows/desktop/ms682499(v=vs.85).aspx

				SECURITY_ATTRIBUTES saAttr;

				// Set the bInheritHandle flag so pipe handles are inherited. 

				saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
				saAttr.bInheritHandle = TRUE;
				saAttr.lpSecurityDescriptor = NULL;

				// Create a pipe for the child process's STDOUT. 

				if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
					U_ERROR << TEXT("StdoutRd CreatePipe");

				// Ensure the read handle to the pipe for STDOUT is not inherited.

				if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
					U_ERROR << TEXT("Stdout SetHandleInformation");

				// Create a pipe for the child process's STDIN. 

				if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
					U_ERROR << TEXT("Stdin CreatePipe");

				// Ensure the write handle to the pipe for STDIN is not inherited. 

				if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
					U_ERROR << TEXT("Stdin SetHandleInformation");

				//init streams!

				fillSI(g_hChildStd_IN_Rd, g_hChildStd_OUT_Wr);
			}
		}
	}
}