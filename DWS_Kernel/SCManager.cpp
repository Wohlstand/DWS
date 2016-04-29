#include "SCManager.h"
#include "DWS_Utility.h"
namespace DWSUtility
{
	using namespace DWSKernel;

	SCManager::~SCManager()
	{
		if (m_scHdl)
		{
			if (!CloseServiceHandle(m_scHdl))
			{
				U_WARNING << " error on close service manager!" << GetLastError() << " \r\n";
			}
		}
	}

	void SCManager::init()
	{
		if ( 
			!( m_scHdl = OpenSCManager(
							NULL,                    // local computer
							NULL,                    // ServicesActive database 
							SC_MANAGER_ALL_ACCESS)  // full access rights 
						) 
			)
		{
			U_ERROR << " Can't open SCManager. Error = " << GetLastError() << "\r\n";
		}
	}

	VServ SCManager::getService(String _name)
	{
		if (!m_scHdl)
		{
			U_ERROR  << " Pls init this! \r\n";
#pragma todo(sergius-dart:uncomment me)
//			throw std::runtime_error("Initialized failed");
		}
		if (SC_HANDLE tmpHdl = OpenService(m_scHdl, _name.c_str(), SC_MANAGER_ALL_ACCESS) )
		{
			return VServ(new SCService(tmpHdl));
		}
		U_ERROR << " Don't open service! Error = " << GetLastError() << " \r\n";
#pragma todo(sergius-dart:remove me)
		return VServ(new SCService(0));
		return VServ(0);
	}


	SCService::SCService(SC_HANDLE _current):
		m_service(_current)
	{
		if (m_service == NULL)
#pragma todo(sergius-dart:Normal translate!)
			U_WARNING << "Ti vtiraesh mne kakuy-to dich'\r\n";
	}

	SCService::~SCService()
	{
		if (m_service)
		{
			if (!CloseServiceHandle(m_service))
			{
				U_WARNING << " error on close service!"<< GetLastError() << " \r\n";
			}
		}
	}

	bool SCService::setStartType(ST_TYPE _st)
	{
		if (!m_service)
		{
			U_ERROR << " Pls init this! \r\n";
#pragma todo(sergius-dart:uncomment me)
//			throw std::runtime_error("Initialized failed");
		}
		if (!ChangeServiceConfig(
			m_service,			   // handle of service 
			SERVICE_NO_CHANGE,     // service type: no change 
			_st,					// service start type 
			SERVICE_NO_CHANGE,     // error control: no change 
			NULL,                  // binary path: no change 
			NULL,                  // load order group: no change 
			NULL,                  // tag ID: no change 
			NULL,                  // dependencies: no change 
			NULL,                  // account name: no change 
			NULL,                  // password: no change 
			NULL))                // display name: no change
		{
			U_WARNING << "Don't set start type service! Error = " << GetLastError() << "\r\n";
			return false;
		}
		return true;
	}

	SCService::ST_TYPE SCService::getStartType() const
	{
		//see pcbBytesNeeded, and reallocate and recall. Но я слишком ленивый.
		QUERY_SERVICE_CONFIG localConfig;

		DWORD needBytes = 0;

		if (!m_service)
		{
			U_ERROR << " Pls init this! \r\n";
#pragma todo(sergius-dart:uncomment me)
			//			throw std::runtime_error("Initialized failed");
		}

		if (!QueryServiceConfig(
			m_service,
			&localConfig, 
			sizeof(localConfig),
			&needBytes))
		{
			U_WARNING <<  "Error on query service = " << GetLastError() << "\r\n";
			return DISABLE;
		}

		return (SCService::ST_TYPE)localConfig.dwServiceType;
	}

	bool SCService::remove()
	{
		U_WARNING << "Not implement! \r\n";
		return false;
	}
}