// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __FBSHELPER_H__
#define __FBSHELPER_H__

#include <e32base.h>
#include <fbs.h>


class CFbsRasterizer;

/**
@internalComponent
*/
NONSHARABLE_CLASS(CFbsSessionHelper) : public CActive
	{
public:
	CFbsSessionHelper(RFbsSession& aFbs);
	~CFbsSessionHelper();
	TInt AddBitmap(CFbsBitmap& aBitmap);
	void RemoveBitmap(CFbsBitmap& aBitmap);
	inline CFbsRasterizer* Rasterizer() const;

private:
	void RunL();
	void DoCancel();

private:
	RFbsSession& iFbs;
	RPointerArray<CFbsBitmap> iBitmaps;
#ifdef __WINS__
	RLibrary iRasterizerLib;
#endif
	CFbsRasterizer* iRasterizer;

public:
#ifdef SYMBIAN_DEBUG_FBS_LOCKHEAP
	RMutex iDebugMutex;
#endif
	HBufC8* iExtraBuffer;
	TInt iServerSessionHandle;
	};


inline CFbsRasterizer* CFbsSessionHelper::Rasterizer() const
	{
	return iRasterizer;
	}


#endif // __FBSHELPER_H__
