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
// ctsimpleprovider.cpp
// This file provides the class implementation of the ECOM plug-in
// interface for the LBS test Network Protocol Module.
// 
//

#include "suplprovider.h"
#include <ecom/implementationproxy.h>


/** The unique ID of this plug-in interface.
This corresponds to the interface UID specified in the .rss file
for this test assistance data provider.
*/
const TInt KPluginUidValue = 0x10285AC2;

CSuplProviderModule* CSuplProviderModule::NewL(TAny* aInitParams)
	{
	CSuplProviderModule* self=new(ELeave) CSuplProviderModule(aInitParams);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


CSuplProviderModule::~CSuplProviderModule()
	{
	}


CSuplProviderModule::CSuplProviderModule(TAny* aInitParams):
	iObserver(REINTERPRET_CAST(TAssistanceDataSourceModuleConstructionParams*, aInitParams)->iAssistanceDataSourceObserver)
	{
	}


void CSuplProviderModule::ConstructL()
	{
	}


CAssistanceDataSourceBase* CSuplProviderModule::NewDataSourceL()
	{
	return CSuplProviderSession::NewL(iObserver);
	}


/** Defines the plug-in interface implementation UIDs, required by ECOM.
*/
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KPluginUidValue, CSuplProviderModule::NewL)
    };


/** Exported function for ECOM to retrieve implementations table
*/
EXPORT_C
const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }


CSuplProviderSession* CSuplProviderSession::NewL(MLbsAssistanceDataSourceObserver& aObserver)
	{
	CSuplProviderSession* self= new(ELeave) CSuplProviderSession(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


CSuplProviderSession::CSuplProviderSession(MLbsAssistanceDataSourceObserver& aObserver):
    iObserver(&aObserver),
    iBuilderSet(NULL)
	{
	}


void CSuplProviderSession::ConstructL()
	{
	TUid KLbsSuplUid = {0x10285A9D};

	TLbsNetProtocolModuleParams params(*this);
	iProvider = CLbsNetworkProtocolBase::NewL(KLbsSuplUid, params);
	}


CSuplProviderSession::~CSuplProviderSession()
	{
	delete iProvider;
	}


/** GetAssistanceData() Called by Network Simulator to request assitance data from the remote
server.

@param aBuilderSet. The CTSimpleProviderSession will set the fields of this builder with
			the assistance data.
*/
void CSuplProviderSession::GetAssistanceData(RLbsAssistanceDataBuilderSet& aBuilderSet, const TLbsAsistanceDataGroup& aAssistanceDataFilter, const TCoordinate& /*aPosition*/)
	{
	// Store builder set for later use
	iBuilderSet = &aBuilderSet;

	// Store assistance data mask
	iAssistanceDataMask = aAssistanceDataFilter;

	// Kick off an MO-LR
	TLbsNetSessionId session(TUid::Uid(0x87654321), 0x1111);
	TLbsNetPosRequestOptionsAssistance options;
	TLbsNetPosRequestQuality quality;
	options.SetNewClientConnected(ETrue);
	options.SetRequestQuality(quality);
	options.SetPosMode(TPositionModuleInfo::ETechnologyAssisted);
	options.SetDataRequestMask(aAssistanceDataFilter);

	iProvider->RequestSelfLocation(session, options);
	}


/** CancelGetAssistanceData.
*/
void CSuplProviderSession::CancelGetAssistanceData()
	{
	if(iBuilderSet)
		{
		iBuilderSet->ClearAllBuilders();
		}
	}

void CSuplProviderSession::GetCurrentCapabilities(TLbsNetPosCapabilities& aCapabilities) const
	{
	TInt numMethods = 1;
	TLbsNetPosMethod posMethods[KLbsMaxNetPosMethods];
	posMethods[0].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
	aCapabilities.SetPosMethods(posMethods, numMethods);
	}


void CSuplProviderSession::ProcessStatusUpdate(TLbsNetProtocolServiceMask /*aActiveServiceMask*/)
	{
	// Dont really care about this
	}


void CSuplProviderSession::ProcessPrivacyRequest(const TLbsNetSessionId& /*aSessionId*/,
												 TBool /*aEmergency*/,
												 const TLbsNetPosRequestPrivacy& /*aPrivacy*/,
												 const TLbsExternalRequestInfo& /*aRequestInfo*/)
	{
	User::Invariant();
	}


void CSuplProviderSession::ProcessLocationRequest(const TLbsNetSessionId& /*aSessionId*/,
												  TBool /*aEmergency*/,
												  TLbsNetProtocolService /*aService*/,
												  const TLbsNetPosRequestQuality& /*aQuality*/,
												  const TLbsNetPosRequestMethod& /*aMethod*/)
	{
	// Request the assistance data
	iProvider->RequestAssistanceData(iAssistanceDataMask);
	}


void CSuplProviderSession::ProcessSessionComplete(const TLbsNetSessionId& /*aSessionId*/,
												  TInt  aReason)
	{
	// We can normally ignore this unless the reason is not KErrNone
	if (aReason != KErrNone)
		{
		// Pass the error back to the requester with the error message
		iObserver->DataSourceNotification(aReason);
		}
	}


void CSuplProviderSession::ProcessAssistanceData(TLbsAsistanceDataGroup /*aDataMask*/,
								   				 const RLbsAssistanceDataBuilderSet& aData,
								   				 TInt aReason)
	{
	RLbsAssistanceDataBuilderSet& data = const_cast<RLbsAssistanceDataBuilderSet&>(aData);
	iBuilderSet->MergeL(data);

	iObserver->DataSourceNotification(aReason);

	// Cancel the request as we have got the assistance data that we were after
	TLbsNetSessionId session(TUid::Uid(0x87654321), 0x1111);
	iProvider->CancelSelfLocation(session, KErrCancel);
	}


void CSuplProviderSession::ProcessAssistanceData(TLbsAsistanceDataGroup aDataMask,
								   				 const RLbsAssistanceDataBuilderSet& aData,
								   				 TInt aReason,
								   				 const TLbsNetSessionIdArray& /*aSessionIdArray*/)
	{
	ProcessAssistanceData(aDataMask, aData, aReason);
	}



void CSuplProviderSession::ProcessLocationUpdate(const TLbsNetSessionId& /*aSessionId*/,
								   				 const TPositionInfoBase& /*aPosInfo*/)
	{
	//User::Invariant();
	}
