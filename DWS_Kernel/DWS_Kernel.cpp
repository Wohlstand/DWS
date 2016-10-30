#include "DWS_Kernel.h"
#include "DWS_Utility.h"

namespace NULLPTR {
	namespace DWS {
		namespace Kernel {
			//create streams
			static LockedStream<std::wostream> &p_wcout = LockedStream<std::wostream>(std::wcout);
			static LockedStream<std::wistream> &p_wcin = LockedStream<std::wistream>(std::wcin);
			static LockedStream<std::wostream> &p_wcerr = LockedStream<std::wostream>(std::wcerr);
			//create collection
			std::list < BaseFixer* > BaseFixer::m_all;

			DllExport LockedStream<std::wostream> &wcout() {
				return p_wcout;
			}
			DllExport LockedStream<std::wistream> &wcin()
			{
				return p_wcin;
			}
			DllExport LockedStream<std::wostream> &wcerr()
			{
				return p_wcerr;
			}

			DllExport void initKernel()
			{
				// Всё равно никто не будет читать этот код, по этому тут будет стих

				/*

				Котёнок возится с клубком:
				То подползет к нему тайком,
				То на клубок начнет кидаться,
				Толкнет его, отпрыгнет вбок...
				Hикак не может догадаться,
				Что здесь не мышка, а клубок.

				*/
				//создаем все что нужно для utility
				Utility::initUtility();
			}
			DllExport void addOutputGrabber(GrabberCallback _callback)
			{
				wcout().outputGrabber += _callback;
				wcerr().outputGrabber += _callback;
			}
		}
	}
}

DWORD to_DWORD(String str)
{
	DWORD result;

	StringStream ss(str);
	ss >> std::hex >> result;
	return result;
}