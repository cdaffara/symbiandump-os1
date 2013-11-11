// Preferences.cpp
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
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include "stdafx.h"
#include "shlwapi.h"
#include "Preferences.h"

const int KMaxFileNameLength = MAX_PATH;
LPCTSTR KPrefsFileName = TEXT("\\console_host.ini");
static bool sPrefsModified = FALSE;


TPreferences::TPreferences()
	: iServerPort(8086), iDefaultWindowWidth(80), iDefaultWindowHeight(40), iDefaultWindowPosX(0), iDefaultWindowPosY(0), iSystemPositionedWindows(TRUE), iNumOverflowLines(100)
	{
	wcscpy(iDefaultCaptureFilePath, TEXT("c:\\"));
	}

void PrefsFileName(LPTSTR aName)
	{
	if (GetModuleFileName(NULL, aName, KMaxFileNameLength) == 0)
		{
		throw KExceptionFailedToFindPrefsFileName;
		}
	if (PathRemoveFileSpec(aName) == 0)
		{
		throw KExceptionFailedToFindPrefsFileName;
		}
	if (wcscat(aName, KPrefsFileName) == 0)
		{
		throw KExceptionFailedToFindPrefsFileName;
		}
	}

void TPreferences::Read()
	{
	TCHAR fileName[MAX_PATH];
	PrefsFileName(fileName);
	int file = _wopen(fileName, _O_RDWR | _O_BINARY);
	if (file != -1)
		{
		int err = 0;
		struct _stat fileStat;
		if (_fstat(file, &fileStat) == 0)
			{
			if (_read(file, this, fileStat.st_size) < fileStat.st_size)
				{
				err = 1;
				}
			}
		_close(file);
		if (err)
			{
			throw KExceptionFailedToReadPrefsFile;
			}
		}
	}

void TPreferences::Write() const
	{
	if (sPrefsModified)
		{
		int err = 0;
		TCHAR fileName[MAX_PATH];
		PrefsFileName(fileName);
		int file = _wopen(fileName, _O_CREAT | _O_WRONLY | _O_BINARY, _S_IWRITE);
		if (file != -1)
			{
			int err = 0;
			if (_write(file, this, sizeof(TPreferences)) < sizeof(TPreferences))
				{
				err = 1;
				}
			_close(file);
			}
		else
			{
			err = 1;
			}
		if (err)
			{
			_wremove(fileName);
			throw KExceptionFailedToWritePrefsFile;
			}
		else
			{
			sPrefsModified = FALSE;
			}
		}
	}

unsigned short TPreferences::ServerPort() const
	{
	return iServerPort;
	}

unsigned int TPreferences::DefaultWindowWidth() const
	{
	return iDefaultWindowWidth;
	}

unsigned int TPreferences::DefaultWindowHeight() const
	{
	return iDefaultWindowHeight;
	}

unsigned int TPreferences::DefaultWindowPosX() const
	{
	return iDefaultWindowPosX;
	}

unsigned int TPreferences::DefaultWindowPosY() const
	{
	return iDefaultWindowPosY;
	}

int TPreferences::SystemPositionedWindows() const
	{
	return iSystemPositionedWindows;
	}

unsigned int TPreferences::NumOverflowLines() const
	{
	return iNumOverflowLines;
	}

LPCTSTR TPreferences::DefaultCaptureFilePath() const
	{
	return iDefaultCaptureFilePath;
	}

void TPreferences::SetServerPort(unsigned short aPort)
	{
	iServerPort = aPort;
	sPrefsModified = TRUE;
	}

void TPreferences::SetDefaultWindowWidth(unsigned int aWidth)
	{
	iDefaultWindowWidth = aWidth;
	sPrefsModified = TRUE;
	}

void TPreferences::SetDefaultWindowHeight(unsigned int aHeight)
	{
	iDefaultWindowHeight = aHeight;
	sPrefsModified = TRUE;
	}

void TPreferences::SetDefaultWindowPosX(unsigned int aPosX)
	{
	iDefaultWindowPosX = aPosX;
	sPrefsModified = TRUE;
	}

void TPreferences::SetDefaultWindowPosY(unsigned int aPosY)
	{
	iDefaultWindowPosY = aPosY;
	sPrefsModified = TRUE;
	}

void TPreferences::SetSystemPositionedWindows(int aSystemPositionedWindows)
	{
	iSystemPositionedWindows = aSystemPositionedWindows;
	sPrefsModified = TRUE;
	}

void TPreferences::SetNumOverflowLines(unsigned int aNumOverflowLines)
	{
	iNumOverflowLines = aNumOverflowLines;
	sPrefsModified = TRUE;
	}

void TPreferences::SetDefaultCaptureFilePath(LPCTSTR aPath)
	{
	wcscpy(iDefaultCaptureFilePath, aPath);
	sPrefsModified = TRUE;
	}
