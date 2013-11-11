// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __OSBWIN_H__
#define __OSBWIN_H__

/**
@file
@internalComponent
*/

#include <gdi.h>

struct TWin32Info;
class CPalette;

NONSHARABLE_CLASS(CDebugOsbWin): public CBase
	{
public:
	IMPORT_C static CDebugOsbWin* NewL(const TDesC& aName, TSize aSize);
	IMPORT_C void Refresh(TSize aSize, TDisplayMode aDisplayMode, const TUint32* aDataAddress);
	IMPORT_C ~CDebugOsbWin();
public:	
	static TInt iId;	
private:
	static TInt ThreadMain(TAny* aArg);
	CDebugOsbWin(const TDesC& aName, TSize aSize);
	void ConstructL();
	void Copy2Bpp(const TUint32* aDataAddress);
	void Copy8Bpp(const TUint32* aDataAddress);
	void Copy12Bpp(const TUint32* aDataAddress);
	void Copy16Bpp(const TUint32* aDataAddress);
	void Copy24Bpp(const TUint32* aDataAddress);
	void CopyU24Bpp(const TUint32* aDataAddress);
private:
	RThread iThread;
	TBool iThreadCreated;
	TSize iSize;
	TBuf<32> iName;
	RSemaphore iSem;
	TWin32Info* iWin32;
	TUint8* iBitmapBits;
	CPalette* iPalette;
	TInt iHExtra;
	TInt iVExtra;
	};

#endif


