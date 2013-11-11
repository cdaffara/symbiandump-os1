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

#include "amastart.h"
#include "loadamastart.h"

/**
Creates an instance of CLoadAmaStart class.

@return An instance of CLoadAmaStart.
*/
EXPORT_C CLoadAmaStart* CLoadAmaStart::CreateL()
	{
	CLoadAmaStart* self = new (ELeave) CLoadAmaStart();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	
	}

/**
Constructs the object. 
*/
void CLoadAmaStart::ConstructL()
	{
	iAmaStart = CAmaStart::NewL();
	}

/**
Default constructor.
*/
CLoadAmaStart::CLoadAmaStart()
	{
	}
/**
Launch AMAs (after market application) from a DSC (Synchronous).
Returns when processing of the DSC is complete. 

@param aDscId Id of the DSC containing AMAs to be started.
@see CAmaStart
*/
void CLoadAmaStart::StartL(const TUid aDscId)
	{
	iAmaStart->StartL(aDscId);
	}

/**
Launch AMAs (after market application) from a DSC.(Asynchronous)
Returns immediately. The supplied TRequestStatus object is used to signal completion. 
When the request is completed.

@param aDscId Id of the DSC containing AMAs to be started.
@param aRequestStatus Status object to signal completion.
@see CAmaStart
*/
void CLoadAmaStart::Start(const TUid aDscId, TRequestStatus& aRequestStatus)
	{
	iAmaStart->Start(aDscId, aRequestStatus);
	}

/**
Cancel's a pending asynchronous Start request.
If there is no request pending, calling this method has no effect.
@see CAmaStart
*/
void CLoadAmaStart::StartCancel()
	{
	iAmaStart->CancelStart();
	}

/**
Destructor
*/
CLoadAmaStart::~CLoadAmaStart()
	{
	if(iAmaStart)
		{
		iAmaStart->CancelStart();
		}
	delete iAmaStart;
	}
