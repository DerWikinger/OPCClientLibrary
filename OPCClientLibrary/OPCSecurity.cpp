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
