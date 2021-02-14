// File Example1Client.cpp
#include <iostream>
#include "..\RPC-Interface1\Interface1-Explicit_h.h"

int main()
{
	RPC_STATUS status;
	RPC_WSTR szStringBinding = NULL;
	RPC_WSTR pszProtSeq = (RPC_WSTR)L"ncacn_ip_tcp"; //reinterpret_cast<RPC_WSTR>(L"ncacn_ip_tcp");
	RPC_WSTR pszTCPHost = (RPC_WSTR)L"Spaceland-SRV-1"; //reinterpret_cast<RPC_WSTR>(L"localhost");
	RPC_WSTR pszTCPPort = (RPC_WSTR)L"8989"; //reinterpret_cast<RPC_WSTR>(L"8989");
	RPC_WSTR pszHostSPN = (RPC_WSTR)L"Host/SPACELAND-SRV-1"; //reinterpret_cast<RPC_WSTR>(L"8989");

	// Creates a string binding handle.
	// Connection is not done here.
	wprintf(L"Create string binding to '%s:%s' using RpcStringBindingCompose()\n", pszTCPHost, pszTCPPort);
	status = RpcStringBindingCompose(
		NULL,           // UUID to bind to.
		pszProtSeq,		// Use TCP/IP protocol.
		pszTCPHost,		// TCP/IP network address to use.
		pszTCPPort,     // TCP/IP port to use.
		NULL,           // Protocol dependent network options to use.
		&szStringBinding);	// String binding output.

	if (status)
		exit(status);

	handle_t hExplicitBinding = NULL;
	// Create a valid binding handle from String
	status = RpcBindingFromStringBinding(
		szStringBinding,	// The string binding to validate.
		&hExplicitBinding	// Put the result in the implicit binding
	);						// handle defined in the IDL file.

	wprintf(L"Set Binding authentication info to SPN '%s'...", pszHostSPN);
	status = RpcBindingSetAuthInfo(
		hExplicitBinding,		// the client's binding handle
		pszHostSPN,				// the server's service principale name (SPN)
		RPC_C_AUTHN_LEVEL_PKT,	// authentication level as defined at https://docs.microsoft.com/en-us/windows/win32/rpc/authentication-level-constants
		RPC_C_AUTHN_WINNT,		// using NTLM as authentication service provider as defined at https://docs.microsoft.com/en-us/windows/win32/rpc/authentication-service-constants
		NULL,					// use current thread credentials
		RPC_C_AUTHZ_NAME		// authorization based on the provided SPN as defined at https://docs.microsoft.com/en-us/windows/win32/com/com-authorization-constants
	);
	if (status) {
		wprintf(L"Failed. Error: %s\n", status);
		exit(status);
	}
	else wprintf(L"Success.\n");

	printf("Trying to connect...\n");
	RpcTryExcept
	{
		// Calls the RPC function. The hExample1Binding binding handle
		// is used implicitly (as defined in the Interface IDL file).
		// Connection is done here.
		int ret = Output(hExplicitBinding, "Hello From Client!");
		ret = Output(hExplicitBinding, "Triggering Remote Shutdown now...");
		std::cout << "Return value was: " << ret << std::endl;
		Shutdown(hExplicitBinding);
	}
		RpcExcept(1)
	{
		std::cerr << "Runtime reported exception " << RpcExceptionCode()
			<< std::endl;
	}
	RpcEndExcept

		// Free the memory allocated by a string.
		status = RpcStringFree(
			&szStringBinding); // String to be freed.

	if (status)
		exit(status);

	// Releases binding handle resources and disconnects from the server.
	status = RpcBindingFree(
		&hExplicitBinding); // Frees the implicit binding handle defined in the IDL file.

	if (status)
		exit(status);
}


/******************************************************/
/*         MIDL allocate and free                     */
/*		   The runtime uses these two functions for	  */
/*		   allocating / deallocating				  */
/******************************************************/

void __RPC_FAR * __RPC_USER midl_user_allocate(size_t len)
{
	return(malloc(len));
}

void __RPC_USER midl_user_free(void __RPC_FAR * ptr)
{
	free(ptr);
}