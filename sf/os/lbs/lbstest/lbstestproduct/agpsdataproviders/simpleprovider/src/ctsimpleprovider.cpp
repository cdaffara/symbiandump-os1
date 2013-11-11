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
// This file provides the class implementation of the ECOM plug-in 
// interface for the LBS test Network Protocol Module.
// 
//

#include "ctsimpleprovider.h"
#include <ecom/implementationproxy.h>
#include <lbs/test/tlbsutils.h>


/** The unique ID of this plug-in interface.
This corresponds to the interface UID specified in the .rss file
for this test assistance data provider.
*/
const TInt KPluginUidValue = 0x10281D77;

CTSimpleProviderModule* CTSimpleProviderModule::NewL(TAny* aInitParams)
	{
	CTSimpleProviderModule* self=new(ELeave) CTSimpleProviderModule(aInitParams);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


CTSimpleProviderModule::~CTSimpleProviderModule()
	{
	}


CTSimpleProviderModule::CTSimpleProviderModule(TAny* aInitParams): 
	iObserver(REINTERPRET_CAST(TAssistanceDataSourceModuleConstructionParams*, aInitParams)->iAssistanceDataSourceObserver)
	{
	// See ConstructL() for initialisation completion.
	}


void CTSimpleProviderModule::ConstructL()
	{
	}


CAssistanceDataSourceBase* CTSimpleProviderModule::NewDataSourceL()
	{
	return CTSimpleProviderSession::NewL(iObserver);
	}


/** Defines the plug-in interface implementation UIDs, required by ECOM.
*/
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(KPluginUidValue, CTSimpleProviderModule::NewL)
    };
    

/** Exported function for ECOM to retrieve implementations table
*/
EXPORT_C 
const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }






CTSimpleProviderSession* CTSimpleProviderSession::NewL(MLbsAssistanceDataSourceObserver& aObserver)
	{
	CTSimpleProviderSession* self= new(ELeave) CTSimpleProviderSession(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


CTSimpleProviderSession::CTSimpleProviderSession(MLbsAssistanceDataSourceObserver& aObserver): 
    iObserver(&aObserver),
    iBuilderSet(NULL)
	{
	}


void CTSimpleProviderSession::ConstructL()
	{
	}


CTSimpleProviderSession::~CTSimpleProviderSession()
	{
	}


/** GetAssistanceData() Called by Network Simulator to request assistance data from the remote
server.

@param aBuilderSet. The CTSimpleProviderSession will set the fields of this builder with
			the assistance data.
*/
void CTSimpleProviderSession::GetAssistanceData(RLbsAssistanceDataBuilderSet& aBuilderSet, const TLbsAsistanceDataGroup& aAssistanceDataFilter, const TCoordinate& aPosition)
	{
	// We don't use reference position.
	(void)aPosition;

	TInt err = KErrNone;

	switch (aAssistanceDataFilter)
		{
		// Return not supported for ref time on it's own.
		case EAssistanceDataReferenceLocation:
			{
			// Notify not found to netsim.
			err = KErrNotFound;
			}
			break;

		// Only support ref time, but can accept the request for a larger amount of assistance data
		case EAssistanceDataReferenceTime:
		case EAssistanceDataReferenceTime | EAssistanceDataReferenceLocation:
		case EAssistanceDataReferenceTime | EAssistanceDataReferenceLocation | EAssistanceDataNavigationModel:
			{
			iBuilderSet = &aBuilderSet;

			// Access the Gps Reference Time which is the only data we support in this simple provider.
			RUEPositioningGpsReferenceTimeBuilder* refTimeBuilder = NULL;

			iBuilderSet->GetDataBuilder(refTimeBuilder);
			if (refTimeBuilder)
				{
				// Fill out the reference time items we support - note the builder set is already open.
				refTimeBuilder->SetField(TUEPositioningGpsReferenceTime::EGpsWeek, KAssistanceData_RefTime_Week);
				refTimeBuilder->SetField(TUEPositioningGpsReferenceTime::EGpsTow1Msec, KAssistanceData_RefTime_Tow1MSec);

				// Time stamp the data.
				TTime timeStamp;
				timeStamp.UniversalTime();

				refTimeBuilder->SetTimeStamp(timeStamp);
				}
				
			else
				{
				err = KErrNotFound;
				}
			}
			break;
			
		default:
			// consider panic/assert
			break;
		}
	
	// Notify success/failure to netsim.
	iObserver->DataSourceNotification(err);
	}


/** CancelGetAssistanceData.
*/
void CTSimpleProviderSession::CancelGetAssistanceData()
	{
	iBuilderSet->ClearAllBuilders();
	}




