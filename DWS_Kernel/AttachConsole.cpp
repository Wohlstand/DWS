#include "DWS_Kernel.h"
#include <fcntl.h>
#include <io.h>
#include <fstream>
#include "DWS_Kernel.h"
// Максимальное количество линий в консоли
static const WORD MAX_CONSOLE_LINES = 500;

/*Честно пизданул код : http://stackoverflow.com/questions/28653381/allocconsole-with-visual-studio-2013 */

static std::wstreambuf *CinBuffer, *CoutBuffer, *CerrBuffer;
static std::wfstream ConsoleInput, ConsoleOutput, ConsoleError;

DllExport void RedirectIOToConsole()
{
	using namespace NULLPTR::DWS::Kernel;
	// Создание консоли в этом приложении
	if (AllocConsole())
	{
		CinBuffer = std::wcin.rdbuf();
		CoutBuffer = std::wcout.rdbuf();
		CerrBuffer = std::wcerr.rdbuf();
		ConsoleInput.open("CONIN$", std::ios::in);
		ConsoleOutput.open("CONOUT$", std::ios::out);
		ConsoleError.open("CONOUT$", std::ios::out);
		wcin().getStream().rdbuf(ConsoleInput.rdbuf());
		wcout().getStream().rdbuf(ConsoleOutput.rdbuf());
		wcerr().getStream().rdbuf(ConsoleError.rdbuf());
	}

	// make cout, wcout(), cin, wcin(), wcerr(), cerr, wclog and clog
	// point to console as well
	std::ios::sync_with_stdio();
}
