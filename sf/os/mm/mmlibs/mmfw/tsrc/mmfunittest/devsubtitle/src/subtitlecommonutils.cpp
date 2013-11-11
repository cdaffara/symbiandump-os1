// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
 @prototype
*/

#include "subtitlecommonutils.h"

_LIT(KFileType, "image/png");

void SubtitleCommonUtils::Png2BmpL(RFs& aFs, const TDesC& aSourcePngFilename, CFbsBitmap& aDestBitmap)
    {
    TBuf8<255> imageType;
    imageType.Append(KFileType);
	
	CImageDecoder* imageDecoder = CImageDecoder::FileNewL(
    	    aFs,
    	    aSourcePngFilename,
    	    imageType
    	    );
    	    
    CleanupStack::PushL(imageDecoder);
    	    		           
    CMsvOperationActiveSchedulerWait* waiter=CMsvOperationActiveSchedulerWait::NewLC();

	imageDecoder->Convert(&waiter->iStatus, aDestBitmap);
	
	waiter->Start();
	
	User::LeaveIfError(waiter->iStatus.Int());
		
	CleanupStack::PopAndDestroy(waiter);
	CleanupStack::PopAndDestroy(imageDecoder);
    }