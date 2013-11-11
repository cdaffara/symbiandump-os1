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

#include <e32svr.h>
#include <mmf/common/mmfaudio.h>
#include <mmf/server/mmfurl.h>
#include "ts_cmmftestterminationcontroller.h"

_LIT(KMMFTestPanic,"TstTermCtrl");

CMMFTestTerminationController* CMMFTestTerminationController::NewL(void)
	{
	CMMFTestTerminationController* self = CMMFTestTerminationController::NewLC();	
	CleanupStack::Pop();
	return self;
	}

CMMFTestTerminationController* CMMFTestTerminationController::NewLC(void)
	{
	CMMFTestTerminationController* self = new (ELeave) CMMFTestTerminationController();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CMMFTestTerminationController::CMMFTestTerminationController(void)
	{	
	}

CMMFTestTerminationController::~CMMFTestTerminationController(void)
	{

	}
                    
void CMMFTestTerminationController::ConstructL (void)
	{
	}

// The method below need to be filled in with the correct responses

void CMMFTestTerminationController::AddDataSourceL(MDataSource& /*aDataSource*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFTestTerminationController::AddDataSinkL(MDataSink& /*aDataSink*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFTestTerminationController::RemoveDataSourceL(MDataSource& /*aDataSource*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFTestTerminationController::RemoveDataSinkL(MDataSink& /*aDataSink*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFTestTerminationController::RemoveSourceSinkL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFTestTerminationController::ResetL() 
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFTestTerminationController::PrimeL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFTestTerminationController::PlayL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFTestTerminationController::PauseL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFTestTerminationController::StopL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

TTimeIntervalMicroSeconds CMMFTestTerminationController::PositionL() const
	{
	User::Leave(KErrExtensionNotSupported);
	
	return 0;	// Just to ignore warnings
	}

void CMMFTestTerminationController::SetPositionL(const TTimeIntervalMicroSeconds& /*aPosition*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

TTimeIntervalMicroSeconds CMMFTestTerminationController::DurationL() const
	{
	User::Leave(KErrExtensionNotSupported);
	
	return 0;	// Just to ignore warnings
	}

void CMMFTestTerminationController::CustomCommand(TMMFMessage& aMessage)
	{
	switch (aMessage.Function())
		{
	case KDummyFunc1:
		User::Panic(KMMFTestPanic,0);
		break;
	default:
		aMessage.Complete(KErrNotSupported);
		break;
		}
	}

void CMMFTestTerminationController::SetPrioritySettings(const TMMFPrioritySettings& /*aPrioritySettings*/)
	{
	}

void CMMFTestTerminationController::GetNumberOfMetaDataEntriesL(TInt& /*aNumberOfEntries*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

CMMFMetaDataEntry* CMMFTestTerminationController::GetMetaDataEntryL(TInt /*aIndex*/)
	{ 
	User::Leave(KErrExtensionNotSupported);
	
	return NULL;	// Just to ignore warnings
	}




// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs


const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KMmfTestTerminationControllerUid,	CMMFTestTerminationController::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

