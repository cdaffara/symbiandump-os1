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

#include <e32base.h>
#include <ecom/ecom.h>

#include "ctsysystemstatepluginhandler.h"

CCtsySystemStatePluginHandler* CCtsySystemStatePluginHandler::NewL()
	{	
	CCtsySystemStatePluginHandler* self = NewLC();
    CleanupStack::Pop();
    return self;
	}

CCtsySystemStatePluginHandler* CCtsySystemStatePluginHandler::NewLC()
	{	
	CCtsySystemStatePluginHandler* self = new(ELeave) CCtsySystemStatePluginHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}

void CCtsySystemStatePluginHandler::ConstructL()
    {    
    // get plugin instance
    iSystemStatePluginService = CCtsySystemStatePluginService::NewL(); 
    }
    
CCtsySystemStatePluginHandler::CCtsySystemStatePluginHandler()
	{	
	}
	
CCtsySystemStatePluginHandler::~CCtsySystemStatePluginHandler()
	{ 	
	delete iSystemStatePluginService;	
	REComSession::FinalClose();	
	}

TInt CCtsySystemStatePluginHandler::Connect()
	{
	return iSystemStatePluginService->Connect();
	}
	
void CCtsySystemStatePluginHandler::ActivateRfForEmergencyCall(MCtsySsmPluginCallback* aSsmPluginCallback, 
																TCtsySsmCallbackData& aCallbackData)
	{
	iSystemStatePluginService->ActivateRfForEmergencyCall(aSsmPluginCallback, aCallbackData);
	}
	
void CCtsySystemStatePluginHandler::DeactivateRfAfterEmergencyCall()
	{
	iSystemStatePluginService->DeactivateRfAfterEmergencyCall();
	}
	
void CCtsySystemStatePluginHandler::Close()
	{
	iSystemStatePluginService->Close();
	}
