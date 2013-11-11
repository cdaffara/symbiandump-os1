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

#include "cmockmsghandler.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmockmsgrouter.h"
#include "cmockltsyengine.h"

/**
Factory function
*/
CMockMsgHandler* CMockMsgHandler::NewL(CMmMessageRouter* aMessageRouter)
	{
	CMockMsgHandler* self = new (ELeave)CMockMsgHandler(aMessageRouter);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
/**
Constructor
*/	
CMockMsgHandler::CMockMsgHandler(CMmMessageRouter* aMessageRouter)
	: iMessageRouter(aMessageRouter)
	{
	
	}
	
/**
2nd phase construction
*/	
void CMockMsgHandler::ConstructL()
	{
	
	}

/**
Destructor
*/	
CMockMsgHandler::~CMockMsgHandler()
	{
	
	}

/**
ExtFuncL
         
@param aIpc
@param aDataPackage
@return Error value
*/
TInt CMockMsgHandler::ExtFuncL( TInt aIpc, const CMmDataPackage* aDataPackage)
	{
	return iMessageRouter->MockLtsyEngine()->HandleMessage(aIpc,aDataPackage);
	}
	
	
	
