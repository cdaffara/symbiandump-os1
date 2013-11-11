// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SCDRAW_H__
#define __SCDRAW_H__

#ifndef __E32SVR_H__
#include <e32svr.h>
#endif
#include "BMDRAW.H"

/**
Change the following two defines if the hardware display mode is changed
@internalComponent
*/
#define SCREEN_LAYOUT_DISPLAY_MODE EColor256
#define SCREEN_LAYOUT_BASE_BITMAP CDrawEightBppBitmapColor

/**
@internalComponent
*/
class CDrawScreenBitmap : public SCREEN_LAYOUT_BASE_BITMAP
	{
public:
	virtual TInt ConstructScreen(TInt aScreenNo, TAny* aBitmapAddress,TSize aSize);
	virtual void SetDisplayMode(CFbsDrawDevice* aDrawDevice);
	virtual void OrientationsAvailable(TBool aOrientation[4]);
	virtual TBool SetOrientation(TOrientation aOrientation);
	virtual TInt HorzTwipsPerThousandPixels() const;
	virtual TInt VertTwipsPerThousandPixels() const;
private:
    TInt iScreenNo;
	};

#endif

