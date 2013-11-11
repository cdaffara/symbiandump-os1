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
#include "Ts_cmmfdummyVideotestcontroller.h"

CMMFDummyVideoTestController* CMMFDummyVideoTestController::NewL(void)
	{
	if(0)
		{
		User::Leave(KErrNone);
		}
		
	CMMFDummyVideoTestController* self = CMMFDummyVideoTestController::NewLC();
	
	CleanupStack::Pop();
	return self;
	}

CMMFDummyVideoTestController* CMMFDummyVideoTestController::NewLC(void)
	{
	CMMFDummyVideoTestController* self = new (ELeave) CMMFDummyVideoTestController();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CMMFDummyVideoTestController::CMMFDummyVideoTestController(void):
										iFlag(EFalse)
	{	
	}

CMMFDummyVideoTestController::~CMMFDummyVideoTestController(void)
	{
	}
                    
void CMMFDummyVideoTestController::ConstructL (void)
	{
	if(iFlag)
		{
		User::Leave(KErrNone);
		}
	}

// The method below need to be filled in with the correct responses

void CMMFDummyVideoTestController::AddDataSourceL(MDataSource& /*aDataSource*/)
	{
	if(iFlag)
		{
		User::Leave(KErrNone);
		}
	}

void CMMFDummyVideoTestController::AddDataSinkL(MDataSink& /*aDataSink*/)
	{
	if(iFlag)
		{
		User::Leave(KErrNone);
		}
	}

void CMMFDummyVideoTestController::RemoveDataSourceL(MDataSource& /*aDataSource*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFDummyVideoTestController::RemoveDataSinkL(MDataSink& /*aDataSink*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFDummyVideoTestController::RemoveSourceSinkL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFDummyVideoTestController::ResetL() 
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFDummyVideoTestController::PrimeL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFDummyVideoTestController::PlayL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFDummyVideoTestController::PauseL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFDummyVideoTestController::StopL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

TTimeIntervalMicroSeconds CMMFDummyVideoTestController::PositionL() const
	{
	if(!iFlag)
		{
		User::Leave(KErrExtensionNotSupported);
		}
		
	TTimeIntervalMicroSeconds time(0);
	return time;
	}

void CMMFDummyVideoTestController::SetPositionL(const TTimeIntervalMicroSeconds& /*aPosition*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

TTimeIntervalMicroSeconds CMMFDummyVideoTestController::DurationL() const
	{
	if(!iFlag)
		{
		User::Leave(KErrExtensionNotSupported);
		}
		
	TTimeIntervalMicroSeconds time(0);
	return time;
	}

void CMMFDummyVideoTestController::CustomCommand(TMMFMessage& aMessage)
	{
	aMessage.Complete(KErrExtensionNotSupported);
	}

void CMMFDummyVideoTestController::SetPrioritySettings(const TMMFPrioritySettings& /*aPrioritySettings*/)
	{
	}

void CMMFDummyVideoTestController::GetNumberOfMetaDataEntriesL(TInt& /*aNumberOfEntries*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

CMMFMetaDataEntry* CMMFDummyVideoTestController::GetMetaDataEntryL(TInt aIndex)
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
		IMPLEMENTATION_PROXY_ENTRY(KMmfDummyVideoTestControllerUid,	CMMFDummyVideoTestController::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

