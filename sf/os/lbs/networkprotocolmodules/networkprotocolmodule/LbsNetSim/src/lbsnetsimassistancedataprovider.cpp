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
// lbsnetsimassitancedataprovider.cpp
// 
//

#include "lbsnetsimassistancedataprovider.h"
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbspositioninfo.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include "lbsdevloggermacros.h"

/** 
	Standard Symbain OS constructor
*/

CLbsNetSimAssistanceDataProvider* CLbsNetSimAssistanceDataProvider::NewL()
	{
		CLbsNetSimAssistanceDataProvider* self = new(ELeave) CLbsNetSimAssistanceDataProvider();
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop();
		
		return self;
	}
	
/**
	Standard C++ constructor
*/
CLbsNetSimAssistanceDataProvider::CLbsNetSimAssistanceDataProvider() :
	iObserver(NULL), iAssistanceDataUid(TUid::Uid(0)), iAssistanceModule(NULL), iAssistanceSource(NULL)
	{
	// Set default Quality(64s fix time, 100m accuracy)
	iQuality.SetMaxFixTime(64*1000000);
	iQuality.SetMinHorizontalAccuracy(100);
	iQuality.SetMinVerticalAccuracy(100);
	iReferenceLocation.SetCoordinate(0, 0);
	}

/**
	Standard C++ destructor
*/
CLbsNetSimAssistanceDataProvider::~CLbsNetSimAssistanceDataProvider()
	{
	iAssistanceData.Close();
	delete iAssistanceSource;
	delete iAssistanceModule;
	}

	
/** 
	Standard Symbian OS second phase constructor
*/
void CLbsNetSimAssistanceDataProvider::ConstructL()
	{
	// Open the assistance data structure
	iAssistanceData.OpenL();
	
	// Load the default assistance data module to start with
	iAssistanceModule = CAssistanceDataSourceModuleBase::NewL(*this);
	/* Note: The assistance data source is not initialised here, due to issues with LbsRoot starting NetSim Up.
		The initialisation of the Assistance Data Source takes place in SetAssistanceDataProvider() , which
		should always be called before using NetSim.*/
	//iAssistanceSource = iAssistanceModule->NewDataSourceL();
	}

/** 
	Sets the observer to use for callbacks
	
	@param aObserver 	the observer too use
*/
void CLbsNetSimAssistanceDataProvider::SetObserver(MLbsNetSimAssistanceDataProviderObserver* aObserver)
	{
	iObserver = aObserver;
	}
	
/**
	Sets the reference location
	
	@param aReferenceLocation	the reference location
	@leave
*/
void CLbsNetSimAssistanceDataProvider::SetReferenceLocationL(TDesC8& /*aReferenceLocation*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
	Sets the reference location
	
	@param aReferenceLocation	the reference location
*/
void CLbsNetSimAssistanceDataProvider::SetReferenceLocation(TPosition& aReferenceLocation)
	{
	iReferenceLocation = aReferenceLocation;
	}
	

/**
*/
void CLbsNetSimAssistanceDataProvider::SetQuality(TLbsNetPosRequestQuality& aQuality)
	{
	iQuality = aQuality;
	}
	
/**
*/
TBool CLbsNetSimAssistanceDataProvider::SetAssistanceDataProvider(TUid aUid)
	{
    LBSLOG2(ELogP1, "CLbsNetSimAssistanceDataProvider::SetAssistanceDataProvider(Uid= dataSourceId  = 0x%08X\n", aUid.iUid);
    
	// Always reload the current plugin to make sure it's properly initialised
	// Delete the old plugins if there's any
	if (iAssistanceDataUid != aUid)
		{
	    LBSLOG(ELogP1, "Delete the old plugins if there's any\n");
	  
 		if (iAssistanceModule != NULL)		
 			{
			delete iAssistanceSource;
			iAssistanceSource = NULL;
			delete iAssistanceModule;
			iAssistanceModule = NULL;
			REComSession::FinalClose();
 			}
		
		// Set the new plugin Uid
		iAssistanceDataUid = aUid;

		// Load the new assistance module plugin	
	    LBSLOG(ELogP1, "Load the new assistance module plugin\n");
	    
		if (iAssistanceModule == NULL)
			{
			if (iAssistanceDataUid.iUid == 0)
				{
			    LBSLOG(ELogP1, "iAssistanceDataUid.iUid == 0\n");
				iAssistanceModule = CAssistanceDataSourceModuleBase::NewL(*this); // Loads default
                LBSLOG(ELogP1, "end iAssistanceDataUid.iUid == 0\n");

				}
			else
				{
			    LBSLOG(ELogP1, "iAssistanceDataUid.iUid != 0 \n");
				iAssistanceModule = CAssistanceDataSourceModuleBase::NewL(iAssistanceDataUid, *this); // Loads specified
                LBSLOG(ELogP1, "end iAssistanceDataUid.iUid != 0 \n");

				}

			// Load the new assistance data source
            LBSLOG(ELogP1, "Load the new assistance data source \n");

			iAssistanceSource = iAssistanceModule->NewDataSourceL();
			
			LBSLOG(ELogP1, "end Load the new assistance data source \n");

			}
		}
	LBSLOG2(ELogP1, "end CLbsNetSimAssistanceDataProvider::SetAssistanceDataProvider(Uid= dataSourceId  = 0x%08X\n", aUid.iUid);
	 
	return ETrue;
	}
	
/**
	Clears the currently set reference locations
	
	@param aReferenceLocation	the reference location
*/
void CLbsNetSimAssistanceDataProvider::ClearReferenceLocations()
	{
	}
	
/**
	Request the assitance data from the assitance data provider
	
	This method will fill in the supplied RLbsAssitanceDataBuilderSet and calls back MLbsNetSimAssitanceDataProviderObserver::ProcessAssitanceData
	
	@param aGroup			the assitance data mask
	@param aAssitanceData	the assitance data structure to fill in
	@return 				ETrue if an observer is set, otherwise EFalse
*/
TBool CLbsNetSimAssistanceDataProvider::RequestAssistanceDataL(TLbsAssistanceDataGroup& aGroup)
	{
	LBSLOG(ELogP1, "CLbsNetSimAssistanceDataProvider::RequestAssistanceDataL()");

	ASSERT(iObserver != NULL);
	
	TBool bSuccess = EFalse;
	
	// Check we have an observer
	if (iObserver != NULL)
		{
		LBSLOG2(ELogP2, "Requesting assistance data from provider, mask = 0x%x", aGroup);
		iRequestedAssistanceDataGroup = aGroup;
		iAssistanceSource->GetAssistanceData(iAssistanceData, 
											 iRequestedAssistanceDataGroup, 
											 iReferenceLocation);
		}

	return bSuccess;
	}


void CLbsNetSimAssistanceDataProvider::DataSourceNotification(const TInt& aError)
	{
	LBSLOG(ELogP1, "CLbsNetSimAssistanceDataProvider::DataSourceNotification()");
	
	TPositionInfo positionInfo;
	positionInfo.SetPosition(iReferenceLocation);
	
	TInt error = aError;
	
	// Check exactly what assistance data we got, if we didn't
	// get any of the requested items then return an error.
	TLbsAsistanceDataGroup mask;
	CalculateValidAssistanceDataMask(iAssistanceData, mask);
	if ((mask & iRequestedAssistanceDataGroup) == 0
		&& error == KErrNone)
		{
		error = KErrAccessDenied;
		}
	
#ifdef ENABLE_LBS_DEV_LOGGER
	TRAPD(err, iObserver->ProcessAssistanceDataL(error, iAssistanceData, positionInfo, iQuality));
	if (err != KErrNone)
		{
		LBSLOG2(ELogP3, "LEAVE from iObserver->ProcessAssistanceDataL: %d", err);
		}
#else
	TRAP_IGNORE(iObserver->ProcessAssistanceDataL(error, iAssistanceData, positionInfo, iQuality));
#endif
	}

/** Generate a bitmask of the valid assistance data items for an assistance data builder set.
*/
void CLbsNetSimAssistanceDataProvider::CalculateValidAssistanceDataMask(
		RLbsAssistanceDataBuilderSet& aAssistanceData,
		TLbsAsistanceDataGroup& aGroup)
	{
	RUEPositioningGpsAlmanacBuilder* almanacPtr;
	RUEPositioningGpsIonosphericModelBuilder* ionosphericPtr;
	RUEPositioningGpsNavigationModelBuilder* navigationPtr;
	RUEPositioningGpsReferenceTimeBuilder* referenceTimePtr;
	RUEPositioningGpsUtcModelBuilder*  utcPtr;
	RUEPositioningGpsAcquisitionAssistanceBuilder* acquisitionPtr;
	RBadSatListBuilder* badSatPtr;
	RReferenceLocationBuilder* referenceLocationPtr;
	
	aGroup = EAssistanceDataNone;

	if (KErrNone == aAssistanceData.GetDataBuilder(almanacPtr))
		{
		if (almanacPtr->IsDataAvailable())
			{
			aGroup |= EAssistanceDataAlmanac;
			}
		}
	
	if (KErrNone == aAssistanceData.GetDataBuilder(acquisitionPtr))
		{
		if (acquisitionPtr->IsDataAvailable())
			{
			aGroup |= EAssistanceDataAquisitionAssistance;
			}
		}
		
	if (KErrNone == aAssistanceData.GetDataBuilder(badSatPtr))
		{
		if (badSatPtr->IsDataAvailable())
			{
			aGroup |= EAssistanceDataBadSatList;
			}
		}
		
	if (KErrNone == aAssistanceData.GetDataBuilder(navigationPtr))
		{
		if (navigationPtr->IsDataAvailable())
			{
			aGroup |= EAssistanceDataNavigationModel;
			}
		}

	if (KErrNone == aAssistanceData.GetDataBuilder(referenceTimePtr))
		{
		if (referenceTimePtr->IsDataAvailable())
			{
			aGroup |= EAssistanceDataReferenceTime;
			}
		}

	if (KErrNone == aAssistanceData.GetDataBuilder(ionosphericPtr))
		{
		if (ionosphericPtr->IsDataAvailable())
			{
			aGroup |= EAssistanceDataIonosphericModel;
			}
		}

	if (KErrNone == aAssistanceData.GetDataBuilder(referenceLocationPtr))
		{
		if (referenceLocationPtr->IsDataAvailable())
			{
			aGroup |= EAssistanceDataReferenceLocation;
			}
		}

	if (KErrNone == aAssistanceData.GetDataBuilder(utcPtr))
		{
		if (utcPtr->IsDataAvailable())
			{
			aGroup |= EAssistanceDataPositioningGpsUtcModel;
			}
		}
	}

