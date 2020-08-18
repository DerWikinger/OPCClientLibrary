#pragma once
#include <string>

#ifdef OPCCLIENTLIBRARY_EXPORTS
#define OPCCLIENTLIBRARY_API __declspec(dllexport)
#else
#define OPCCLIENTLIBRARY_API __declspec(dllimport)
#endif

namespace OPCClientLibrary {
	
	using namespace std;
	using namespace OpcEnumLib;
	using namespace OPCDA;

	class OPCCLIENTLIBRARY_API OPCSecurity
	{
		string _hostname;
		string _username;
		string _password;
		string _domain;
		string _principalName;

		DWORD _authnLevel;
		DWORD _authnSvc;
		//DWORD _authzLevel;
		DWORD _authzSvc;
		DWORD _impersonateLevel;
		DWORD _capabilities;

		COAUTHINFO* pAuthInfo = NULL;
		SECURITY_DESCRIPTOR* pSecDescr = NULL;

	public:
		explicit OPCSecurity(const string &hostname = "localhost", const string &username = "", 
			const string &password = "", const string &domain = "") :
			_hostname(hostname), 
			_username(username), 
			_password(password), 
			_domain(domain), 
			_principalName(""),
			_authnLevel(RPC_C_AUTHN_LEVEL_CONNECT), 
			_authnSvc(RPC_C_AUTHN_WINNT), 
			_authzSvc(RPC_C_AUTHZ_NONE),
			_impersonateLevel(RPC_C_IMP_LEVEL_IMPERSONATE), 
			_capabilities(EOAC_NONE)
		{}
		virtual ~OPCSecurity() {}

		COAUTHIDENTITY* GetAuthIdentity();
		COAUTHINFO* GetAuthInfo()
		{
			return pAuthInfo;
		}
		COSERVERINFO* GetServerInfo();
		HRESULT InitializeSecurity();
		
		const string HostName() const {
			return _hostname;
		}

		const string HostName(string value) {
			return _hostname = value;
		}

		const string Username() const {
			return _username;
		}

		const string Username(string value) {
			return _username = value;
		}

		const string Password() const {
			return _password;
		}

		const string Password(string value) {
			return _password = value;
		}

		const string Domain() const {
			return _domain;
		}

		const string Domain(string value) {
			return _domain = value;
		}

		const string PrincipalName() const {
			return _principalName;
		}

		const string PrincipalName(string value) {
			return _principalName = value;
		}

		const DWORD AuthnLevel() const {
			return _authnLevel;
		}

		const DWORD AuthnLevel(DWORD value) {
			return _authnLevel = value;
		}

		const DWORD AuthnSvc() const {
			return _authnSvc;
		}

		const DWORD AuthnSvc(DWORD value) {
			return _authnSvc = value;
		}

		const SECURITY_DESCRIPTOR* SecurityDescriptor() const {
			return pSecDescr;
		}

		const SECURITY_DESCRIPTOR* SecurityDescriptor(SECURITY_DESCRIPTOR *value) {
			return pSecDescr = value;
		}

		const DWORD AuthzSvc() const {
			return _authzSvc;
		}

		const DWORD AuthzSvc(DWORD value) {
			return _authzSvc = value;
		}

		const DWORD ImpersonateLevel() const {
			return _impersonateLevel;
		}

		const DWORD ImpersonateLevel(DWORD value) {
			return _impersonateLevel = value;
		}

		const DWORD Capabilities() const {
			return _capabilities;
		}

		const DWORD Capabilities(DWORD value) {
			return _capabilities = value;
		}
	};
}

