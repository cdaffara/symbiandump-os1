// RemoteConsole.cpp
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
#include <commdlg.h>
#include "console_host.h"
#include "RemoteConsole.h"
#include "ConsoleWindow.h"

LPCTSTR KWindowClassName = TEXT("RemoteConsoleWindowClass");
LPCTSTR KRemoteConsoleName = TEXT("Remote Console");
LPCTSTR KRemoteConsoleCommandReaderName = TEXT("Remote Console Command Reader");

bool CRemoteConsole::sWindowClassRegistered = FALSE;
int CRemoteConsole::sNextId = 1;
CRemoteConsole* CRemoteConsole::sFirst = NULL;
CRemoteConsole* CRemoteConsole::sLast = NULL;


CRemoteConsole* CRemoteConsole::Instance(int aId)
	{
	CRemoteConsole* console = sFirst;
	bool found(0);
	while (console && !found)
		{
		if (console->iId == aId)
			{
			found = 1;
			}
		else
			{
			console = console->iNext;
			}
		}
		

	if (found)
		{
		return console;
		}
	return NULL;
	}

CRemoteConsole* CRemoteConsole::New(HINSTANCE aAppHandle, CSocketCommandReader& aCommandReader, LPCTSTR aTitle, int aWidth, int aHeight, TPreferences& aPreferences)
	{
	std::auto_ptr<CRemoteConsole> self(new(EThrow) CRemoteConsole(aCommandReader, aPreferences));
	self->Construct(aAppHandle, aTitle, aWidth, aHeight);
	return self.release();
	}

CRemoteConsole::~CRemoteConsole()
	{
	iConsole->StopCaptureToFile();
	delete iCommandReader;
	delete iKeyEventSocket;
	delete iConsole;
	if (sLast == this)
		{
		ASSERT(iNext == NULL);
		sLast = iPrevious;
		}
	if (sFirst == this)
		{
		sFirst = iNext;
		}
	if (iPrevious)
		{
		iPrevious->iNext = iNext;
		}
	if (iNext)
		{
		iNext->iPrevious = iPrevious;
		}
	}

int CRemoteConsole::Id() const
	{
	return iId;
	}

void CRemoteConsole::AttachKeyEventSocket(CClientSocket& aSocket)
	{
	ASSERT(iKeyEventSocket == NULL);
	iKeyEventSocket = &aSocket;
	iKeyEventSocket->SetClosureObserver(this);
	}

CRemoteConsole::CRemoteConsole(CSocketCommandReader& aCommandReader, TPreferences& aPreferences)
	: iId(sNextId++), iCommandReader(&aCommandReader), iKeyEventSocket(NULL), iNext(NULL), iPrevious(NULL), iClosing(FALSE), iPreferences(aPreferences)
	{
	if (sFirst == NULL)
		{
		ASSERT(sLast == NULL);
		sFirst = this;
		sLast = this;
		}
	else
		{
		sLast->iNext = this;
		iPrevious = sLast;
		sLast = this;
		}
	}

void CRemoteConsole::Construct(HINSTANCE aAppHandle, LPCTSTR aTitle, int aWidth, int aHeight)
	{
	if (!sWindowClassRegistered)
		{
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX); 
		wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wcex.lpfnWndProc	= (WNDPROC)CWindow::HandleMessage;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= aAppHandle;
		wcex.hIcon			= LoadIcon(aAppHandle, (LPCTSTR)IDI_CONSOLE_HOST);
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= NULL;
		wcex.lpszMenuName	= (LPCTSTR)ID_REMOTE_CONSOLE_MENU;
		wcex.lpszClassName	= KWindowClassName;
		wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
		if (RegisterClassEx(&wcex) == 0)
			{
			throw KExceptionWindowClassRegistrationFailed;
			}
		sWindowClassRegistered = TRUE;
		}

	int posX = CW_USEDEFAULT;
	int posY = CW_USEDEFAULT;
	if (!iPreferences.SystemPositionedWindows())
		{
		posX = iPreferences.DefaultWindowPosX();
		posY = iPreferences.DefaultWindowPosY();
		}
	iConsole = CConsoleWindow::New(aAppHandle, KWindowClassName, aTitle, posX, posY, (aWidth == -1) ? iPreferences.DefaultWindowWidth() : aWidth, (aHeight == -1) ? iPreferences.DefaultWindowHeight() : aHeight, iPreferences.NumOverflowLines(), this, this, FALSE);
	iConsole->SetName(KRemoteConsoleName);
	iCommandReader->ChangeHandler(*this);
	iCommandReader->SetName(KRemoteConsoleCommandReaderName);
	iCommandReader->ReadCommand();
	}

void CRemoteConsole::HandleSocketClosure(CSocket& aSocket)
	{
	if (!iClosing)
		{
		iConsole->SetTitle(TEXT("Remote console closed - press any key to close window"));
		iConsole->SetDimmed(TRUE);
		iClosing = TRUE;
		}
	}

void CRemoteConsole::HandleWindowClosure(CWindow&)
	{
	delete this;
	}

LRESULT CRemoteConsole::HandleWindowCommand(UINT aMessage, WPARAM aWParam, LPARAM aLParam)
	{
	switch (LOWORD(aWParam))
		{
		case ID_RC_CAPTURE:
			{
			UINT checkMenuItem = MF_BYCOMMAND | MF_UNCHECKED;
			if (iConsole->IsCapturingToFile())
				{
				iConsole->StopCaptureToFile();
				}
			else
				{
				TCHAR captureFileName[MAX_PATH];
				if (GetCaptureFileName(captureFileName))
					{
					iConsole->CaptureToFile(captureFileName);
					checkMenuItem = MF_BYCOMMAND | MF_CHECKED;
					}
				}
			CheckMenuItem(GetMenu(iConsole->Handle()), ID_RC_CAPTURE, checkMenuItem);
			break;
			}
		case ID_RC_CLOSE:
			{
			DestroyWindow(iConsole->Handle());
			break;
			}
		case ID_EDIT_COPY:
			{
			iConsole->CopyToClipboard();
			break;
			}
		case ID_EDIT_PASTE:
			{
			iConsole->PasteFromClipboard();
			break;
			}
		default:
			{
			return DefWindowProc(iConsole->Handle(), aMessage, aWParam, aLParam);
			}
		}
	return 0;
	}

class TKeyEvent
	{
public:
	TCHAR iChar;
	UINT iModifiers;
	};

void CRemoteConsole::HandleConsoleChar(TCHAR aChar, UINT aModifiers)
	{
	if (iClosing)
		{
		DestroyWindow(iConsole->Handle());
		}
	else if (iKeyEventSocket)
		{
		TKeyEvent keyEvent;
		keyEvent.iChar = aChar;
		keyEvent.iModifiers = aModifiers;
		iKeyEventSocket->Write((char*)&keyEvent, sizeof(TKeyEvent));
		}
	}

void CRemoteConsole::HandleConsoleString(LPCTSTR aString, int aLength)
	{
	if (iClosing)
		{
		DestroyWindow(iConsole->Handle());
		}
	else if (iKeyEventSocket)
		{
		for (int i = 0; i < aLength; ++i)
			{
			HandleConsoleChar(*(aString + i), 0);
			}
		}
	}

void CRemoteConsole::HandleSocketCommand(TPacketHeader::TPacketType aCommand, const char* aPacket, int aPacketLength, CSocketCommandReader& aReader)
	{
	ASSERT(&aReader == iCommandReader);
	switch (aCommand)
		{
		case TPacketHeader::ECommandWrite:
			{
			const int* toWriteLength = new((char*)aPacket) int;
			LPTSTR toWrite = (TCHAR*)(aPacket + sizeof(int));
			iConsole->Write(toWrite, *toWriteLength);
			break;
			}
		case TPacketHeader::ECommandGetCursorPos:
			{
			class TPos
				{
			public:
				int iX;
				int iY;
				};
			TPos pos;
			iConsole->GetCursorPos(pos.iX, pos.iY);
			iCommandReader->SendResponse((char*)&pos, sizeof(TPos));
			break;
			}
		case TPacketHeader::ECommandSetAbsCursorPos:
			{
			const int* x = new((char*)aPacket) int;
			const int* y = new(((char*)aPacket) + sizeof(int)) int;
			iConsole->SetAbsCursorPos(*x, *y);
			break;
			}
		case TPacketHeader::ECommandSetRelCursorPos:
			{
			const int* x = new((char*)aPacket) int;
			const int* y = new(((char*)aPacket) + sizeof(int)) int;
			iConsole->SetRelCursorPos(*x, *y);
			break;
			}
		case TPacketHeader::ECommandSetCursorHeight:
			{
			const int* height = new((char*)aPacket) int;
			iConsole->SetCursorHeight(*height);
			break;
			}
		case TPacketHeader::ECommandGetScreenSize:
			{
			class TSize
				{
			public:
				int iWidth;
				int iHeight;
				};
			TSize size;
			iConsole->GetConsoleSize(size.iWidth, size.iHeight);
			iCommandReader->SendResponse((char*)&size, sizeof(TSize));
			break;
			}
		case TPacketHeader::ECommandSetTitle:
			{
			iConsole->SetTitle((TCHAR*)aPacket);
			break;
			}
		case TPacketHeader::ECommandClearScreen:
			{
			iConsole->ClearScreen();
			break;
			}
		case TPacketHeader::ECommandClearToEndOfLine:
			{
			iConsole->ClearToEndOfLine();
			break;
			}
		default:
			{
			// Unknown command - do nothing.
			}
		}
	iCommandReader->ReadCommand();
	}

void CRemoteConsole::HandleSocketClosure(CSocketCommandReader& aReader)
	{
	HandleSocketClosure(aReader.Socket());
	}

bool CRemoteConsole::GetCaptureFileName(LPTSTR aFileName) const
	{
	GetDefaultCaptureFileName(aFileName);
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = iConsole->Handle();
	ofn.lpstrFile = aFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("All\0*.*\0Text\0*.TXT\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	ofn.lpstrTitle = TEXT("Capture to");
	bool ok = !(GetSaveFileName(&ofn) == 0);
	if (ok)
		{
		if (FileExists(aFileName))
			{
			LPCTSTR constErrorText = TEXT(" already exists. Overwrite it?");
			std::auto_ptr<TCHAR> errorText(new(EThrow) TCHAR[wcslen(aFileName) + wcslen(constErrorText) + 1]);
			wcscpy(errorText.get(), aFileName);
			wcscat(errorText.get(), constErrorText);
			if (MessageBox(iConsole->Handle(), errorText.get(), TEXT("Warning"), MB_OKCANCEL | MB_ICONWARNING) == IDOK)
				{
				if (DeleteFile(aFileName) <= 0)
					{
					throw KExceptionFailedToDeleteExistingCaptureFile;
					}
				}
			else
				{
				ok = FALSE;
				}
			}
		}
	if (ok)
		{
		SetDefaultCaptureFilePath(aFileName);
		}
	return ok;
	}

void CRemoteConsole::GetDefaultCaptureFileName(LPTSTR aFileName) const
	{
	TCHAR formatString[MAX_PATH];
	wcscpy(formatString, iPreferences.DefaultCaptureFilePath());
	TCHAR windowTitle[MAX_PATH];
	iConsole->GetTitle(windowTitle, MAX_PATH);
	if (wcslen(formatString) + wcslen(windowTitle) + 10 > MAX_PATH) // + 10 for the '_%03d.txt\0'.
		{
		wcscpy(formatString, TEXT("c:\\"));
		}
	wcscat(formatString, windowTitle);
	wcscat(formatString, TEXT("_%03d.txt"));
	int i = 1;
	bool uniqueNameFound(FALSE);
	do
		{
		_snwprintf(aFileName, MAX_PATH, formatString, i++);
		if (!FileExists(aFileName))
			{
			uniqueNameFound = TRUE;
			}
		}
		while (!uniqueNameFound);
	}

void CRemoteConsole::SetDefaultCaptureFilePath(LPCTSTR aFileName) const
	{
	int pathLength;
	for (pathLength = wcslen(aFileName) - 1; (pathLength > 0) && (aFileName[pathLength - 1] != TCHAR('\\')); --pathLength);
	TCHAR path[MAX_PATH];
	wcsncpy(path, aFileName, pathLength);
	*(path + pathLength) = CHAR('\0');
	iPreferences.SetDefaultCaptureFilePath(path);
	iPreferences.Write();
	}
