// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include <smspver.h>
#include "Te_SmsTestSteps.h"

CCloseSMSProtocol::CCloseSMSProtocol(RSocketServ &aSocketServer)
/** 
	Each test step initialises it's own name
	@param aSocketServer is the session handle
	@param aSocket is the sub-session handle
*/
	{
    iSharedSocketServer = &aSocketServer;
	}

/**
	Close the socket sub-session and socket server session 
*/
TVerdict CCloseSMSProtocol::doTestStepL()
	{	
	//Close session
    if( iSharedSocketServer != NULL )
        {
        iSharedSocketServer->Close(); 
        }

#ifdef _DEBUG	
	TInt err = RProperty::Delete(KUidPSSMSStackCategory, KUidPSSMSStackFreeDiskSpaceKey);
	if (err != KErrNone && err != KErrNotFound)
        {
        ERR_PRINTF2(_L("RProperty::Delete() failure [err=%d]"), err);
        }	
#endif
	
	return TestStepResult();
	}
