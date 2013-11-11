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

#include <bluetooth/hci/commandbaseextension.h>


/**
Return a newly constucted CHCICommandBaseExtension
*/
CHCICommandBaseExtension* CHCICommandBaseExtension::NewL()
	{
	CHCICommandBaseExtension* self = new(ELeave) CHCICommandBaseExtension();
	return(self);
	}


/**
Do nothing
*/
CHCICommandBaseExtension::CHCICommandBaseExtension()
	{
	}


/**
Clean up
*/
CHCICommandBaseExtension::~CHCICommandBaseExtension()
	{
	delete iCompletingEventQueryHelper;
	}


/**
Return CHCICompletingEventQueryHelper
*/
CHCICompletingEventQueryHelper* CHCICommandBaseExtension::CompletingEventQueryHelper()
	{
	return iCompletingEventQueryHelper;
	}


/**
Method to allow control of the iExpectCompletingEvent
*/
void CHCICommandBaseExtension::SetCompletingEventQueryHelper(CHCICompletingEventQueryHelper* aCompletingEventQueryHelper)
	{
	delete iCompletingEventQueryHelper;
	iCompletingEventQueryHelper = aCompletingEventQueryHelper;
	}
	
	
	
