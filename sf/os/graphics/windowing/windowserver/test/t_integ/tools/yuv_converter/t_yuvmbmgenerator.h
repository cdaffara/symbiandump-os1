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

#if !defined __TMBMYUVGENERATOR_H__
#define __TMBMYUVGENERATOR_H__

#include <w32std.h>		//CWindowGc, CWsScreenDevice

_LIT(KYuvFrame,"c:\\graphics\\yuvFrame");
_LIT(KMultiRgbFrame,"z:\\graphics\\multi_clock_frame.mbm");
_LIT(KMultiYuvFrame,"c:\\graphics\\multi_clock_frame_yuv422.mbm");
_LIT(KEmpty, "");
_LIT(KBmpSuffix, ".bmp");
_LIT(KTMbmYuvGeneratorAppPanic, "T_MbmYuvGenerator");

const TInt KScreenNo = 0;
const TInt KMultiBitmapFileSize = 100;
const TInt KFixedNumWidth = 3;

GLDEF_C TInt E32Main();
LOCAL_C void StartGeneratorL();
LOCAL_C void ConvertToYuv422(CFbsBitmap& bitmapSource);

#endif //__TMBMYUVGENERATOR_H__
