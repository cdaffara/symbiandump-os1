// console_host.cpp
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "stdafx.h"
#include "resource.h"
#include "Misc.h"
#include "Window.h"
#include "ConsoleWindow.h"
#include <winsock2.h>
#include "Server.h"

void InitWinSock()
	{
	WORD version = MAKEWORD (2, 2);
	WSADATA wsaData;

	if (WSAStartup(version, &wsaData) == SOCKET_ERROR)
		{
		throw KExceptionWinSockStartupFailed;
		}

	if (version != wsaData.wVersion)
		{
		throw KExceptionWinSockVersionInvalid;
		}
	}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
	{
	MSG msg;
	HACCEL hAccelTable;
	CServer* server;
	try
		{
		hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_CONSOLE_HOST);
		InitWinSock();
		server = CServer::New(hInstance);
		}
	catch (TException aException)
		{
		LPCTSTR errorTitle = TEXT("Remote Console Server Initialization Error");
		LPCTSTR errorText = NULL;
		switch (aException)
			{
			case KExceptionNoMemory:
				errorText = TEXT("Not enough memory to initialize server.");
				break;
			case KExceptionWindowConstructFailed:
				errorText = TEXT("Failed to construct server window.");
				break;
			case KExceptionWinSockStartupFailed:
				errorText = TEXT("Failed to initialize WINSOCK.");
				break;
			case KExceptionWinSockVersionInvalid:
				errorText = TEXT("Invalid WINSOCK version.");
				break;
			case KExceptionSocketConstructFailed:
				errorText = TEXT("Failed to construct server socket.");
				break;
			default:
				errorText = TEXT("Unknown error.");
				break;
			}
		MessageBox(NULL, errorText, errorTitle, MB_OK);
		WSACleanup();
		return 0;
		}

	while (GetMessage(&msg, NULL, 0, 0)) 
		{
		try
			{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
				{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				}
			}
		catch (TException aException)
			{
			server->HandleException(aException);
			}
		}

	delete server;
	WSACleanup();

	return msg.wParam;
	}
