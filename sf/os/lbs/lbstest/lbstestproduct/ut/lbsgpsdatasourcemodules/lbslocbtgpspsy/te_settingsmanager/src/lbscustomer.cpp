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
// CLBSCustomer implementation
//



/**
 @file lbscustomer.cpp
 @internalTechnology
*/

#include "lbscustomer.h"

//BTGPSPSY PSY UID
const TUid KBtGpsPsyImplUid = 
    {
    0x101FE999
    };

CLBSCustomer::CLBSCustomer()
	{
	iBuildPosServer = EFalse;
	iBuildPositioner = EFalse;
	}

CLBSCustomer::~CLBSCustomer ()
	{
	if(iBuildPositioner)
		{
		CleanupStack::PopAndDestroy(&iPositioner);
		}

	if(iBuildPosServer)
		{
		CleanupStack::PopAndDestroy(&iPosServer);
		}
	}

CLBSCustomer* CLBSCustomer::NewLC ()
	{
	CLBSCustomer* self = new (ELeave)CLBSCustomer();
	CleanupStack::PushL (self);
	return self;
	}

CLBSCustomer* CLBSCustomer::NewL ()
	{
	CLBSCustomer* self=CLBSCustomer::NewLC ();
	CleanupStack::Pop(self);
	return self;
	}

// connect to server
TInt CLBSCustomer::ConnectToPositionServer()
	{
	TInt ret = iPosServer.Connect();
	if (ret != KErrAlreadyExists && ret != KErrNone)
		{
		CleanupClosePushL(iPosServer);
		iBuildPosServer = ETrue;
		}
	return ret;
	}

// open positioner
TInt CLBSCustomer::OpenBTPSYPositioner()
	{   
	TInt ret = iPositioner.Open(iPosServer, KBtGpsPsyImplUid);
	if (ret != KErrAlreadyExists && ret != KErrNone)
		{
		CleanupClosePushL(iPositioner);
		iBuildPositioner = ETrue;
		}
	return ret;
	}
