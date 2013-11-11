// Misc.h
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
#ifndef __MISC_H__
#define __MISC_H__

#define UNICODE

#include <memory>
#include "stdafx.h"

enum TException
	{
	KExceptionNoMemory,
	KExceptionWindowClassRegistrationFailed,
	KExceptionWindowConstructFailed,
	KExceptionConsoleWindowConstructFailed,
	KExceptionWinSockStartupFailed,
	KExceptionWinSockVersionInvalid,
	KExceptionSocketConstructFailed,
	KExceptionSocketBindFailed,
	KExceptionSocketSelectFailed,
	KExceptionSocketListenFailed,
	KExceptionSocketAcceptFailed,
	KExceptionSocketReadFailed,
	KExceptionInvalidClientVersion,
	KExceptionFailedToFindPrefsFileName,
	KExceptionFailedToReadPrefsFile,
	KExceptionFailedToWritePrefsFile,
	KExceptionFailedToCreateCaptureFile,
	KExceptionFailedToWriteToCaptureFile,
	KExceptionFailedToDeleteExistingCaptureFile,
	KExceptionUnknown
	};

enum TThrow { EThrow = 0 };
void* operator new(size_t n, TThrow);
void operator delete(void* const p, TThrow);

void* operator new[](size_t, TThrow);
void operator delete[](void* const p, TThrow);

#ifdef _DEBUG
#define ASSERT(x) if (!(x)) Panic()
#else
#define ASSERT(x)
#endif

void Panic();
void DebugLog(LPCTSTR aFormat, ...);
bool FileExists(LPCTSTR aFileName);
void ConvertNarrowToWide(const char* aNarrow, int aLength, WCHAR* aWide);
void ConvertNarrowToWideAlloc(const char* aNarrow, int aLength, WCHAR*& aWide);
void ConvertWideToNarrow(const WCHAR* aWide, int aLength, char* aNarrow);
void ConvertWideToNarrowAlloc(const WCHAR* aWide, int aLength, char*& aNarrow);

#endif
