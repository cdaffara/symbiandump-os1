// Misc.cpp
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

void* operator new(size_t n, TThrow)
	{
	void* cell = ::operator new(n);
	if (!cell)
		{
		throw KExceptionNoMemory;
		}
	return cell;
	}

void operator delete(void* const p, TThrow)
	{
	::operator delete(p);
	}

void* operator new[](size_t n, TThrow)
	{
	void* cell = ::operator new[](n);
	if (!cell)
		{
		throw KExceptionNoMemory;
		}
	return cell;
	}

void operator delete[](void* const p, TThrow)
	{
	::operator delete[](p);
	}

void Panic()
	{
	_asm int 3;
	}

void DebugLog(LPCTSTR aFormat, ...)
	{
	const int KMaxFormatBuf = 256;
	va_list args;
	va_start(args, aFormat);
	TCHAR buf[KMaxFormatBuf];
	int length = _vsnwprintf(buf, KMaxFormatBuf, aFormat, args);
	OutputDebugString(buf);
	OutputDebugString(TEXT("\r\n"));
	}

bool FileExists(LPCTSTR aFileName)
	{
	int att = GetFileAttributes(aFileName);
	return (att > 0);
	}

void ConvertNarrowToWide(const char* aNarrow, int aLength, WCHAR* aWide)
	{
	for (int i = 0; i < aLength; ++i)
		{
		aWide[i] = (unsigned short)aNarrow[i];
		}
	aWide[aLength] = TCHAR('\0');
	}

void ConvertNarrowToWideAlloc(const char* aNarrow, int aLength, WCHAR*& aWide)
	{
	aWide = new WCHAR[aLength + 1];
	if (aWide)
		{
		ConvertNarrowToWide(aNarrow, aLength, aWide);
		}
	}

void ConvertWideToNarrow(const WCHAR* aWide, int aLength, char* aNarrow)
	{
	for (int i = 0; i < aLength; ++i)
		{
		aNarrow[i] = (char)aWide[i];
		}
	aNarrow[aLength] = '\0';
	}

void ConvertWideToNarrowAlloc(const WCHAR* aWide, int aLength, char*& aNarrow)
	{
	aNarrow = new char[aLength + 1];
	if (aNarrow)
		{
		ConvertWideToNarrow(aWide, aLength, aNarrow);
		}
	}
