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

#include <e32svr.h>
#include <mmf/common/mmfaudio.h>
#include <mmf/server/mmfurl.h>
#include "Ts_cmmfdummytestcontroller.h"

CMMFDummyTestController* CMMFDummyTestController::NewL(void)
	{
	CMMFDummyTestController* self = CMMFDummyTestController::NewLC();
	
	if(0)
		{
		User::Leave(KErrNone);
		}
		
	CleanupStack::Pop();
	return self;
	}

CMMFDummyTestController* CMMFDummyTestController::NewLC(void)
	{
	CMMFDummyTestController* self = new (ELeave) CMMFDummyTestController();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CMMFDummyTestController::CMMFDummyTestController(void):
										iFlag(EFalse)
	{	
	}

CMMFDummyTestController::~CMMFDummyTestController(void)
	{
	}
                    
void CMMFDummyTestController::ConstructL (void)
	{
	if(iFlag)
		{
		User::Leave(KErrNone);
		}
	}

// The method below need to be filled in with the correct responses

void CMMFDummyTestController::AddDataSourceL(MDataSource& /*aDataSource*/)
	{
	if(iFlag)
		{
		User::Leave(KErrNone);
		}
	}

void CMMFDummyTestController::AddDataSinkL(MDataSink& /*aDataSink*/)
	{
	if(iFlag)
		{
		User::Leave(KErrNone);
		}
	}

void CMMFDummyTestController::RemoveDataSourceL(MDataSource& /*aDataSource*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFDummyTestController::RemoveDataSinkL(MDataSink& /*aDataSink*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFDummyTestController::RemoveSourceSinkL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFDummyTestController::ResetL() 
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFDummyTestController::PrimeL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFDummyTestController::PlayL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFDummyTestController::PauseL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFDummyTestController::StopL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

TTimeIntervalMicroSeconds CMMFDummyTestController::PositionL() const
	{
	if(!iFlag)
		{
		User::Leave(KErrExtensionNotSupported);
		}
		
	TTimeIntervalMicroSeconds time(0);
	return time;
	}

void CMMFDummyTestController::SetPositionL(const TTimeIntervalMicroSeconds& /*aPosition*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

TTimeIntervalMicroSeconds CMMFDummyTestController::DurationL() const
	{
	if(!iFlag)
		{
		User::Leave(KErrExtensionNotSupported);
		}
		
	TTimeIntervalMicroSeconds time(0);
	return time;
	}

void CMMFDummyTestController::CustomCommand(TMMFMessage& aMessage)
	{
	aMessage.Complete(KErrExtensionNotSupported);
	}

void CMMFDummyTestController::SetPrioritySettings(const TMMFPrioritySettings& /*aPrioritySettings*/)
	{
	}

void CMMFDummyTestController::GetNumberOfMetaDataEntriesL(TInt& /*aNumberOfEntries*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

CMMFMetaDataEntry* CMMFDummyTestController::GetMetaDataEntryL(TInt aIndex)
	{ 
	if(!iFlag)
		{
		User::Leave(KErrExtensionNotSupported);
		}

	CMMFMetaDataEntry* ptr = reinterpret_cast<CMMFMetaDataEntry*>(aIndex);  // just to ignore warning. Nothing Logical!!!
	return ptr;
	}




// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs


const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfDummyTestControllerUid,	CMMFDummyTestController::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

