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

#include <in_sock.h>

#include "CommonFramework.h"
#include "RequestHandler.h"

CConsoleBase* console; // write all your messages to this


// do the example
LOCAL_C void doExampleL()
    {
	// Create the main component 
	rmPartySml = CRmPartySmlS::NewL();
	delete rmPartySml;
	}

CRmPartySmlS* CRmPartySmlS::NewL()
	{
	CRmPartySmlS* self = new(ELeave)CRmPartySmlS();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CRmPartySmlS::ConstructL()
	{
	iRqstHdlr = CRequestHandler::NewL();
	// We now wait for someone to connect to us.
	iRqstHdlr->WaitForRemoteConnectionL();
	}
	
CRmPartySmlS::~CRmPartySmlS()
	{
	delete iRqstHdlr;
	}
	
	
	
