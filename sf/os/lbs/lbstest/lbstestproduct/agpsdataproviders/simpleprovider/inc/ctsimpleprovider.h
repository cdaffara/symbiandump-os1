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

#ifndef CT_SIMPLE_PROVIDER_H
#define CT_SIMPLE_PROVIDER_H


// Epoc includes.
#include <e32base.h>

// Lbs includes.
#include <lbs/lbsassistancedatasourcemodule.h>


/**
This class defines the LBS Assistance Data Source Module
plug-in interface. It is used by the LBS Network Simulator.

@see CAssistanceDataSourceModule

The class also implements the internal MProtocolMgrObserver interface
which is used to receive calls from within the protocol module.
@see MProtocolMgrObserver
*/
NONSHARABLE_CLASS(CTSimpleProviderSession) : public CAssistanceDataSourceBase
	{

public:

// not sure we can have a asocket - i take it we call this from the module so it's up to us
    static CTSimpleProviderSession* NewL(MLbsAssistanceDataSourceObserver& aObserver);
	~CTSimpleProviderSession();
	
	// Implementation of interface CAsssistanceDataSourceBase
	void GetAssistanceData(RLbsAssistanceDataBuilderSet& aSet, const TLbsAsistanceDataGroup& aAssistanceDataFilter, const TCoordinate& aPosition);
	void CancelGetAssistanceData();
	


	
private:
	CTSimpleProviderSession(MLbsAssistanceDataSourceObserver& aObserver);
	void ConstructL();
	
private:
	// From the Network Simulator
	MLbsAssistanceDataSourceObserver* iObserver; // For callbacks into the NetSim

	RLbsAssistanceDataBuilderSet* iBuilderSet; // Not owned, belongs to the NetSim
	};



/**
This class implements the LBS Assistance Data Source Module
plug-in interface. It is used by the LBS Network Simulator.
@see CAssistanceDataSourceModule


@see 
*/
NONSHARABLE_CLASS(CTSimpleProviderModule) : public CAssistanceDataSourceModuleBase
	{
	
public:

	static CTSimpleProviderModule* NewL(TAny* aParams);
	~CTSimpleProviderModule();

    CAssistanceDataSourceBase* NewDataSourceL();

private:

	CTSimpleProviderModule(TAny*);
	CTSimpleProviderModule();
	void ConstructL();

private:

	/** Reference to this module's observer.
	This is used to make callbacks into the Network Simulator.
	*/
	MLbsAssistanceDataSourceObserver& iObserver;
	};

#endif // CT_SIMPLE_PROVIDER_H
