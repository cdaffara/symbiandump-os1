// Preferences.h
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

#ifndef __PREFERENCES_H__
#define __PREFERENCES_H__


class TPreferences
	{
public:
	TPreferences();
	void Read();
	void Write() const;
	unsigned short ServerPort() const;
	unsigned int DefaultWindowWidth() const;
	unsigned int DefaultWindowHeight() const;
	unsigned int DefaultWindowPosX() const;
	unsigned int DefaultWindowPosY() const;
	int SystemPositionedWindows() const;
	unsigned int NumOverflowLines() const;
	LPCTSTR DefaultCaptureFilePath() const;
	void SetServerPort(unsigned short aPort);
	void SetDefaultWindowWidth(unsigned int aWidth);
	void SetDefaultWindowHeight(unsigned int aHeight);
	void SetDefaultWindowPosX(unsigned int aPosX);
	void SetDefaultWindowPosY(unsigned int aPosY);
	void SetSystemPositionedWindows(int aSystemPositionedWindows);
	void SetNumOverflowLines(unsigned int aNumOverflowLines);
	void SetDefaultCaptureFilePath(LPCTSTR aPath);
private:
	unsigned short iServerPort;
	unsigned int iDefaultWindowWidth;
	unsigned int iDefaultWindowHeight;
	unsigned int iDefaultWindowPosX;
	unsigned int iDefaultWindowPosY;
	int iSystemPositionedWindows;
	unsigned int iNumOverflowLines;
	TCHAR iDefaultCaptureFilePath[MAX_PATH];
	};

#endif // __PREFERENCES_H__
