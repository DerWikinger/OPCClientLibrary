#include "pch.h"
#include "OPCSecurity.h"

using namespace OPCClientLibrary;

COSERVERINFO* OPCSecurity::GetServerInfo()
{
	COAUTHIDENTITY* pAuthIdentity = NULL;

	if (!_username.empty()) {
		pAuthIdentity = this->GetAuthIdentity();

		BSTR str = _com_util::ConvertStringToBSTR(_principalName.c_str());
		LPWSTR principalName = LPWSTR(str);

		pAuthInfo = new COAUTHINFO();
		pAuthInfo->pAuthIdentityData = pAuthIdentity;
		pAuthInfo->dwAuthnSvc = _authnSvc;
		pAuthInfo->pwszServerPrincName = _principalName.empty() ? NULL : principalName;
		pAuthInfo->dwAuthnLevel = _authnLevel;
		pAuthInfo->dwImpersonationLevel = _impersonateLevel;
		pAuthInfo->dwCapabilities = _capabilities;
		pAuthInfo->dwAuthzSvc = _authzSvc;
	}

	COSERVERINFO* pServerInfo = new COSERVERINFO();
	pServerInfo->dwReserved1 = 0;
	pServerInfo->dwReserved2 = 0;
	BSTR name = _com_util::ConvertStringToBSTR(_hostname.c_str());
	pServerInfo->pwszName = name;
	pServerInfo->pAuthInfo = pAuthInfo;

	return pServerInfo;
}

COAUTHIDENTITY* OPCSecurity::GetAuthIdentity() {

	COAUTHIDENTITY* pAuthIdentity = new COAUTHIDENTITY();
	pAuthIdentity->Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;

	ULONG usernameLength = _username.length();
	USHORT* pUsername = 0;
	if (usernameLength > 0) {
		pUsername = new USHORT[usernameLength];
		for (int i = 0; i < usernameLength; i++)
		{
			pUsername[i] = _username.at(i);
		}
		pAuthIdentity->User = pUsername;
		pAuthIdentity->UserLength = usernameLength;
	}

	ULONG passwordLength = _password.length();
	USHORT* pPassword = 0;
	if (passwordLength > 0) {
		pPassword = new USHORT[passwordLength];
		for (int i = 0; i < passwordLength; i++)
		{
			pPassword[i] = _password.at(i);
		}
		pAuthIdentity->Password = pPassword;
		pAuthIdentity->PasswordLength = passwordLength;
	}

	ULONG domainLength = _domain.length();
	USHORT* pDomain = 0;
	if (domainLength > 0) {
		pDomain = new USHORT[domainLength];
		for (int i = 0; i < domainLength; i++)
		{
			pDomain[i] = _domain.at(i);
		}
		pAuthIdentity->Domain = pDomain;
		pAuthIdentity->DomainLength = domainLength;
	}

	return pAuthIdentity;
}

HRESULT OPCSecurity::InitializeSecurity() {
	
	SOLE_AUTHENTICATION_INFO* authInfo = new SOLE_AUTHENTICATION_INFO();
	SecureZeroMemory(authInfo, sizeof(SOLE_AUTHENTICATION_INFO));

	authInfo->dwAuthnSvc = RPC_C_AUTHN_WINNT;
	authInfo->dwAuthzSvc = RPC_C_AUTHZ_NONE;
	authInfo->pAuthInfo = pAuthInfo;

	SOLE_AUTHENTICATION_LIST* authInfoList = new SOLE_AUTHENTICATION_LIST();
	authInfoList->cAuthInfo = 1;
	authInfoList->aAuthInfo = authInfo;

	HRESULT hRes;

	hRes = CoInitializeSecurity(
		pSecDescr,
		-1,
		NULL,
		NULL,
		_authnLevel,
		_impersonateLevel,
		authInfoList,
		_capabilities,
		NULL);

	if (FAILED(hRes)) {
		throw hRes;
	}

	return hRes;
}

//hRes = CoSetProxyBlanket(
//	_server,
//	RPC_C_AUTHN_GSS_NEGOTIATE,
//	RPC_C_AUTHZ_NONE,
//	NULL,
//	//_serverInfo->pAuthInfo->pwszServerPrincName,
//	RPC_C_AUTHN_LEVEL_CONNECT,
//	RPC_C_IMP_LEVEL_DELEGATE,
//	_serverInfo->pAuthInfo->pAuthIdentityData,
//	EOAC_MUTUAL_AUTH
//	);
//DWORD* pwAuthnSvc = new DWORD();
//DWORD* pwAuthzSvc = new DWORD();
//LPOLESTR* princName = new LPOLESTR();
//DWORD* pAuthnLvl = new DWORD();
//DWORD* pImpLvl = new DWORD();
//RPC_AUTH_IDENTITY_HANDLE* pAuthHandle = new RPC_AUTH_IDENTITY_HANDLE();
//DWORD* pCabability = new DWORD();

// RPC_C_AUTHN_WINNT = 10

//hRes = CoQueryProxyBlanket(
//	_server,
//	pwAuthnSvc, // RPC_C_AUTHN_GSS_NEGOTIATE = 9
//	pwAuthzSvc, // RPC_C_AUTHZ_NONE = 0
//	//NULL,
//	princName, // CEE-NAUL\\ETL
//	pAuthnLvl, // RPC_C_AUTHN_LEVEL_PKT_INTEGRITY = 5
//	pImpLvl, // RPC_C_IMP_LEVEL_IDENTIFY = 2
//	pAuthHandle,
//	pCabability // EOAC_MUTUAL_AUTH = 1
//);
//IClientSecurity * pIClientSecurity;
//hRes = _server->QueryInterface(IID_IClientSecurity, (void**)&pIClientSecurity);
//if (FAILED(hRes))
//{
//	throw hRes;
//}
