// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// lbsnetsimassitancedataprovider.h
// 
//

/**
 @file
 @internalComponent
 @test
*/


#ifndef LBSNETSIMASSITANCEDATAPROVIDER_H
#define LBSNETSIMASSITANCEDATAPROVIDER_H

#include "lbsnetsim.h"	
#include "lbsnetsimassistancedataproviderobserver.h"

#include <lbsposition.h>
#include <lbs/lbsassistancedatasourcemodule.h>
#include <lbs/lbslocdatasourcegpsbase.h>

class CLbsNetSimAssistanceDataProvider : 
	public MLbsAssistanceDataSourceObserver
/** Class that talks to the assitance data supplier.

@internalTechnology
@test
*/
	{
	public:
		static CLbsNetSimAssistanceDataProvider* NewL();
		~CLbsNetSimAssistanceDataProvider();
		
		// MLbsAssistanceDataSourceObserver
		void DataSourceNotification(const TInt& aError);
		
		// Methods
		void SetObserver(MLbsNetSimAssistanceDataProviderObserver* aObserver);
		void SetReferenceLocationL(TDesC8& aReferenceLocation);
		void SetReferenceLocation(TPosition& aReferenceLocation);
		void SetQuality(TLbsNetPosRequestQuality& aQuality);
		void ClearReferenceLocations();
		TBool SetAssistanceDataProvider(TUid aUid);
		TBool RequestAssistanceDataL(TLbsAssistanceDataGroup& aMask); 
		
	private:
		// Constructor
		CLbsNetSimAssistanceDataProvider();
		void ConstructL();
		
		// Methods
		TBool LoadAssistanceDataProviderL(TUid aUid);
		void CalculateValidAssistanceDataMask(RLbsAssistanceDataBuilderSet& aAssistanceData,
											  TLbsAsistanceDataGroup& aGroup);

	private:
		// Call back observer
		MLbsNetSimAssistanceDataProviderObserver*	iObserver;
		
		// Required info
		TPosition									iReferenceLocation;	
		RLbsAssistanceDataBuilderSet				iAssistanceData;
		TLbsAsistanceDataGroup						iRequestedAssistanceDataGroup;
		TLbsNetPosRequestQuality					iQuality;

		// Getting assistance data
		TUid										iAssistanceDataUid;
		CAssistanceDataSourceModuleBase*			iAssistanceModule;
		CAssistanceDataSourceBase*					iAssistanceSource;
	};


#endif // LBSNETSIMASSITANCEDATAPROVIDER_H
