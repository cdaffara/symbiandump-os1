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
// ctsimpleprovider.h
// 
//

#ifndef SUPL_PROVIDER_H
#define SUPL_PROVIDER_H


// Epoc includes.
#include <e32base.h>

// Lbs includes.
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatasourcemodule.h>

/**
This class defines the LBS Assistance Data Source Module
plug-in interface. It is used by the LBS Network Simulator.

@see CAssistanceDataSourceModule

The class also implements the internal MProtocolMgrObserver interface
which is used to receive calls from within the protocol module.
@see MProtocolMgrObserver
*/
NONSHARABLE_CLASS(CSuplProviderSession) : public CAssistanceDataSourceBase,	
										  public MLbsNetworkProtocolObserver2
	{
public:
    static CSuplProviderSession* NewL(MLbsAssistanceDataSourceObserver& aObserver);
	~CSuplProviderSession();
	
	// Implementation of interface CAsssistanceDataSourceBase
	void GetAssistanceData(RLbsAssistanceDataBuilderSet& aSet, const TLbsAsistanceDataGroup& aAssistanceDataFilter, const TCoordinate& aPosition);
	void CancelGetAssistanceData();
	
	// Implementation of MLbsNetworkProtocolObserver
	virtual void GetCurrentCapabilities(TLbsNetPosCapabilities& aCapabilities) const;
	virtual void ProcessStatusUpdate(TLbsNetProtocolServiceMask aActiveServiceMask);
	virtual void ProcessPrivacyRequest(const TLbsNetSessionId& aSessionId,
									   TBool aEmergency, 
									   const TLbsNetPosRequestPrivacy& aPrivacy,
									   const TLbsExternalRequestInfo& aRequestInfo);
	virtual void ProcessLocationRequest(const TLbsNetSessionId& aSessionId,
										TBool aEmergency,
										TLbsNetProtocolService aService, 
										const TLbsNetPosRequestQuality& aQuality,
										const TLbsNetPosRequestMethod& aMethod);
	virtual void ProcessSessionComplete(const TLbsNetSessionId& aSessionId,
										TInt  aReason);
	virtual void ProcessAssistanceData(TLbsAsistanceDataGroup aDataMask,
									   const RLbsAssistanceDataBuilderSet& aData,
									   TInt aReason);
	virtual void ProcessLocationUpdate(const TLbsNetSessionId& aSessionId,
									   const TPositionInfoBase& aPosInfo);	

	// Implementation of MLbsNetworkProtocolObserver
	virtual void ProcessAssistanceData(TLbsAsistanceDataGroup aDataMask,
									   const RLbsAssistanceDataBuilderSet& aData,
									   TInt aReason,
									   const TLbsNetSessionIdArray& aSessionIdArray);
	
private:
	CSuplProviderSession(MLbsAssistanceDataSourceObserver& aObserver);
	void ConstructL();
	
private:
	// From the Network Simulator
	MLbsAssistanceDataSourceObserver* iObserver; // For callbacks into the NetSim
	RLbsAssistanceDataBuilderSet* iBuilderSet; // Not owned, belongs to the NetSim
	
	// Used to request assistance data
	CLbsNetworkProtocolBase* iProvider;
	
	// The assitance data we are after
	TLbsAsistanceDataGroup iAssistanceDataMask;
	};

/**
This class implements the LBS Assistance Data Source Module
plug-in interface. It is used by the LBS Network Simulator.
@see CAssistanceDataSourceModule


@see 
*/
NONSHARABLE_CLASS(CSuplProviderModule) : public CAssistanceDataSourceModuleBase
	{
	
public:

	static CSuplProviderModule* NewL(TAny* aParams);
	~CSuplProviderModule();

    CAssistanceDataSourceBase* NewDataSourceL();
private:
	CSuplProviderModule(TAny*);
	CSuplProviderModule();
	void ConstructL();
private:

	/** Reference to this module's observer.
	This is used to make callbacks into the Network Simulator.
	*/
	MLbsAssistanceDataSourceObserver& iObserver;
	};

#endif // SUPL_PROVIDER_H
