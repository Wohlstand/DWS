#include "DWS_Utility.h"

namespace NULLPTR {
	namespace DWS {
		namespace Utility {
			Utility instance;

			Utility& initUtility()
			{
				instance.init();
				return instance;
			}

			Utility& getUtility()
			{
				return instance;
			}

			Utility::Utility() :
				inited(false)
			{}

			void Utility::init()
			{
				p.scManager.init();
				//p.regManager->init();
				inited = true;
			}

			using namespace Kernel;
			bool RunCmd(String _cmd)
			{
				_cmd.c_str();
				return true;
			}

			void disable_service(String _servName)
			{

			}

			SCManager & Utility::getServiceManager()
			{
				if (!inited)
					throw std::runtime_error("Don't inited Utility");
				return p.scManager;
			}

			RegistryManager & Utility::getRegManager()
			{
				if (!inited)
					throw std::runtime_error("Don't inited Utility");
				return p.regManager;
			}


		}
	}
}

#include "PSRunner.h"

DllExport void _main()
{
	using namespace NULLPTR::DWS::Kernel;
	using namespace NULLPTR::DWS::Utility;
#pragma message("WHAT? \r\n? It should be added automatically.")
#pragma message("Ne trogai. Mne tak ydobno!!! Ya tyt testiruy!!!")

	RegistryManager& _rm = getUtility().getRegManager();
	//auto _q = _rm.getKey(Registry::RegInformation::HKCU, TEXT("SOFTWARE\\7-zip\\Path"));

	//_q.setValue(TEXT("C:\\Program Files\\7-Zip\\"));

	auto _w = _rm.getDir(Registry::RegInformation::HKCU, TEXT("SOFTWARE\\Nullptr"));


	auto _qq = _w.createKey(TEXT("MyItem"), Registry::RegInformation::DWORD);
	_qq.remove();
	//_qq.setValue(TEXT("9"));
	//String _qwe = _qq.getValue();
	//auto _ww = _w.createDir(TEXT("MyDir"));

	//_qq.setType(Registry::RegInformation::LINK);
	//_qq.setValue(TEXT("9"));

	//PSRunner run(TEXT("C:\\Windows\\System32\\cmd.exe"));
	//wcout() << TEXT("EXEC PSRunner:::") << run.exec() << TEXT(" error = ") << GetLastError() << TEXT("\r\n")  ;
	//String result;
	//run >> result;
	//wcout() << TEXT("PSRunner res:::") << result << TEXT("\r\n");
	//String _tmp;
	//run >> _tmp;
	//wcout() << "OUTPUT::" << _tmp;

	wcout() << "Init all fixers::\r\n";
	for (BaseFixer* _ptr : BaseFixer::getAllFixers())
	{
		_ptr->init();
		wcout() << TEXT("::") << _ptr->getName() << TEXT(" == ") << _ptr->isApply() << TEXT("::") << "\r\n";
	}
	wcout() << "End\r\n";
	wcout() << "Apply all!\r\n";
}