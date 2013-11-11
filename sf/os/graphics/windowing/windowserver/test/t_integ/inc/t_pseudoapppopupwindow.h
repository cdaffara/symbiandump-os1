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

/**
 @file
 @test
 @internalComponent
*/

#ifndef __T_PSEUDOAPPPOPUPWINDOW_H__
#define __T_PSEUDOAPPPOPUPWINDOW_H__

_LIT(KScreenWindowPopup,     "screen%d_window_popup_");
_LIT(KTPopupWindowAppPanic, "T_PseudoAppPopupWindow");

/**
Standard prototypes
*/
GLDEF_C TInt E32Main(void);

LOCAL_C void DisplayPopupWindowL();
LOCAL_C void ReadPopupConfig(TSize& aWindowSize, TPoint& aWindowPosition, TPtrC& aBitmapFile, TInt& aTransparency,
                             TInt& aScreenNo, TDisplayMode& aMode, CIniData* aUtils);
LOCAL_C void DeterminePixelValues(TSize& aWindowSize, TPoint& aWindowPosition, 	CWsScreenDevice& aScreenDevice);

#endif //__T_PSEUDOAPPPOPUPWINDOW_H__
