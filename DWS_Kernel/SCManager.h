#pragma once
#include <Windows.h>
#include <memory>
#include "DWS_Kernel.h"
namespace DWSUtility
{
	class SCService;
	typedef std::shared_ptr<SCService> VServ;

	class SCManager
	{
		SC_HANDLE m_scHdl;
	public:
		~SCManager();
		//skip constructors

		void init();

		VServ getService(String _name);
	};

	class SCService
	{
		friend class SCManager;
		SC_HANDLE m_service;
		SCService(SC_HANDLE _current);//Руки отсюда убери! Создавай этот клас через менеджер выше!
	public:
		enum ST_TYPE
		{
			BOOT	= SERVICE_BOOT_START,
			SYSTEM	= SERVICE_SYSTEM_START,
			AUTO	= SERVICE_AUTO_START,
			DEMAND	= SERVICE_DEMAND_START,
			DISABLE = SERVICE_DISABLED
		};

		~SCService();//а вот уничтожать может кто угодно :D Только руками не вызывай...
		bool setStartType(ST_TYPE _t);
		ST_TYPE getStartType() const;
		bool remove();
#pragma message("sergius-dart:write more code!")
	};
}