#include "DWS_Kernel.h"
#include "DWS_Utility.h"
namespace {
	CreateFixer(RemoveTelemetry);

	struct RegistryFixerItem
	{
		Registry::RegInformation::BRANCH _br;
		String path;
		DWORD value;
	};

	static std::list<RegistryFixerItem> regEditList{
		{
			Registry::RegInformation::HKLM,
			TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Device Metadata\\PreventDeviceMetadataFromNetwork"),
			1
		},
		{
			Registry::RegInformation::HKLM,
			TEXT("SOFTWARE\\Policies\\Microsoft\\MRT\\DontOfferThroughWUAU"),
			1
		},
		{
			Registry::RegInformation::HKLM,
			TEXT("SOFTWARE\\Policies\\Microsoft\\Windows\\AppCompat\\DisableUAR"),
			1
		},
		{
			Registry::RegInformation::HKLM,
			TEXT("SYSTEM\\ControlSet001\\Control\\WMI\\AutoLogger\\AutoLogger-Diagtrack-Listener"),
			0
		},
		{
			Registry::RegInformation::HKLM,
			TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\DataCollection\\AllowTelemetry"),
			0
		},
		{
			Registry::RegInformation::HKLM,
			TEXT("SOFTWARE\\Policies\\Microsoft\\SQMClient\\Windows\\CEIPEnable"),
			0
		},
		{
			Registry::RegInformation::HKLM,
			TEXT("SOFTWARE\\Policies\\Microsoft\\Windows\\AppCompat\\AITEnable"),
			0
		},
		{
			Registry::RegInformation::HKLM,
			TEXT("SOFTWARE\\Policies\\Microsoft\\Windows\\DataCollection\\AllowTelemetry"),
			0
		},
		{
			Registry::RegInformation::HKLM,
			TEXT("SYSTEM\\CurrentControlSet\\Control\\WMI\\AutoLogger\\AutoLogger-Diagtrack-Listener\\Start"),
			0
		},
		{
			Registry::RegInformation::HKLM,
			TEXT("SYSTEM\\CurrentControlSet\\Control\\WMI\\AutoLogger\\SQMLogger\\Start"),
			0
		},
		{
			Registry::RegInformation::HKCU,
			TEXT("SOFTWARE\\Microsoft\\Siuf\\Rules\\NumberOfSIUFInPeriod"),
			0
		}
	};
	
	struct ServListItem {
		String name;
		SCService::ST_TYPE start;
	};

	static std::list < ServListItem > services
	{
		{ TEXT("DiagTrack"),								SCService::DISABLE},
		{ TEXT("diagnosticshub.standardcollector.service"), SCService::DISABLE },
		{ TEXT("dmwappushservice"),							SCService::DISABLE },
		{ TEXT("WMPNetworkSvc"),							SCService::DISABLE }
	};


	void RemoveTelemetry::init()
	{
		bool isRegistrySetted = true;

		Utility& inst = getUtility();
		RegistryManager& rc = inst.getRegManager();

		for (RegistryFixerItem _item : regEditList)
		{
			try {
				auto  value = rc.getKey(_item._br, _item.path).getValue();
				DWORD* realValue = reinterpret_cast<DWORD*>(
					const_cast<typename String::traits_type::char_type*>(value.c_str())
					);
				if ( *realValue != _item.value)
				{
					isRegistrySetted = false;
					U_WARNING << "Value " << _item.path << " != " << _item.value << "\r\n";
					break;
				}
			}
			catch (...)
			{
#pragma message("normal catch!")
				U_WARNING << "Some error...";
			}
		}

		//list services and check start type
		SCManager& sc = inst.getServiceManager();
		for (ServListItem _item : services)
		{
			if (sc.getService(_item.name)->getStartType() != _item.start)
			{
				isRegistrySetted = false;
				U_WARNING << "Service " << _item.name << " not set need start type\r\n";
			}
		}
		//skip check and run always
		m_applyed = isRegistrySetted;
	}

	bool RemoveTelemetry::exec()
	{
		Utility& inst = getUtility();

		RegistryManager& rc = inst.getRegManager();

		for (RegistryFixerItem _item : regEditList)
		{
			try {
				auto key = rc.getKey(_item._br, _item.path);
				key.setValue(to_string( _item.value ));
			}
			catch (...)
			{
#pragma message("normal catch!")
				U_WARNING << "Some error...";
			}
		}

		SCManager& sc = inst.getServiceManager();
		for (ServListItem _item : services)
		{
			if (sc.getService(_item.name)->setStartType(_item.start) )
			{
				U_WARNING << "Service " << _item.name << " not set need start type\r\n";
			}
		}

		rc.getKey(Registry::RegInformation::HKCU,
			TEXT("SOFTWARE\\Microsoft\\Siuf\\Rules\\PeriodInNanoSeconds\\ ")
			).remove();
		return false;
	}
}