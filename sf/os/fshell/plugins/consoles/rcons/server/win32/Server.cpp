// Server.cpp
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

#include "Misc.h"
#include "stdafx.h"
#include "console_host.h"
#include "Server.h"
#include "ConsoleWindow.h"
#include "RemoteConsole.h"

const int KMajorVersion = 1;
const int KServerInitParam = ID_RC_CLOSE - 1;
static CServer* sServer = NULL;
LPCTSTR KServerWindowClassName = TEXT("ServerWindowClass");
LPCTSTR KServerConsoleName = TEXT("Server Console");
LPCTSTR KServerSocketName = TEXT("Server Socket");


class TCreateConsoleWindowPacket
	{
public:
	void Validate() const;
public:
	int iMajorVersion;
	int iMinorVersion;
	int iWidth;
	int iHeight;
	int iTitleLength;
	};

void TCreateConsoleWindowPacket::Validate() const
	{
	if (iMajorVersion > KMajorVersion)
		{
		throw KExceptionInvalidClientVersion;
		}
	}


CServer* CServer::New(HINSTANCE aAppHandle)
	{
	std::auto_ptr<CServer> self(new(EThrow) CServer(aAppHandle));
	self->Construct();
	return self.release();
	}

CServer::~CServer()
	{
	delete iSocket;
	delete iConsole;
	}

void CServer::HandleException(TException aException)
	{
	switch (aException)
		{
		case KExceptionNoMemory:
			iConsole->Write(TEXT("Out of memory\r\n"));
			break;
		case KExceptionWindowConstructFailed:
			iConsole->Write(TEXT("Failed to construct new window\r\n"));
			break;
		case KExceptionConsoleWindowConstructFailed:
			iConsole->Write(TEXT("Failed to construct new console window\r\n"));
			break;
		case KExceptionSocketConstructFailed:
			iConsole->Write(TEXT("Failed to construct new socket\r\n"));
			break;
		case KExceptionSocketBindFailed:
			iConsole->Write(TEXT("Failed to bind server socket. Is something else already listening on that port?\r\n"));
			break;
		case KExceptionSocketListenFailed:
			iConsole->Write(TEXT("Failed to listen on server socket\r\n"));
			break;
		case KExceptionSocketSelectFailed:
			iConsole->Write(TEXT("Failed to asynchronously wait on socket\r\n"));
			break;
		case KExceptionSocketAcceptFailed:
			iConsole->Write(TEXT("Failed to accept to client connection\r\n"));
			break;
		case KExceptionSocketReadFailed:
			iConsole->Write(TEXT("Failed to read from socket\r\n"));
			break;
		case KExceptionInvalidClientVersion:
			iConsole->Write(TEXT("Invalid client version\r\n"));
			break;
		case KExceptionFailedToWritePrefsFile:
			iConsole->Write(TEXT("Failed to write preferences file\r\n"));
			break;
		default:
			iConsole->Write(TEXT("Unknown error\r\n"));
			break;
		}
	}

TPreferences& CServer::Preferences()
	{
	return iPreferences;
	}

const TPreferences& CServer::Preferences() const
	{
	return iPreferences;
	}

CServer::CServer(HINSTANCE aAppHandle) : iAppHandle(aAppHandle), iSocket(NULL), iConsole(NULL), iPort(0)
	{
	sServer = this;
	}

void CServer::Construct()
	{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)CWindow::HandleMessage;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= iAppHandle;
	wcex.hIcon			= LoadIcon(iAppHandle, (LPCTSTR)IDI_CONSOLE_HOST);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= (LPCTSTR)ID_SERVER_MENU;
	wcex.lpszClassName	= KServerWindowClassName;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	if (RegisterClassEx(&wcex) == 0)
		{
		throw KExceptionWindowClassRegistrationFailed;
		}
	
	iConsole = CConsoleWindow::New(iAppHandle, KServerWindowClassName, TEXT("Remote Console Server"), CW_USEDEFAULT, CW_USEDEFAULT, 40, 20, 20, this, NULL, TRUE);
	iConsole->SetName(KServerConsoleName);

	try
		{
		iPreferences.Read();
		}
	catch (...)
		{
		iConsole->Write(TEXT("Unable to read preferences"));
		}

	iSocket = CServerSocket::New(*iConsole);
	iSocket->SetName(KServerSocketName);
	PostMessage(iConsole->Handle(), WM_COMMAND, KServerInitParam, 0);
	}

void CServer::Listen()
	{
	if (iPort != iPreferences.ServerPort())
		{
		delete iSocket;
		iSocket = CServerSocket::New(*iConsole);
		iSocket->SetName(KServerSocketName);
		iSocket->Listen(iPreferences.ServerPort(), 5, *this);
		iConsole->WriteFormat(TEXT("Listening on port %d\r\n"), iPreferences.ServerPort());
		iPort = iPreferences.ServerPort();
		}
	}

void CServer::HandleNewClient(CClientSocket& aClientSocket)
	{
	CSocketCommandReader* commandReader = CSocketCommandReader::New(aClientSocket, *this);
	commandReader->ReadCommand();
	}

void CServer::HandleSocketClosure(CSocket& aSocket)
	{
	ASSERT(&aSocket == iSocket);
	PostQuitMessage(0);
	}

void CServer::HandleSocketCommand(TPacketHeader::TPacketType aCommand, const char* aPacket, int aPacketLength, CSocketCommandReader& aReader)
	{
	switch (aCommand)
		{
		case TPacketHeader::ECommandCreateConsoleWindow:
			{
			iConsole->Write(TEXT("Creating new console window...\r\n"));
			const TCreateConsoleWindowPacket* createConsoleWindowPacket = new((char*)aPacket) TCreateConsoleWindowPacket;
			LPTSTR title = (TCHAR*)((char*)aPacket + sizeof(TCreateConsoleWindowPacket));
			try
				{
				createConsoleWindowPacket->Validate();
				CRemoteConsole* remoteConsole = CRemoteConsole::New(iAppHandle, aReader, title, createConsoleWindowPacket->iWidth, createConsoleWindowPacket->iHeight, iPreferences);
				int consoleId = remoteConsole->Id();
				iConsole->WriteFormat(TEXT("Created window \"%s\" (%d)...\r\n"), title, consoleId);
				aReader.SendResponse((char*)&consoleId, sizeof(int));
				}
			catch (TException aException)
				{
				int err = -2; // KErrGeneral.
				if (aException == KExceptionInvalidClientVersion)
					{
					err = -21; // KErrAccessDenied.
					}
				aReader.SendResponse((char*)&err, sizeof(int));
				delete &aReader;
				throw aException;
				}
			catch (...)
				{
				delete &aReader;
				throw KExceptionUnknown;
				}
			break;
			}
		case TPacketHeader::ECommandAttachKeyEventSocket:
			{
			iConsole->Write(TEXT("Attaching key event socket...\r\n"));
			int* consoleId = new((char*)aPacket) int;
			CRemoteConsole* remoteConsole = CRemoteConsole::Instance(*consoleId);
			if (remoteConsole)
				{
				remoteConsole->AttachKeyEventSocket(aReader.ReleaseSocket());
				iConsole->WriteFormat(TEXT("Attached key event socket to window %d...\r\n"), *consoleId);
				}
			delete &aReader;
			break;
			}
		default:
			{
			iConsole->WriteFormat(TEXT("Invalid server socket command: %d\r\n"), aCommand);
			delete &aReader;
			break;
			}
		}
	}

void CServer::HandleSocketClosure(CSocketCommandReader& aReader)
	{
	delete &aReader;
	}

void CServer::HandleWindowClosure(CWindow& aWindow)
	{
	ASSERT(&aWindow == iConsole);
	PostQuitMessage(0);
	}

LRESULT CALLBACK PrefsCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
	{
	switch (message)
		{
		case WM_INITDIALOG:
			TCHAR string[128];
			swprintf(string, TEXT("%u"), sServer->Preferences().ServerPort());
			SetDlgItemText(hDlg, ID_EDIT_PORT, string);
			swprintf(string, TEXT("%u"), sServer->Preferences().DefaultWindowWidth());
			SetDlgItemText(hDlg, ID_EDIT_WIN_WIDTH, string);
			swprintf(string, TEXT("%u"), sServer->Preferences().DefaultWindowHeight());
			SetDlgItemText(hDlg, ID_EDIT_WIN_HEIGHT, string);
			swprintf(string, TEXT("%u"), sServer->Preferences().DefaultWindowPosX());
			SetDlgItemText(hDlg, ID_EDIT_WIN_POS_X, string);
			swprintf(string, TEXT("%u"), sServer->Preferences().DefaultWindowPosY());
			SetDlgItemText(hDlg, ID_EDIT_WIN_POS_Y, string);
			EnableWindow(GetDlgItem(hDlg, ID_EDIT_WIN_POS_X), !sServer->Preferences().SystemPositionedWindows());
			EnableWindow(GetDlgItem(hDlg, ID_EDIT_WIN_POS_Y), !sServer->Preferences().SystemPositionedWindows());
			CheckDlgButton(hDlg, ID_CHECK_WIN_POS, sServer->Preferences().SystemPositionedWindows());
			swprintf(string, TEXT("%u"), sServer->Preferences().NumOverflowLines());
			SetDlgItemText(hDlg, ID_OVERFLOW_LINES, string);
			return TRUE;
		case WM_COMMAND:
			switch (LOWORD(wParam))
				{
				case IDOK:
					{
					TCHAR string[128];
					GetDlgItemText(hDlg, ID_EDIT_PORT, string, 128);
					unsigned int val;
					swscanf(string, TEXT("%u"), &val);
					sServer->Preferences().SetServerPort((unsigned short)val);
					GetDlgItemText(hDlg, ID_EDIT_WIN_WIDTH, string, 128);
					swscanf(string, TEXT("%u"), &val);
					sServer->Preferences().SetDefaultWindowWidth(val);
					GetDlgItemText(hDlg, ID_EDIT_WIN_HEIGHT, string, 128);
					swscanf(string, TEXT("%u"), &val);
					sServer->Preferences().SetDefaultWindowHeight(val);
					GetDlgItemText(hDlg, ID_EDIT_WIN_POS_X, string, 128);
					swscanf(string, TEXT("%u"), &val);
					sServer->Preferences().SetDefaultWindowPosX(val);
					GetDlgItemText(hDlg, ID_EDIT_WIN_POS_Y, string, 128);
					swscanf(string, TEXT("%u"), &val);
					sServer->Preferences().SetDefaultWindowPosY(val);
					GetDlgItemText(hDlg, ID_OVERFLOW_LINES, string, 128);
					swscanf(string, TEXT("%u"), &val);
					sServer->Preferences().SetNumOverflowLines(val);
					sServer->Preferences().SetSystemPositionedWindows((IsDlgButtonChecked(hDlg, ID_CHECK_WIN_POS) == BST_CHECKED) ? TRUE : FALSE);
					sServer->Preferences().Write();
					sServer->Listen();
					}	// Deliberate fall through.
				case IDCANCEL:
					{
					EndDialog(hDlg, LOWORD(wParam));
					return TRUE;
					}
				case ID_CHECK_WIN_POS:
					{
					int isChecked = IsDlgButtonChecked(hDlg, ID_CHECK_WIN_POS);
					EnableWindow(GetDlgItem(hDlg, ID_EDIT_WIN_POS_X), !isChecked);
					EnableWindow(GetDlgItem(hDlg, ID_EDIT_WIN_POS_Y), !isChecked);
					break;
					}
				}
			break;
		}
    return FALSE;
	}

LRESULT CServer::HandleWindowCommand(UINT aMessage, WPARAM aWParam, LPARAM aLParam)
	{
	int wmId = LOWORD(aWParam); 
	int wmEvent = HIWORD(aWParam); 
	switch (wmId)
		{
		case KServerInitParam:
			Listen();
			break;
		case ID_PREFS:
			DialogBox(iAppHandle, (LPCTSTR)IDD_PREFS_DIALOG, iConsole->Handle(), (DLGPROC)PrefsCallback);
			break;
		default:
			return DefWindowProc(iConsole->Handle(), aMessage, aWParam, aLParam);
		}
	return 0;
	}
