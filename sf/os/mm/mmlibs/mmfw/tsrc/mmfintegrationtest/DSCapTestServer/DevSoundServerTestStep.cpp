// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <mmf/plugin/mmfhwdeviceimplementationuids.hrh>
#include "DevSoundServerTestStep.h"
#include <e32msgqueue.h>


CSDevSoundTestStep1* CSDevSoundTestStep1::NewL()
	{
	CSDevSoundTestStep1* self = new (ELeave) CSDevSoundTestStep1;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CSDevSoundTestStep1::ConstructL()
	{}


void CSDevSoundTestStep1::InitializeComplete (TInt aError)
	{
	iCallbackArray[EInitComplete] ++;
	iCallbackError = aError;
	}


void CSDevSoundTestStep1::ToneFinished (TInt aError)
	{
	User::RequestComplete(iStatus, KErrNone);
	iCallbackArray[EToneFinished] ++;
	iCallbackError = aError;
	CActiveScheduler::Stop();
	}

void CSDevSoundTestStep1::BufferToBeFilled (CMMFBuffer* /*aBuffer*/)
	{}

void CSDevSoundTestStep1::PlayError (TInt /*aError*/)
	{}

void CSDevSoundTestStep1::BufferToBeEmptied (CMMFBuffer* /*aBuffer*/)
	{}

void CSDevSoundTestStep1::RecordError (TInt /*aError*/)
	{}

void CSDevSoundTestStep1::ConvertError (TInt /*aError*/)
	{}

void CSDevSoundTestStep1::DeviceMessage (TUid, const TDesC8& /*aMsg*/)
	{}
	

void CSDevSoundTestStep1::StartProcessing(TRequestStatus& /*aStatus*/)
	{
		//Method implimentaion has been removed as per the part of DEF139262
	}

TVerdict CSDevSoundTestStep1::EndProcessingAndReturnResult(TDes8& aMessage)
	{
	aMessage.Copy(_L("Done"));
	return EPass;
	}

CSDevSoundTestStep1::~CSDevSoundTestStep1()
	{}


