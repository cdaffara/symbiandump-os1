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
*/

#include <test/tefunit.h> // for ASSERT macros
#include "egltestcommoninisettings.h"

EXPORT_C CEglTestCommonIniSettings* CEglTestCommonIniSettings::NewL()
	{
	CEglTestCommonIniSettings* self = CEglTestCommonIniSettings::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CEglTestCommonIniSettings* CEglTestCommonIniSettings::NewLC()
	{
	CEglTestCommonIniSettings* self = new(ELeave) CEglTestCommonIniSettings;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CEglTestCommonIniSettings::ConstructL()
	{
	iIniData = CIniData::NewL(KConfigFileName);
	}

EXPORT_C CEglTestCommonIniSettings::~CEglTestCommonIniSettings()
	{
	delete iIniData;
	}

EXPORT_C VGImageFormat CEglTestCommonIniSettings::GetVgFormat(const TDesC& aSectioName, const TInt aWhich)
	{
    TBuf<20> bufVgPixelFormatNameKey;
    bufVgPixelFormatNameKey.Format(KKeyFormatX, aWhich);    
    TBuf16<100> bufPixelValue16;
    TPtrC16 ptrPixelValue16(bufPixelValue16);
    if(!iIniData->FindVar(aSectioName,bufVgPixelFormatNameKey,ptrPixelValue16))
        {
        return VG_IMAGE_FORMAT_INVALID;
        }

    _LIT(K565,"VG_sRGB_565");
    _LIT(KX8888,"VG_sXRGB_8888");
    _LIT(KA8888,"VG_sARGB_8888");
    _LIT(KA8888PRE,"VG_sARGB_8888_PRE");
    
    if(!ptrPixelValue16.Compare(K565))
        {
        return VG_sRGB_565;
        }
    if(!ptrPixelValue16.Compare(KX8888))
        {
        return VG_sXRGB_8888;
        }
    if(!ptrPixelValue16.Compare(KA8888))
        {
        return VG_sARGB_8888;
        }
    if(!ptrPixelValue16.Compare(KA8888PRE))
        {
        return VG_sARGB_8888_PRE;
        }   
    
    return VG_IMAGE_FORMAT_INVALID;
	}

EXPORT_C TUidPixelFormat CEglTestCommonIniSettings::GetPixelFormat(const TDesC& aSectioName, const TInt aWhich)
	{
	TBuf<20> bufPixelNameKey;
	bufPixelNameKey.Format(KKeyFormatX, aWhich);	
	TBuf16<100> bufPixelValue16;
	TPtrC16 ptrPixelValue16(bufPixelValue16);
	if(!iIniData->FindVar(aSectioName,bufPixelNameKey,ptrPixelValue16))
		{
		return EUidPixelFormatUnknown;
		}
	
	_LIT(K565,"EUidPixelFormatRGB_565");
	_LIT(KX8888,"EUidPixelFormatXRGB_8888");
	_LIT(KA8888,"EUidPixelFormatARGB_8888");
	_LIT(KA8888PRE,"EUidPixelFormatARGB_8888_PRE");
    _LIT(KA8,"EUidPixelFormatA_8");
	
	if(!ptrPixelValue16.Compare(K565))
		{
		return EUidPixelFormatRGB_565;
		}
	if(!ptrPixelValue16.Compare(KX8888))
		{
		return EUidPixelFormatXRGB_8888;
		}
	if(!ptrPixelValue16.Compare(KA8888))
		{
		return EUidPixelFormatARGB_8888;
		}
	if(!ptrPixelValue16.Compare(KA8888PRE))
		{
		return EUidPixelFormatARGB_8888_PRE;
		}	
    if(!ptrPixelValue16.Compare(KA8))
        {
        return EUidPixelFormatA_8;
        }   
	return EUidPixelFormatUnknown;
	}

EXPORT_C TInt CEglTestCommonIniSettings::GetNumberOfFormats(const TDesC& aSectioName)
	{
	TInt numFormats = 0;
	if(iIniData->FindVar(aSectioName,KKeyNumFormats,numFormats))
		{
		return numFormats;
		}
	return 0;
	}

EXPORT_C TInt CEglTestCommonIniSettings::GetNumberOfIterations(const TDesC& aSectioName)
    {
    _LIT(KKeyNumIterations, "NumIterations"); 
    TInt numIterations = 0;
    if(iIniData->FindVar(aSectioName, KKeyNumIterations, numIterations))
        {
        return numIterations;
        }
    return 0;
    }

EXPORT_C TSize CEglTestCommonIniSettings::GetImageSize(const TDesC& aSectioName)
    {
    _LIT(KKeyBufferWidth, "ImageWidth");
    _LIT(KKeyBufferHeight, "ImageHeight");
    TSize size(0, 0);
    if(iIniData->FindVar(aSectioName, KKeyBufferWidth, size.iWidth) && 
            iIniData->FindVar(aSectioName, KKeyBufferHeight, size.iHeight))
        {
        return size;
        }
    return TSize(0,0);
   }

EXPORT_C TSize CEglTestCommonIniSettings::GetWindowSize(const TDesC& aSectioName)
    {
    _LIT(KKeyWindowWidth, "WindowWidth");
    _LIT(KKeyWindowHeight, "WindowHeight");
    TSize size(0, 0);
    if(iIniData->FindVar(aSectioName, KKeyWindowWidth, size.iWidth) && 
            iIniData->FindVar(aSectioName, KKeyWindowHeight, size.iHeight))
        {
        return size;
        }
	return TSize(0,0);
   }

EXPORT_C TInt CEglTestCommonIniSettings::GetThresholdGPUUsedMemory(const TDesC& aSectioName)
    {
    _LIT(KKeyThresholdGPUUsedMemory, "ThresholdGPUUsedMemory");
    TInt thresholdGPUUsedMemory = 0;
    if(iIniData->FindVar(aSectioName, KKeyThresholdGPUUsedMemory, thresholdGPUUsedMemory))
        {
        return thresholdGPUUsedMemory;
        }
    return 0;
   }

EXPORT_C TInt CEglTestCommonIniSettings::GetThresholdLastIteration(const TDesC& aSectioName)
    {
    _LIT(KKeyThresholdLastIteration, "ThresholdLastIteration");
    TInt thresholdLastIteration = 0;
    if(iIniData->FindVar(aSectioName, KKeyThresholdLastIteration, thresholdLastIteration))
        {
        return thresholdLastIteration;
        }
    return 0;
   }
