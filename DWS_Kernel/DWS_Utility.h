#pragma once
#include "DWS_Kernel.h"
#include "Signal.h" 
#include "SCManager.h"
#include "RegManager.h"
#include "HostsEditor.h"
//для функций
#define U_ERROR wcerr() << "error:::" << __FUNCSIG__ << ":::" 
#define U_WARNING wcerr() << "warning:::" << __FUNCSIG__ << ":::"
//для классов
#define UC_ERROR wcerr() << "error:::" << to_string(typeid(*this).name()) << ":::" 
#define UC_WARNING wcerr() << "warning:::" << to_string(typeid(*this).name()) << ":::"

#define UC_ASSERT(x)	if (!(x)) {UC_ERROR<<#x ; throw #x;}


//namespace для всяких полезных функций
namespace DWSUtility
{
	using namespace DWSKernel;

	bool RunCmd(String _cmd);

	void disable_service(String _servName);

	struct Utility {
		Utility();
		void init();
		SCManager& getServiceManager();
		RegistryManager& getRegManager();

	private:
		struct {
			SCManager scManager;
			RegistryManager regManager;
		} p;
		bool inited;
	};

	Utility& initUtility();
	Utility& getUtility();
}