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

#include <e32base.h>
#include "tsi_mvs_plugininfo.h"

/**
 *
 * RTestStepMVSPluginInfoPos
 *
 */
RTestStepMVSPluginInfoPos::RTestStepMVSPluginInfoPos(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid aExpectedmediaType, const TUid aControllerUid)
    : RTestStepMVSPluginInfo(aTestName, aSectName, aKeyName, aExpectedmediaType, aControllerUid)
    {
    }

/**
 * DoTestStepL
 */
TVerdict RTestStepMVSPluginInfoPos::DoTestStepL()
    {
    return( PerformTestL() );
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSPluginInfoPos::PerformTestL()
    {
    INFO_PRINTF1(_L("Positive Testing for MVS Plug-in Info Classes"));
    iTestStepResult = EFail;
    TInt counter=0;
    TInt err = KErrNone;
    TBuf<80> tmpBuf;
    TUid tmpUid = KNullUid;
    
    // Get the plugin names
    INFO_PRINTF1(_L("iMVSControllerPluginInfo->GetPluginListL()"));
    err = iMVSControllerPluginInfo->GetPluginListL(iPluginArray, iUidArray );
    INFO_PRINTF2(_L("iMVSControllerPluginInfo->GetPluginListL() returned %d Plug-ins"), err);    
    
    // Print the Plugin Array
    for(counter = 0; counter < iPluginArray->Count(); ++counter)
        {
        tmpBuf = iPluginArray->operator[](counter);
        INFO_PRINTF4(_L("Controller#%d = < %S > <0x%8x>"), counter, &tmpBuf, iUidArray.operator[](counter));
        }

    // Get the extensions supported by the Controller UID given as Input via the suite.
    INFO_PRINTF1(_L("iMVSControllerPluginInfo->GetExtensionListL()"));
    err = iMVSControllerPluginInfo->GetExtensionListL(iMediaType, iExtArray);
    INFO_PRINTF2(_L("iMVSControllerPluginInfo->GetExtensionListL() returned %d extensions"), err);
    
    // Print the Extension Array
    for(counter = 0; counter < iExtArray->Count(); ++counter)
        {
        tmpBuf = iExtArray->operator[](counter);
        INFO_PRINTF2(_L("<< %S >>"), &tmpBuf);
        }
    
    // Get the mediaType of a file (Audio / Video)
    INFO_PRINTF2(_L("iMVSControllerPluginInfo->GetMediaTypeL(%S)"), &iFilename);
    TUid mediaTypeUid = iMVSControllerPluginInfo->GetMediaTypeL(iFilename);
    if (mediaTypeUid == KUidMediaTypeAudio)
        {
        INFO_PRINTF1(_L(">>>Media Type is Audio"));
        }
    else if (mediaTypeUid == KUidMediaTypeVideo)
        {
        INFO_PRINTF1(_L(">>>Media Type is Video"));
        }
    else
        {
        INFO_PRINTF1(_L(">>>Media Type is Un-recognised"));
        }
    if (mediaTypeUid != iExpectedMediaType)
        {
        return EFail;
        }
        
    return EPass;
    }

/**
 *
 * RTestStepMVSPluginInfoNeg
 *
 */
RTestStepMVSPluginInfoNeg::RTestStepMVSPluginInfoNeg(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName, const TUid aExpectedmediaType, const TUid aControllerUid)
    : RTestStepMVSPluginInfo(aTestName, aSectName, aKeyName, aExpectedmediaType, aControllerUid)
    {
    }

/**
 * DoTestStepL
 */
TVerdict RTestStepMVSPluginInfoNeg::DoTestStepL()
    {
    return( PerformTestL() );
    }

/**
 *
 * PerformTestL
 *
 */
TVerdict RTestStepMVSPluginInfoNeg::PerformTestL()
    {
    INFO_PRINTF1(_L("Negative Testing for MVS Plug-in Info Classes"));
    iTestStepResult = EFail;
    TInt counter = 0;
    TInt err = KErrNone;
    TBuf<80> tmpBuf;
    TInt listCount = 0;
    
    // Get the plugin names
    INFO_PRINTF1(_L("iMVSControllerPluginInfo->GetPluginListL()"));
    err = iMVSControllerPluginInfo->GetPluginListL(iPluginArray, iUidArray);
    INFO_PRINTF2(_L("iMVSControllerPluginInfo->GetPluginListL() returned %d Plug-ins"), err);
    
    // Get the extensions supported by the chosen plugin
    INFO_PRINTF1(_L("iMVSControllerPluginInfo->GetExtensionListL()"));
    TRAP(err, listCount = iMVSControllerPluginInfo->GetExtensionListL(iMediaType, iExtArray));
    INFO_PRINTF2(_L("iMVSControllerPluginInfo->GetExtensionListL() returned %d extensions"), listCount);
    
    // Print the Extension Array
    for(counter = 0; counter < iExtArray->Count(); ++counter)
        {
        tmpBuf = iExtArray->operator[](counter);
        INFO_PRINTF2(_L("<< %S >>"), &tmpBuf);
        }

    // Get the mediaType of a file (Audio / Video)
    INFO_PRINTF2(_L("iMVSControllerPluginInfo->GetMediaTypeL(%S)"), &iFilename);
    TUid mediaTypeUid=TUid::Null();
    TRAP(err,mediaTypeUid=iMVSControllerPluginInfo->GetMediaTypeL(iFilename));
    if(err==KErrNotFound)
        {
        INFO_PRINTF1(_L(">>>Media Type is Un-recognised"));
        }
    else{
		if (mediaTypeUid == KUidMediaTypeAudio)
		    {
		    INFO_PRINTF1(_L(">>>Media Type is Audio"));
		    }
		else if (mediaTypeUid == KUidMediaTypeVideo)
		    {
		    INFO_PRINTF1(_L(">>>Media Type is Video"));
		    }
		else
		    {
		    INFO_PRINTF1(_L(">>>Media Type is Un-recognised"));
		    }
    	}
    
    if (mediaTypeUid != iExpectedMediaType)
        {
        return EFail;
        }

    return EPass;
    }

