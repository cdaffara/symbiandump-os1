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

#include <e32svr.h>
#include <mmf/common/mmfaudio.h>
#include <mmf/server/mmfurl.h>
#include "ts_cmmftestconstructcontroller.h"


//
// Test Constructor1
CMMFTestConstructController* CMMFTestConstructController::NewL(void)
	{
	CMMFTestConstructController* self = CMMFTestConstructController::NewLC();	
	CleanupStack::Pop();
	return self;
	}

CMMFTestConstructController* CMMFTestConstructController::NewLC(void)
	{
	CMMFTestConstructController* self = new (ELeave) CMMFTestConstructController();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CMMFTestConstructController::CMMFTestConstructController(void)
	{	
	}

CMMFTestConstructController::~CMMFTestConstructController(void)
	{

	}
                    
void CMMFTestConstructController::ConstructL (void)
	{
	}

// The method below need to be filled in with the correct responses

void CMMFTestConstructController::AddDataSourceL(MDataSource& /*aDataSource*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFTestConstructController::AddDataSinkL(MDataSink& /*aDataSink*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFTestConstructController::RemoveDataSourceL(MDataSource& /*aDataSource*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFTestConstructController::RemoveDataSinkL(MDataSink& /*aDataSink*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFTestConstructController::RemoveSourceSinkL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFTestConstructController::ResetL() 
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFTestConstructController::PrimeL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFTestConstructController::PlayL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFTestConstructController::PauseL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

void CMMFTestConstructController::StopL()
	{
	User::Leave(KErrExtensionNotSupported);
	}

TTimeIntervalMicroSeconds CMMFTestConstructController::PositionL() const
	{
	User::Leave(KErrExtensionNotSupported);
	
	return 0;	// Just to ignore warnings
	}

void CMMFTestConstructController::SetPositionL(const TTimeIntervalMicroSeconds& /*aPosition*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

TTimeIntervalMicroSeconds CMMFTestConstructController::DurationL() const
	{
	User::Leave(KErrExtensionNotSupported);
	
	return 0;	// Just to ignore warnings
	}

void CMMFTestConstructController::CustomCommand(TMMFMessage& aMessage)
	{
	aMessage.Complete(KErrExtensionNotSupported);
	}

void CMMFTestConstructController::SetPrioritySettings(const TMMFPrioritySettings& /*aPrioritySettings*/)
	{
	}

void CMMFTestConstructController::GetNumberOfMetaDataEntriesL(TInt& /*aNumberOfEntries*/)
	{
	User::Leave(KErrExtensionNotSupported);
	}

CMMFMetaDataEntry* CMMFTestConstructController::GetMetaDataEntryL(TInt /*aIndex*/)
	{ 
	User::Leave(KErrExtensionNotSupported);
	
	return NULL;	// Just to ignore warnings
	}

//
// Test Constructor2
CMMFTestConstructController2* CMMFTestConstructController2::NewL(void)
	{
	CMMFTestConstructController2* self = CMMFTestConstructController2::NewLC();	
	CleanupStack::Pop();
	return self;
	}

CMMFTestConstructController2* CMMFTestConstructController2::NewLC(void)
	{
	CMMFTestConstructController2* self = new (ELeave) CMMFTestConstructController2();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CMMFTestConstructController2::CMMFTestConstructController2(void)
	: CMMFTestConstructController()
	{	
	}

CMMFTestConstructController2::~CMMFTestConstructController2(void)
	{

	}
                    
void CMMFTestConstructController2::ConstructL (void)
	{
	CMMFTestConstructController::ConstructL();
	TBool sdrmmode = IsSecureDrmModeL();
	}

//
// Test Constructor3
CMMFTestConstructController3::CDummyMMFObject* CMMFTestConstructController3::CDummyMMFObject::NewL()
	{
	CDummyMMFObject* self = new (ELeave) CDummyMMFObject();
	return self;
	}

CMMFTestConstructController3::CDummyMMFObject::~CDummyMMFObject()
	{
	}

const TUid KDummyMMFObjectUid = {0x12345678};
CMMFTestConstructController3::CDummyMMFObject::CDummyMMFObject()
	: CMMFObject(KDummyMMFObjectUid)
	{
	}

void CMMFTestConstructController3::CDummyMMFObject::HandleRequest(TMMFMessage& /*aMessage*/)
	{	
	}

CMMFTestConstructController3* CMMFTestConstructController3::NewL(void)
	{
	CMMFTestConstructController3* self = CMMFTestConstructController3::NewLC();	
	CleanupStack::Pop();
	return self;
	}

CMMFTestConstructController3* CMMFTestConstructController3::NewLC(void)
	{
	CMMFTestConstructController3* self = new (ELeave) CMMFTestConstructController3();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CMMFTestConstructController3::CMMFTestConstructController3(void)
	: CMMFTestConstructController()
	{	
	}

CMMFTestConstructController3::~CMMFTestConstructController3(void)
	{

	}
                    
void CMMFTestConstructController3::ConstructL (void)
	{
	CMMFTestConstructController::ConstructL();
	CDummyMMFObject* dummyObject = CDummyMMFObject::NewL();
	CleanupStack::PushL(dummyObject);
	MMFObjectContainerL().AddMMFObject(*dummyObject);
	CleanupStack::Pop(dummyObject);
	
	TBool sdrmmode = IsSecureDrmModeL();
	}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs


const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfTestConstructController1Uid,	CMMFTestConstructController::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfTestConstructController2Uid,	CMMFTestConstructController2::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KMmfTestConstructController3Uid,	CMMFTestConstructController3::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

