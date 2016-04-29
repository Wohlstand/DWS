#include "DWS_Kernel.h"
#include "DWS_Utility.h"
#include <netfw.h>

namespace Firewall{
//https://msdn.microsoft.com/en-us/library/windows/desktop/aa366418(v=vs.85).aspx

#pragma comment( lib, "ole32.lib" )
#pragma comment( lib, "oleaut32.lib" )

#pragma todo("ADD CHECK RULE EXITS")
#pragma todo("ADD REMOVE RULE BY NAME")

	struct FireWallSettings
	{
		BOOL EnableFirewall;
		BOOL BlockAllInTraffic;
		BOOL Notifications;
		BOOL UnicastResponsesToMulticastBroadcast;
		BOOL DefaultInAction;
		BOOL DefaultOutAction;
	};

	struct FirewallState
	{
		FireWallSettings Domain;
		FireWallSettings Private;
		FireWallSettings Public;
	};

	enum RuleType
	{
		Out,
		In
	};

	// Forward declarations
	FireWallSettings	Get_FirewallSettings_PerProfileType(NET_FW_PROFILE_TYPE2 ProfileTypePassed, INetFwPolicy2* pNetFwPolicy2);
	HRESULT	WFCOMInitialize(INetFwPolicy2** ppNetFwPolicy2);

	BOOL EnableFirewall()
	{
		HRESULT hrComInit = S_OK;
		HRESULT hr = S_OK;

		INetFwPolicy2 *pNetFwPolicy2 = NULL;

		// Initialize COM.
		hrComInit = CoInitializeEx(
			0,
			COINIT_APARTMENTTHREADED
			);

		// Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
		// initialized with a different mode. Since we don't care what the mode is,
		// we'll just use the existing mode.
		if (hrComInit != RPC_E_CHANGED_MODE)
		{
			if (FAILED(hrComInit))
			{
				goto Cleanup;
			}
		}

		// Retrieve INetFwPolicy2
		hr = WFCOMInitialize(&pNetFwPolicy2);
		if (FAILED(hr))
		{
			goto Cleanup;
		}

		// Enable Windows Firewall for the Domain profile
		hr = pNetFwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_DOMAIN, TRUE);
		if (FAILED(hr))
		{
			goto Cleanup;
		}

		// Enable Windows Firewall for the Private profile
		hr = pNetFwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_PRIVATE, TRUE);
		if (FAILED(hr))
		{
			goto Cleanup;
		}

		// Enable Windows Firewall for the Public profile
		hr = pNetFwPolicy2->put_FirewallEnabled(NET_FW_PROFILE2_PUBLIC, TRUE);
		if (FAILED(hr))
		{
			goto Cleanup;
		}

	Cleanup:

		// Release INetFwPolicy2
		if (pNetFwPolicy2 != NULL)
		{
			pNetFwPolicy2->Release();
		}

		// Uninitialize COM.
		if (SUCCEEDED(hrComInit))
		{
			CoUninitialize();
		}

		return SUCCEEDED(hr);
	}


	FirewallState FirewallStatus()
	{
		HRESULT hrComInit = S_OK;
		HRESULT hr = S_OK;
		FirewallState fw_state;
		INetFwPolicy2 *pNetFwPolicy2 = NULL;

		// Initialize COM.
		hrComInit = CoInitializeEx(
			0,
			COINIT_APARTMENTTHREADED
			);

		// Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
		// initialized with a different mode. Since we don't care what the mode is,
		// we'll just use the existing mode.
		if (hrComInit != RPC_E_CHANGED_MODE)
		{
			if (FAILED(hrComInit))
			{
				goto Cleanup;
			}
		}

		// Retrieve INetFwPolicy2
		hr = WFCOMInitialize(&pNetFwPolicy2);
		if (FAILED(hr))
		{
			goto Cleanup;
		}

		fw_state.Domain = Get_FirewallSettings_PerProfileType(NET_FW_PROFILE2_DOMAIN, pNetFwPolicy2);

		fw_state.Private = Get_FirewallSettings_PerProfileType(NET_FW_PROFILE2_PRIVATE, pNetFwPolicy2);

		fw_state.Public = Get_FirewallSettings_PerProfileType(NET_FW_PROFILE2_PUBLIC, pNetFwPolicy2);

	Cleanup:

		// Release INetFwPolicy2
		if (pNetFwPolicy2 != NULL)
		{
			pNetFwPolicy2->Release();
		}

		// Uninitialize COM.
		if (SUCCEEDED(hrComInit))
		{
			CoUninitialize();
		}

		return fw_state;
	}

	//Adding an Rule
	BOOL AddRule(String RuleName, String RuleDescription, String RuleGroup, String RuleAddr, RuleType rule_type)
	{
		BOOL _success = TRUE;
		HRESULT hrComInit = S_OK;
		HRESULT hr = S_OK;

		INetFwPolicy2 *pNetFwPolicy2 = NULL;
		INetFwRules *pFwRules = NULL;
		INetFwRule *pFwRule = NULL;
		
		BSTR bstrRuleName = SysAllocString(RuleName.c_str());
		BSTR bstrRuleDescription = SysAllocString(RuleDescription.c_str());
		BSTR bstrRuleGroup = SysAllocString(RuleGroup.c_str());
		BSTR bstrRuleLAddr = SysAllocString(RuleAddr.c_str());

		// Initialize COM.
		hrComInit = CoInitializeEx(
			0,
			COINIT_APARTMENTTHREADED
			);

		// Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
		// initialized with a different mode. Since we don't care what the mode is,
		// we'll just use the existing mode.
		if (hrComInit != RPC_E_CHANGED_MODE)
		{
			if (FAILED(hrComInit))
			{
				_success = FALSE;
				goto Cleanup;
			}
		}

		// Retrieve INetFwPolicy2
		hr = WFCOMInitialize(&pNetFwPolicy2);
		if (FAILED(hr))
		{
			goto Cleanup;
		}

		// Retrieve INetFwRules
		hr = pNetFwPolicy2->get_Rules(&pFwRules);
		if (FAILED(hr))
		{
			_success = FALSE;
			goto Cleanup;
		}

		// Create a new Firewall Rule object.
		hr = CoCreateInstance(
			__uuidof(NetFwRule),
			NULL,
			CLSCTX_INPROC_SERVER,
			__uuidof(INetFwRule),
			(void**)&pFwRule);
		if (FAILED(hr))
		{
			_success = FALSE;
			goto Cleanup;
		}

		// Populate the Firewall Rule object
		pFwRule->put_Name(bstrRuleName);
		pFwRule->put_Description(bstrRuleDescription);
		pFwRule->put_RemoteAddresses(bstrRuleLAddr);
		pFwRule->put_Protocol(NET_FW_IP_PROTOCOL_ANY);
		if(rule_type == Out)
			pFwRule->put_Direction(NET_FW_RULE_DIR_OUT);
		else
			pFwRule->put_Direction(NET_FW_RULE_DIR_IN);
		pFwRule->put_Grouping(bstrRuleGroup);
		pFwRule->put_Profiles(NET_FW_PROFILE2_ALL);
		pFwRule->put_Action(NET_FW_ACTION_ALLOW);
		pFwRule->put_Enabled(VARIANT_TRUE);

		// Add the Firewall Rule
		hr = pFwRules->Add(pFwRule);
		if (FAILED(hr))
		{
			_success = FALSE;
			goto Cleanup;
		}

	Cleanup:

		// Free BSTR's
		SysFreeString(bstrRuleName);
		SysFreeString(bstrRuleDescription);
		SysFreeString(bstrRuleGroup);
		SysFreeString(bstrRuleLAddr);

		// Release the INetFwRule object
		if (pFwRule != NULL)
		{
			pFwRule->Release();
		}

		// Release the INetFwRules object
		if (pFwRules != NULL)
		{
			pFwRules->Release();
		}

		// Release the INetFwPolicy2 object
		if (pNetFwPolicy2 != NULL)
		{
			pNetFwPolicy2->Release();
		}

		// Uninitialize COM.
		if (SUCCEEDED(hrComInit))
		{
			CoUninitialize();
		}

		return _success;
	}


	// Instantiate INetFwPolicy2
	HRESULT WFCOMInitialize(INetFwPolicy2** ppNetFwPolicy2)
	{
		HRESULT hr = S_OK;

		hr = CoCreateInstance(
			__uuidof(NetFwPolicy2),
			NULL,
			CLSCTX_INPROC_SERVER,
			__uuidof(INetFwPolicy2),
			(void**)ppNetFwPolicy2);

		if (FAILED(hr))
		{
			goto Cleanup;
		}

	Cleanup:
		return hr;
	}

	FireWallSettings Get_FirewallSettings_PerProfileType(NET_FW_PROFILE_TYPE2 ProfileTypePassed, INetFwPolicy2* pNetFwPolicy2)
	{
		VARIANT_BOOL bIsEnabled = FALSE;
		NET_FW_ACTION action;
		FireWallSettings fw_settings;

		if (SUCCEEDED(pNetFwPolicy2->get_FirewallEnabled(ProfileTypePassed, &bIsEnabled)))
		{
			fw_settings.EnableFirewall = bIsEnabled;
		}

		if (SUCCEEDED(pNetFwPolicy2->get_BlockAllInboundTraffic(ProfileTypePassed, &bIsEnabled)))
		{
			fw_settings.BlockAllInTraffic = bIsEnabled;
		}

		if (SUCCEEDED(pNetFwPolicy2->get_NotificationsDisabled(ProfileTypePassed, &bIsEnabled)))
		{
			fw_settings.Notifications = bIsEnabled;
		}

		if (SUCCEEDED(pNetFwPolicy2->get_UnicastResponsesToMulticastBroadcastDisabled(ProfileTypePassed, &bIsEnabled)))
		{
			fw_settings.UnicastResponsesToMulticastBroadcast = bIsEnabled;
		}

		if (SUCCEEDED(pNetFwPolicy2->get_DefaultInboundAction(ProfileTypePassed, &action)))
		{
			fw_settings.DefaultInAction = action != NET_FW_ACTION_BLOCK;
		}

		if (SUCCEEDED(pNetFwPolicy2->get_DefaultOutboundAction(ProfileTypePassed, &action)))
		{
			fw_settings.DefaultOutAction = action != NET_FW_ACTION_BLOCK;
		}

		return fw_settings;
	}
}