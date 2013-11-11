/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/
 

#include "NGAPostProcNotifier.h"
#include "NGAPostProcSessionManager.h"
#include "MdfRDebug.h"

CNGAPostProcNotifier::CNGAPostProcNotifier( CNGAPostProcSessionManager& aParent )
:CActive(CActive::EPriorityStandard),iParent(aParent)
{
	CActiveScheduler::Add(this);
}

CNGAPostProcNotifier::~CNGAPostProcNotifier()
{
	Cancel();
}

CNGAPostProcNotifier* CNGAPostProcNotifier::NewL( CNGAPostProcSessionManager& aParent )
{
	CNGAPostProcNotifier* self = new (ELeave)CNGAPostProcNotifier(aParent);
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
}

void CNGAPostProcNotifier::ConstructL()
{
	// No Impl
}

void CNGAPostProcNotifier::SetBufferId(TInt aBufId)
{
	iBufferId = aBufId;
}

void CNGAPostProcNotifier::Activate()
{
	PP_DEBUG(_L("CNGAPostProcNotifier:Activate ++"));
	if(!IsActive())
	{
		PP_DEBUG(_L("CNGAPostProcNotifier:Activate SetActive"));
		SetActive();
	}
	PP_DEBUG(_L("CNGAPostProcNotifier:Activate --"));
}

void CNGAPostProcNotifier::RunL()
{
	PP_DEBUG(_L("CNGAPostProcNotifier:RunL ++"));
	//KErrCancel should never get invoked Since cancellation handled syncronously under AO's
	//DoCancel.
	iParent.HandleBufferRelease(iBufferId, iStatus.Int());

	PP_DEBUG(_L("CNGAPostProcNotifier:RunL --"));
}

void CNGAPostProcNotifier::DoCancel()
{
    iParent.CancelUpdate();
}

TInt CNGAPostProcNotifier::RunError( TInt /*aError*/ )
{
	PP_DEBUG(_L("CNGAPostProcNotifier[%x]:CNGAPostProcNotifier::RunError "), this);
	return KErrNone;
}
