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
// @file CT_Utils.cpp
// This is the Cpp file which contains the utility functions common across the servers
// 
//

// User includes
#include "tlbsutils.h"
#include "ctlbsconfigreader.h"
#include <lbs/test/ctlbsasyncwaiter.h>
#include "ctlbstestloggermacros.h"

// Epoc includes
#include <f32file.h>
#include <e32math.h>
#include <e32property.h>

// Lbs includes
#include <lbsclasstypes.h>
#include <lbssatellite.h>


// defines
#define BAD_MODULE_ID	0x87654321


/**
*/
EXPORT_C TLbsModuleUpdateItem::TLbsModuleUpdateItem()
	{
	}


/**
*/
EXPORT_C const TPositionExtendedSatelliteInfo& TLbsModuleUpdateItem::Position() const
	{
	return iPosition;
	}

/**
*/
EXPORT_C void TLbsModuleUpdateItem::SetPosition(const TPositionExtendedSatelliteInfo& aPosition)
	{
	iPosition = aPosition;
	}


/**
*/
EXPORT_C const TPositionGpsMeasurementInfo& TLbsModuleUpdateItem::Measurement() const
	{
	return iMeasurement;
	}

/**
*/
EXPORT_C void TLbsModuleUpdateItem::SetMeasurement(const TPositionGpsMeasurementInfo& aMeasurement)
	{
	iMeasurement = aMeasurement;
	}


/**
*/
EXPORT_C TInt TLbsModuleUpdateItem::Error() const
	{
	return iError;
	}

/**
*/
EXPORT_C void TLbsModuleUpdateItem::SetDelay(TInt aDelay)
	{
	iDelay = aDelay;
	}

/**
*/
EXPORT_C TInt TLbsModuleUpdateItem::Delay() const
	{
	return iDelay;
	}

/**
*/
EXPORT_C void TLbsModuleUpdateItem::SetError(TInt aError)
	{
	iError = aError;
	}



EXPORT_C T_LbsUtils::T_LbsUtils()
/** Constructor.
*/
	{
	}


EXPORT_C T_LbsUtils::~T_LbsUtils()
/** Destructor.
*/
	{
	}


/*EXPORT_C static*/ /*void GetDefault_ModuleStatusInfo(TModuleConfiguredStatus& aStatusInfo)
	{
		(void)aStatusInfo;	
	}
*/

EXPORT_C void T_LbsUtils::ResetIntegrationModulesCountOfCancelsL()
/** Sets the test integration modules count of cancels received from LBS to zero.
*/
	{
	// reset integration modules count of number of cancels recieved from LBS 
	TPckgBuf<TCancelCount> cancelCountBuf;
	TCancelCount& cancelCount = cancelCountBuf();
	cancelCount.iCancelCount = 0;
	TInt err = RProperty::Set(KUidSystemCategory, ELbsTestAGpsModuleCancelCount, cancelCountBuf);

	if (err != KErrNone)
		{
		User::Leave(err);
		}
	
	}

EXPORT_C void T_LbsUtils::IncrementIntegrationModulesCountOfCancels()
/** Gets the current test integration modules count of cancels received from LBS.
*/
	{
	TCancelCount readCount;
	TPckg<TCancelCount> pckgount(readCount);
	TInt err = RProperty::Get(KUidSystemCategory, ELbsTestAGpsModuleCancelCount, pckgount);
	if (err != KErrNone)
		{
		// error deliberately ignored! If property is not defined just do nothing as counting of cancels is now not supported
		return;
		}
	
	TPckgBuf<TCancelCount> cancelCountBuf;
	TCancelCount& cancelCount = cancelCountBuf();

	TUint countOfNumberOfCancels = readCount.iCancelCount;
	countOfNumberOfCancels++;
	cancelCount.iCancelCount = countOfNumberOfCancels;
	err = RProperty::Set(KUidSystemCategory, ELbsTestAGpsModuleCancelCount, cancelCountBuf);
	}
EXPORT_C TUint T_LbsUtils::IntegrationModulesCountOfCancelsL()
	{
	TCancelCount readCancelCount;
	TPckg<TCancelCount> pckgCount(readCancelCount);
	TInt err = RProperty::Get(KUidSystemCategory, ELbsTestAGpsModuleCancelCount, pckgCount);
	if (err != KErrNone)
		{
		User::Leave(err);
		}
	return readCancelCount.iCancelCount;
	}

EXPORT_C void T_LbsUtils::CreateTestPropertiesL()
/** Creates the test properties for the test module. To allow data to be passed between
	test steps and the test module.
*/
	{
	const TSecurityPolicy KReadPolicy(ECapability_None);
	const TSecurityPolicy KWritePolicy(ECapabilityWriteDeviceData);


	// Create the test module data bus properties.
	TInt err;
	
	// Data to the test module.
	err = RProperty::Define(KUidSystemCategory, ELbsTestAGpsModuleIn, RProperty::ELargeByteArray, KReadPolicy, KWritePolicy, sizeof(TModuleDataIn));
	if ((KErrNone != err) && (KErrAlreadyExists != err))
	{
		User::LeaveIfError(err);
	}
	
	// Data from the test module.
	err = RProperty::Define(KUidSystemCategory, ELbsTestAGpsModuleOut, RProperty::ELargeByteArray, KReadPolicy, KWritePolicy, sizeof(TModuleDataOut));
	if ((KErrNone != err) && (KErrAlreadyExists != err))
	{
		User::LeaveIfError(err);
	}

	// Data from the test clock plugin.
	err = RProperty::Define(KUidSystemCategory, ELbsTestClockPluginOut, RProperty::ELargeByteArray, KReadPolicy, KWritePolicy, sizeof(TClockPluginDataOut));
	if ((KErrNone != err) && (KErrAlreadyExists != err))
	{
		User::LeaveIfError(err);
	}
	
	// Data from the agps module count of cancels received from lbs
	err = RProperty::Define(KUidSystemCategory, ELbsTestAGpsModuleCancelCount, RProperty::ELargeByteArray, KReadPolicy, KWritePolicy, sizeof(TCancelCount));
	if ((KErrNone != err) && (KErrAlreadyExists != err))
	{
		User::LeaveIfError(err);
	}
	
	// Creating the property for allowing the AGPS module to report when the mode it's running in changes
	err = RProperty::Define(KUidSystemCategory, ELbsTestAGpsModuleModeChanges, RProperty::ELargeByteArray, KReadPolicy, KWritePolicy, sizeof(TLbsGpsOptionsArray));
	if ((KErrNone != err) && (KErrAlreadyExists != err))
		{
			User::LeaveIfError(err);
		}

	// Create other test properties here also - just update the TLbsTestPropertyKeys enum don't add extra Categories.

	}


EXPORT_C void T_LbsUtils::GetConfigured_PosInfosL(const TDesC& aConfigFileName, const TDesC& aConfigSection, RPointerArray<TAny>& aPosInfoArr)
/** Fills a position info array with values read from a configuration ini file.

@param aConfigFileName	The name of the ini file to read. If the file name is empty (0 length) then
						the array will contain a single pos info item with default values.
@param aConfigSection	The section within the ini file to read data from.
@param aPosInfoArr		The pos info array to which the items are added. The array will cleared of
						existing items.
*/
	{
	// Clear array.
	ResetAndDestroy_PosInfoArr(aPosInfoArr);

	// Check for config file, if not present create a single default TPositionInfo.
	if (aConfigFileName.Length() == 0)
		{
		TPositionSatelliteInfo* posInfo = new(ELeave) TPositionSatelliteInfo();
		TPosition position;

		position.SetCoordinate(DEFAULT_NOTIFY_POS_UPDATE_LATITUDE, DEFAULT_NOTIFY_POS_UPDATE_LONGITUDE, DEFAULT_NOTIFY_POS_UPDATE_ALTITUDE);
		position.SetAccuracy(DEFAULT_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY, DEFAULT_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY);
		position.SetCurrentTime();
	
		posInfo->SetPosition(position);

		User::LeaveIfError(aPosInfoArr.Append(posInfo));			
		}
	
	else
		{
		CPosInfoConfigReader* reader;
	
		reader = CPosInfoConfigReader::NewL(aConfigFileName, aConfigSection, aPosInfoArr);
		CleanupStack::PushL(reader);
		
		reader->ProcessL();
	
		CleanupStack::PopAndDestroy(reader);
		}
	}

EXPORT_C void T_LbsUtils::GetConfigured_UpdateArrayL(const TDesC& aConfigFileName, const TDesC& aConfigSection, RPointerArray<TLbsModuleUpdateItem>& aUpdateArr)
/** Fills an updata array structure with values read from a configuration ini file.

@param aConfigFileName	The name of the ini file to read. If the file name is empty (0 length) then
						default values will be used.
@param aConfigSection	The section within the ini file to read data from.
@param aUpdateArr		The update array to be populated.
*/
	{
	// Use default values if file name not given.
	if (aConfigFileName.Length() == 0)
		{
		// I am not sure we want to fill in the array with default values - ??
		}
	
	else
		{
		CUpdateConfigReader* reader;
	
		reader = CUpdateConfigReader::NewL(aConfigFileName, aConfigSection, aUpdateArr);
		CleanupStack::PushL(reader);
		
		reader->ProcessL();
	
		CleanupStack::PopAndDestroy(reader);
		}
	}


// don't think we need this...
EXPORT_C void T_LbsUtils::GetConfigured_ModuleStatusL(const TDesC& aConfigFileName, const TDesC& aConfigSection, TPositionModuleStatus& aModuleStatus, TBool& aDelayReportingModStatus)
	{
	// Use default values if file name not given.
	if (aConfigFileName.Length() == 0)
		{
		aModuleStatus.SetDeviceStatus(TPositionModuleStatus::EDeviceReady);
		aModuleStatus.SetDataQualityStatus(TPositionModuleStatus::EDataQualityNormal);
		aDelayReportingModStatus = EFalse;
		}
	else
		{
		CModuleStatusConfigReader* reader;
	
		reader = CModuleStatusConfigReader::NewL(aConfigFileName, aConfigSection, aModuleStatus, aDelayReportingModStatus);
		CleanupStack::PushL(reader);
				
		reader->ProcessL();
	
		CleanupStack::PopAndDestroy(reader);
		}
	}


EXPORT_C void T_LbsUtils::GetConfigured_ModuleUpdateOptionsL(const TDesC& aConfigFileName, const TDesC& aConfigSection, TPositionUpdateOptions& aUpdateOpts)
/** Fills a module updata options class/structure with values read from a configuration ini file.

@param aConfigFileName	The name of the ini file to read. If the file name is empty (0 length) then
						default values will be used.
@param aConfigSection	The section within the ini file to read data from.
@param aUpdateOpts		The modified update options structure.
*/
	{
	// Use default values if file name not given.
	if (aConfigFileName.Length() == 0)
		{
		aUpdateOpts.SetUpdateInterval(0);
		aUpdateOpts.SetUpdateTimeOut(0);
		aUpdateOpts.SetMaxUpdateAge(0);
		aUpdateOpts.SetAcceptPartialUpdates(EFalse);			
		}
	
	else
		{
		CUpdateOptsConfigReader* reader;
	
		reader = CUpdateOptsConfigReader::NewL(aConfigFileName, aConfigSection, aUpdateOpts);
		CleanupStack::PushL(reader);
		
		reader->ProcessL();
	
		CleanupStack::PopAndDestroy(reader);
		}
	}

EXPORT_C void T_LbsUtils::GetConfigured_ClientDetailsL(const TDesC& aConfigFileName, const TDesC& aConfigSection, 
						RArray<TCTClientDetails>& aClientDetailsArray)
/** Gets the delays for each client with values read from a configuration ini file.

@param aConfigFileName		The name of the ini file to read. If the file name is empty (0 length) then
							default values will be used.
@param aConfigSection		The section within the ini file to read data from.
@param aClientDelayArray	The array of client delays.
*/
	{
	// Use default values if file name not given.
	if (aConfigFileName.Length() == 0)
		{ // by default have one client
		TCTClientDetails client;
		aClientDetailsArray.AppendL(client);
		return;
		}
	
	else
		{
		CClientDetailsConfigReader* reader;
		reader = CClientDetailsConfigReader::NewL(aConfigFileName, aConfigSection, aClientDetailsArray);
		CleanupStack::PushL(reader);
		
		reader->ProcessL();
	
		CleanupStack::PopAndDestroy(reader);
		if(aClientDetailsArray.Count() == 0)
			{ // by default have one client
			TCTClientDetails client;
			aClientDetailsArray.AppendL(client);
			}
		}
	}

EXPORT_C void T_LbsUtils::GetExpected_ModuleModes(const TDesC& aConfigFileName, const TDesC& aConfigSection, RPointerArray<TLbsGpsOptions>& aModuleModes)
	{
	if (aConfigFileName.Length() == 0)
		{
		return;
		}
	
	CAgpsModuleModesConfigReader* reader;
	reader = CAgpsModuleModesConfigReader::NewL(aConfigFileName, aConfigSection, aModuleModes);
	CleanupStack::PushL(reader);
	reader->ProcessL();
	CleanupStack::PopAndDestroy(reader);
	}


EXPORT_C void T_LbsUtils::NotifyModuleOfConfigChangeL(const TModuleDataIn& aModuleDataIn)
/** Notify the module of a configuration change.

Used when the test harness has read a configuration file and wishes to notify the test module
that it should also do a read of the configuration file. To ensure both the test harness and
the test module are using the same reference data.
	
The ELbsTestAGpsModuleIn property is used to pass the ini file and section names to the
test module. The ELbsTestAGpsModuleOut is then used to receive an response from the test module.

@param aConfigFileName	The file name string to send to the test mdoule.
@param aConfigSection	The ini section name string to send to the test module.
*/
	{
	// Ask to be notified when the module receives the ini file re-read request.

	// Attach and subscribe to listen for data bus updates. Use the async waiter to process
	// the update.
	RProperty modDataOutProperty;
	User::LeaveIfError(modDataOutProperty.Attach(KUidSystemCategory, ELbsTestAGpsModuleOut));

	CT_LbsAsyncWaiter* waiter = CT_LbsAsyncWaiter::NewL();
	CleanupStack::PushL(waiter);

	modDataOutProperty.Subscribe(waiter->iStatus);


	// Publish the file and section names to the module, to inform the module it has to
	// carry out a re-read of the ini file.
	TPckgBuf<TModuleDataIn> modDataInBuf;
	TModuleDataIn& modDataIn = modDataInBuf();

	// Copy across all the data items.
	modDataIn.iRequestType = aModuleDataIn.iRequestType;
	modDataIn.iConfigFileName = aModuleDataIn.iConfigFileName;
	modDataIn.iConfigSection = aModuleDataIn.iConfigSection;
	modDataIn.iAssDataEventType = aModuleDataIn.iAssDataEventType;
	modDataIn.iAssDataTestMode = aModuleDataIn.iAssDataTestMode;
	modDataIn.iKickOffEventType = aModuleDataIn.iKickOffEventType;
	modDataIn.iTimeOut = aModuleDataIn.iTimeOut;
	modDataIn.iError = aModuleDataIn.iError;
	modDataIn.iTimeStampOffset = aModuleDataIn.iTimeStampOffset;
    modDataIn.iOptionsChangeReportMode = aModuleDataIn.iOptionsChangeReportMode;
    modDataIn.iOneOffTimeOut = aModuleDataIn.iOneOffTimeOut;
    
	User::LeaveIfError(RProperty::Set(KUidSystemCategory, ELbsTestAGpsModuleIn, modDataInBuf));

	TBool noResponse = ETrue;

	// Wait for the notification from the test module.	
	while(noResponse)
		{
	waiter->StartAndWait();
	User::LeaveIfError(waiter->Result());

	// Now ensure the notification response was good.
	TModuleDataOut modDataOut;
	TPckg<TModuleDataOut> modDataOutPckg(modDataOut);

	User::LeaveIfError(modDataOutProperty.Get(modDataOutPckg));

		if ((TModuleDataOut::EModuleResponse == modDataOut.iRequestType) && (TModuleDataOut::EModuleResponseOk == modDataOut.iResponse)) 
		{
			noResponse = EFalse;
			}
		}

	CleanupStack::PopAndDestroy(waiter);
	}


EXPORT_C void T_LbsUtils::WaitForModuleToRequestAssistanceDataL()
/** Wait for notification that the test AGPS module as requested assistance data.

	Blocking function used to determine when the test module has requested assistance data as part of its location update.
*/
	{
	RProperty modDataOutProperty;
	User::LeaveIfError(modDataOutProperty.Attach(KUidSystemCategory, ELbsTestAGpsModuleOut));

	CT_LbsAsyncWaiter* waiter = CT_LbsAsyncWaiter::NewL();
	CleanupStack::PushL(waiter);

	modDataOutProperty.Subscribe(waiter->iStatus);

	// Wait for the notification from the test module.	
	waiter->StartAndWait();
	User::LeaveIfError(waiter->Result());

	// Ensure the correct response is given.
	TModuleDataOut modDataOut;
	TPckg<TModuleDataOut> modDataOutPckg(modDataOut);

	User::LeaveIfError(modDataOutProperty.Get(modDataOutPckg));
	
	if (TModuleDataOut::EModuleAssDataRequestedOk != modDataOut.iResponse)
		{
		User::LeaveIfError(KErrArgument);
		}

	CleanupStack::PopAndDestroy(waiter);
	}

EXPORT_C TBool T_LbsUtils::Compare_PosInfo(const TPositionInfoBase& aPosInfoSideA, const TPositionInfoBase& aPosInfoSideB)
/** Compare two position info structures. Ensure all fields match excatly.

@param aPosInfoSideA	The first structure to compare.
@param aPosInfoSideB	The second structure to compare.
@return					ETrue if both structure match excatly else EFalse.
*/
	{
	return Compare_PosInfo(aPosInfoSideA, aPosInfoSideB, EExactAccuracy);
	}


EXPORT_C TBool T_LbsUtils::Compare_PosInfo(const TPositionInfoBase& aPosInfoSideA, const TPositionInfoBase& aPosInfoSideB, TComparisonAccuracyType aCmpAccuracy)
	{
	// TODO compare base class items, such as module id, position mode, etc.

	TUint32 typeA = aPosInfoSideA.PositionClassType();
	TUint32 typeB = aPosInfoSideB.PositionClassType();
	
	// Compare TPositionInfo type items.
	if(typeA & typeB & EPositionInfoClass)
		{
		TESTLOG(ELogP1, "T_LbsUtils::Compare_PosInfo() Both positions are of type EPositionInfoClass");
		const TPositionInfo& posInfoSideA = reinterpret_cast<const TPositionInfo&>(aPosInfoSideA);
		const TPositionInfo& posInfoSideB = reinterpret_cast<const TPositionInfo&>(aPosInfoSideB);
		
		TPosition posSideA;
		TPosition posSideB;
		posInfoSideA.GetPosition(posSideA);
		posInfoSideB.GetPosition(posSideB);

		// Carry out an exact check when comparing items.
		if (EExactAccuracy == aCmpAccuracy)
			{
			TESTLOG(ELogP1, "T_LbsUtils::Compare_PosInfo() Comparing positions for exact match");
			// Compare latitude. 
			if (Math::IsNaN(posSideA.Latitude()) && Math::IsNaN(posSideB.Latitude()))
				;
			else if (posSideA.Latitude() != posSideB.Latitude())
				{
				TESTLOG(ELogP1, "T_LbsUtils::Compare_PosInfo() Positions don't match!");
				TESTLOG3(ELogP1, "Latitudes %d and %d respectively", posSideA.Latitude(), posSideB.Latitude());
				return EFalse;
				}
			
			// Compare longitude.
			if (Math::IsNaN(posSideA.Longitude()) && Math::IsNaN(posSideB.Longitude()))
				;
			else if (posSideA.Longitude() != posSideB.Longitude())
				{
				TESTLOG(ELogP1, "T_LbsUtils::Compare_PosInfo() Positions don't match!");
				TESTLOG3(ELogP1, "Longitudes %d and %d respectively", posSideA.Longitude(), posSideB.Longitude());
				return EFalse;
				}		
			
			// Compare altitude.
			if (Math::IsNaN(posSideA.Altitude()) && Math::IsNaN(posSideB.Altitude()))
				;
			else if (posSideA.Altitude() != posSideB.Altitude())
				{
				TESTLOG(ELogP1, "T_LbsUtils::Compare_PosInfo() Positions don't match!");
				TESTLOG3(ELogP1, "Altitudes %d and %d respectively", posSideA.Altitude(), posSideB.Altitude());				
				return EFalse;
				}
			
			// Compare datum.
			if (posSideA.Datum() != posSideB.Datum())
				{
				TESTLOG(ELogP1, "T_LbsUtils::Compare_PosInfo() Positions don't match!");
				TESTLOG3(ELogP1, "Datums %d and %d respectively", posSideA.Datum(), posSideB.Datum());				
				return EFalse;
				}				
			
			// Compare horizontal accuracy.
			if (Math::IsNaN(posSideA.HorizontalAccuracy()) && Math::IsNaN(posSideB.HorizontalAccuracy()))
				;
			else if (posSideA.HorizontalAccuracy() != posSideB.HorizontalAccuracy())
				{
				TESTLOG(ELogP1, "T_LbsUtils::Compare_PosInfo() Positions don't match!");
				TESTLOG3(ELogP1, "Horizontal Accuracies %d and %d respectively", posSideA.HorizontalAccuracy(), posSideB.HorizontalAccuracy());				
				return EFalse;
				}		
			
			// Compare vertical accuracy.
			if (Math::IsNaN(posSideA.VerticalAccuracy()) && Math::IsNaN(posSideB.VerticalAccuracy()))
				;
			else if (posSideA.VerticalAccuracy() != posSideB.VerticalAccuracy())
				{
				TESTLOG(ELogP1, "T_LbsUtils::Compare_PosInfo() Positions don't match!");
				TESTLOG3(ELogP1, "Vertical Accuracies %d and %d respectively", posSideA.VerticalAccuracy(), posSideB.VerticalAccuracy());				
				return EFalse;
				}		
			}
		else
			{
			// Check latitude + longitude using horz accuracy.
			TReal horzAct = posSideA.HorizontalAccuracy(); // Use the verify accuracy value (which is side A).
			TReal distance ;			
	
			TESTLOG(ELogP1, "T_LbsUtils::Compare_PosInfo() Comparing positions for 'rough' match");			
		//	if(NAN != horzAct)
				{
				// The following is a temporary patch until TPositionInfo.Distance() is implemented:				
/* 
	from http://www.movable-type.co.uk/scripts/GIS-FAQ-5.1.html
	
	presuming a spherical Earth with radius R (see below), and the locations of the two points in spherical coordinates (longitude and latitude) are lon1,lat1 and lon2,lat2 then the

	Haversine Formula (from R.W. Sinnott, "Virtues of the Haversine", Sky and Telescope, vol. 68, no. 2, 1984, p. 159):

	dlon = lon2 - lon1
	dlat = lat2 - lat1
	a = sin^2(dlat/2) + cos(lat1) * cos(lat2) * sin^2(dlon/2)
	c = 2 * arcsin(min(1,sqrt(a)))
	d = R * c

	will give mathematically and computationally exact results. 
	
*/				
				const TReal pi = 3.141592653589793;
				const TReal earthRadius = 6367 * 1000;	// earth radius in metres
				
				TReal32 latA = posSideA.Latitude() * (pi/180);
				TReal32 latB = posSideB.Latitude() * (pi/180);
				TReal32 lonA = posSideA.Longitude() * (pi/180);
				TReal32 lonB = posSideB.Longitude() * (pi/180);
				
				TReal dlon = (lonB - lonA);	
				TReal dlat = (latB - latA);
				TReal sin_half_dlat, sin_half_dlon, coslatA, coslatB;
				
				Math::Sin(sin_half_dlat, dlat/2);
				Math::Sin(sin_half_dlon, dlon/2);
				Math::Cos(coslatA, latA);
				Math::Cos(coslatB, latB);
				
				TReal a = (sin_half_dlat * sin_half_dlat) + (coslatA * coslatB * (sin_half_dlon * sin_half_dlon));
				TReal sqrt_a;
				Math::Sqrt(sqrt_a, a);
				TReal arcsinmin;
				
				TReal min = Min(static_cast<TReal>(1), sqrt_a);
				Math::ASin(arcsinmin, min);
				
				distance = earthRadius * (2 * arcsinmin);
				
				//__ASSERT_ALWAYS(!Math::IsNaN(distance), User::Panic(_L("Lbs Test Utils"), KErrGeneral));
				if(Math::IsNaN(latA) || Math::IsNaN(lonA) || Math::IsNaN(horzAct))
					{	
					TESTLOG(ELogP1, "T_LbsUtils::Compare_PosInfo() Positions don't match because contains NaNs!");	
					return EFalse;			
					}
				else if(distance > horzAct + 30)	// lrm allow for 30m discrepency for now TO DO figure out whether we should be able to check finer accuracy
					{
					TESTLOG(ELogP1, "T_LbsUtils::Compare_PosInfo() Positions don't match because distance greater than reported accuracy + margin!");
					return EFalse;
					}
				}
				
			/*	put back later:	
			TReal32 horzAct = posSideA.HorizontalAccuracy(); // Use the verify accuracy value (which is side A).
			TReal32 distance ;			
			
			posSideA.Distance(posSideB, distance);
			if (distance > horzAct)
				return EFalse;
		
			// Check altitude using vert accuracy.
			TReal32 vertAct = posSideA.VerticalAccuracy(); // Use the verify accuracy value (which is side A).
			TReal32 height = Abs(posSideA.Altitude() - posSideB.Altitude());
			if (height > vertAct)
				return EFalse;
			*/
			
			}
			
		// TODO, we don't compare times, not sure if this is something we would do later on
//		if (posSideA.Time() != posSideB.Time())
//			return EFalse;
		
		return ETrue;
		}

	// Compare TPositionCourseInfo type items.		
	if (typeA & typeB & EPositionCourseInfoClass)
		{
		TESTLOG(ELogP1, "T_LbsUtils::Compare_PosInfo() Both positions are of type EPositionCourseInfoClass");
		// TODO
		}

	// Compare TPositionSatelliteInfo type items.
	if (typeA & typeB & EPositionSatelliteInfoClass)
		{
		TESTLOG(ELogP1, "T_LbsUtils::Compare_PosInfo() Both positions are of type EPositionSatelliteInfoClass");
		// TODO
		}

/* For extended tests when we have them.
	if (infoBase.PositionClassType() & EPositionClassTestExtension)
		{
		}
*/
	return EFalse;
	}
	
	
EXPORT_C TBool T_LbsUtils::Compare_PosInfoArr(RPointerArray<TAny>& aPosInfoArrSideA, RPointerArray<TAny>& aPosInfoArrSideB)
	{
	return Compare_PosInfoArr(aPosInfoArrSideA, aPosInfoArrSideB, EExactAccuracy);
	}

EXPORT_C TBool T_LbsUtils::Compare_PosInfoArr(RPointerArray<TAny>& aPosInfoArrSideA, RPointerArray<TAny>& aPosInfoArrSideB, TComparisonAccuracyType aCmpAccuracy)
	{
	// Only compare items if the arrays are the same size.
	if (aPosInfoArrSideA.Count() == aPosInfoArrSideB.Count())
		{
		TPositionInfoBase* posInfoA;
		TPositionInfoBase* posInfoB;
		TBool res;
		
		for (TInt i = 0; i < aPosInfoArrSideA.Count(); ++i)
			{
			posInfoA = reinterpret_cast<TPositionInfoBase*>(aPosInfoArrSideA[i]);
			posInfoB = reinterpret_cast<TPositionInfoBase*>(aPosInfoArrSideB[i]);
			
			res = Compare_PosInfo(*posInfoA, *posInfoB, aCmpAccuracy);
			if (!res)
				return EFalse;
			}
		}
	
	else
		{
		TESTLOG(ELogP1, "T_LbsUtils::Compare_PosInfoArr() Failed because different number of positions in arrays being compared!");
		return EFalse;
		}
	return ETrue;
	}


EXPORT_C void T_LbsUtils::ResetAndDestroy_PosInfoArr(RPointerArray<TAny>& aPosInfoArr)
	{
		for (TInt i = 0; i < aPosInfoArr.Count(); ++i)
			delete aPosInfoArr[i];
		aPosInfoArr.Reset();	
	}


EXPORT_C TPositionModuleId T_LbsUtils::GetNetworkModuleIdL(RPositionServer& aServer)
	{
	TPositionModuleId modId;

	User::LeaveIfError(GetModIdByTechnology(aServer, TPositionModuleInfo::ETechnologyNetwork, modId));
	
	return modId;
	}


EXPORT_C TPositionModuleId T_LbsUtils::GetAGpsModuleIdL(RPositionServer& aServer)
	{
	TPositionModuleId modId;

	User::LeaveIfError(GetModIdByTechnology(aServer, (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted), modId));
	
	return modId;
	}
	
EXPORT_C TPositionModuleId T_LbsUtils::GetBadModuleId()
	{
	TPositionModuleId modId;
	
	modId.iUid = BAD_MODULE_ID;
		
	return modId;
	}	


EXPORT_C TBool T_LbsUtils::Compare_ModuleInfo(TPositionModuleInfo& aModInfoSideA, TPositionModuleInfo& aModInfoSideB)
	{
	// Compare all module information items and quality.
	
	// Compare module id.
	if (aModInfoSideA.ModuleId() != aModInfoSideB.ModuleId())
		{
		return EFalse;
		}	

	//	Compare is available flag.
	if (aModInfoSideA.IsAvailable() != aModInfoSideB.IsAvailable())
		{
		return EFalse;
		}	

	// Compare technology type.
	if (aModInfoSideA.TechnologyType() != aModInfoSideB.TechnologyType())
		{
		return EFalse;
		}	

	//	Compare device location.
	if (aModInfoSideA.DeviceLocation() != aModInfoSideB.DeviceLocation())
		{
		return EFalse;
		}	

	//	Compare capabilities.
	if (aModInfoSideA.Capabilities() != aModInfoSideB.Capabilities())
		{
		return EFalse;
		}	

	// Compare each of the family classes. Use min and max enum values.
	for (TInt i = EPositionInfoFamily; i <= EPositionUpdateOptionsFamily; i++)
		{
		if (aModInfoSideA.ClassesSupported(static_cast<TPositionClassFamily>(i)) != aModInfoSideB.ClassesSupported(static_cast<TPositionClassFamily>(i)))
			{
			return EFalse;
			}	
		}
		
	// Compare version.
	if ((aModInfoSideA.Version().iMajor != aModInfoSideB.Version().iMajor) ||
		(aModInfoSideA.Version().iMinor != aModInfoSideB.Version().iMinor) ||
		(aModInfoSideA.Version().iBuild != aModInfoSideB.Version().iBuild))
		{
		return EFalse;
		}	

	// Compare module name.
	TBuf<KPositionMaxModuleName> modNameSideA;
	TBuf<KPositionMaxModuleName> modNameSideB;

	aModInfoSideA.GetModuleName(modNameSideA);
	aModInfoSideB.GetModuleName(modNameSideB);
	if (0 != modNameSideA.Compare(modNameSideB))
	{
		return EFalse;
	}

	// Compare module quality.
	TPositionQuality qualitySideA;
	TPositionQuality qualitySideB;	
	
	aModInfoSideA.GetPositionQuality(qualitySideA);
	aModInfoSideB.GetPositionQuality(qualitySideB);
	
	return Compare_PositionQuality(qualitySideA, qualitySideB);
	}

EXPORT_C TBool T_LbsUtils::Compare_ModuleStatus(TPositionModuleStatus& aModStatusSideA, TPositionModuleStatus& aModStatusSideB)
	{
	// Compare device status.
	if (aModStatusSideA.DeviceStatus() != aModStatusSideB.DeviceStatus())
		{
		return EFalse;
		}	

	//	Compare quality status.
	if (aModStatusSideA.DataQualityStatus() != aModStatusSideB.DataQualityStatus())
		{
		return EFalse;
		}	
		
	return TRUE;
	}
	
	
EXPORT_C TBool Compare_ModuleUpdateOptions(TPositionUpdateOptions& aModUpdateOptsSideA, TPositionUpdateOptions& aModUpdateOptsSideB)
	{
	// Compare base class items.
	if(aModUpdateOptsSideA.PositionClassSize() != aModUpdateOptsSideB.PositionClassSize())
		{
			return EFalse;
		}

	if(aModUpdateOptsSideA.PositionClassType() != aModUpdateOptsSideB.PositionClassType())
		{
			return EFalse;
		}
	
	// Compare interval option.
	if(aModUpdateOptsSideA.UpdateInterval() != aModUpdateOptsSideB.UpdateInterval())
		{
			return EFalse;
		}
	
	// Compare timeout option.
	if(aModUpdateOptsSideA.UpdateTimeOut() != aModUpdateOptsSideB.UpdateTimeOut())
		{
			return EFalse;
		}

	// Compare maxage option.
	if(aModUpdateOptsSideA.MaxUpdateAge() != aModUpdateOptsSideB.MaxUpdateAge())
		{
			return EFalse;
		}

	// Compare partial update option.
	if(aModUpdateOptsSideA.AcceptPartialUpdates() != aModUpdateOptsSideB.AcceptPartialUpdates()) 
		{
			return EFalse;
		}
		
	return ETrue;
	}


EXPORT_C TBool T_LbsUtils::Compare_PositionQuality(TPositionQuality& aQualitySideA, TPositionQuality& aQualitySideB)
/** Compare two position quality structures.

@param aQualitySideA	
@param aQualitySideB	
*/
	{
	// Check if each quality contains the same number of assigned quality items.
	if (aQualitySideA.HighWaterMark() != aQualitySideB.HighWaterMark())
	{
		return EFalse;
	}
	
	// Compare each quality item.
	TInt endId = aQualitySideA.HighWaterMark();
	
	for (TInt i = 0; i <= endId; i++)
		{
		// Element for each side must be defined.
		if (!aQualitySideA.IsDefined(i) || !aQualitySideB.IsDefined(i))
			{
			return EFalse;
			}

		// Now compare the actual element.
		TPositionQualityItem::TResult res;
		if (KErrNone != aQualitySideA.Compare(aQualitySideB, i, res))
			{
			return EFalse;	
			}
		if (TPositionQualityItem::EIsEqual != res)
			{
			return EFalse;
			}
		}

	return ETrue;	
	}


/**
 *  Verifies that the supplied position is 'reasonable'
 *
 */
EXPORT_C TBool T_LbsUtils::Verify_PositionIsValid(TPositionInfo& aPosInfo)
	{
	TBool valid = TRUE;
	TPosition pos;
	TReal32 alt;
	TReal64 lat, longt;
	
	aPosInfo.GetPosition(pos);
	
	alt = pos.Altitude();
	lat = pos.Latitude();
	longt = pos.Longitude();
	
	// TO DO figure out what values are reasonable here (in Milton somewhere!?)
	// We could use the normal verify posinfo stuff, and check to see if the values are roughly equal.
	// Either update this func (add a parameter) or new func like the compare posinfo func we have
	if(alt == 0 || lat == 0 || longt == 0)
		{
		valid = FALSE;
		}
	
	
	return valid;
	}


// May not be required.
//EXPORT_C void T_LbsUtils::CreateSimpleAssistanceData(/*TLbsAssistanceDataGroup aDataMask, RDataReaderRootBase& aGpsRefTimeReader*/)
//	{

//	}

/** Ensure the simple assistance data is correct. Typically the data is given by the simple
	assistance data provider module used by the Net Sim.
	
@param aRefTimeReader The assistance data reader which holds the reference time data, the 
						reader must be opened (use OpenL()) before calling this function.
*/
EXPORT_C TBool T_LbsUtils::VerifySimpleAssistanceData(RUEPositioningGpsReferenceTimeReader& aRefTimeReader)
	{    
	TUint week;
	TUint tow1MSec;	                                            

	// Access the reference time values.
	aRefTimeReader.GetField(TUEPositioningGpsReferenceTime::EGpsWeek, week);
	aRefTimeReader.GetField(TUEPositioningGpsReferenceTime::EGpsTow1Msec, tow1MSec);
			
	// Verify the reference time values.
	TBool passed = ETrue;
	
	passed &= (week == KAssistanceData_RefTime_Week);
	passed &= (tow1MSec == KAssistanceData_RefTime_Tow1MSec);

	return passed;
	}

/** Create the module information depending on module id(i'e either agps_manager or network location manager) 
    Fills a module info class/structure with values depending on the module id. 

    @param  aModuleId       module id within the ini file
    @param  aModInfo		The modified module info structure.
*/
EXPORT_C void T_LbsUtils::Create_ModuleInfoL(TPositionModuleId aModuleId, TPositionModuleInfo& aModInfo)

	{  
	   TPositionQuality posQuality;
	
	// Use these values if the module is a-gps manager 
		if (aModuleId == (TUid::Uid(APGS_MODINFO_MODULE_ID)))
		 {
		 aModInfo.SetModuleId(TUid::Uid(APGS_MODINFO_MODULE_ID));
	     aModInfo.SetIsAvailable(AGPS_MODINFO_IS_AVAILABLE);
	     aModInfo.SetModuleName(AGPS_MODINFO_MOD_NAME);
	     aModInfo.SetTechnologyType(AGPS_MODINFO_TECHNOLOGY_TYPE);
	     aModInfo.SetDeviceLocation(AGPS_MODINFO_DEVICE_LOCATION);
	     aModInfo.SetCapabilities(AGPS_MODINFO_CAPABILITIES);
	      
	     // Classes Supported - this is a binary bitmap, but support for each class 
    	// has to be set individually.
		
	 	TInt supportedClasses = AGPS_MODINFO_CLASSES_SUPPORTED_POSITION_FAMILY; 
    	TPositionClassFamily currentClass = EPositionInfoFamily;
    	while(currentClass <= EPositionUpdateOptionsFamily)
        	{
        	aModInfo.SetClassesSupported(currentClass, supportedClasses & 1); 
        	supportedClasses >>= 1;
        	currentClass = static_cast<TPositionClassFamily>(static_cast<TInt>(currentClass) + 1);   		
        	}
	     	     
	     aModInfo.SetVersion(AGPS_MODINFO_VERSION);
    	 posQuality.SetTimeToFirstFix(AGPS_MODINFO_TIME_TO_FIRST_FIX);
	     posQuality.SetTimeToNextFix(AGPS_MODINFO_TIME_TO_NEXT_FIX);
    	 posQuality.SetHorizontalAccuracy(AGPS_MODINFO_HORIZONTAL_ACCURACY);
	     posQuality.SetVerticalAccuracy(AGPS_MODINFO_VERTICAL_ACCURACY);
    	 posQuality.SetCostIndicator(AGPS_MODINFO_COST_INDICATOR);
	     posQuality.SetPowerConsumption(AGPS_MODINFO_POWER_CONSUMPTION);
       	 aModInfo.SetPositionQuality(posQuality);
      	 }
	
	//need to change after network location manager module is avaliable
	//Use these values if the module is network location manager
	   else if(aModuleId ==(TUid::Uid(NETWORK_MODINFO_MODULE_ID)))
		   {
		   aModInfo.SetModuleId(TUid::Uid(NETWORK_MODINFO_MODULE_ID));
	       aModInfo.SetIsAvailable(NETWORK_MODINFO_IS_AVAILABLE);
	       aModInfo.SetModuleName(NETWORK_MODINFO_MOD_NAME);
	       aModInfo.SetTechnologyType(NETWORK_MODINFO_TECHNOLOGY_TYPE);
	       aModInfo.SetDeviceLocation(NETWORK_MODINFO_DEVICE_LOCATION);
	       aModInfo.SetCapabilities(NETWORK_MODINFO_CAPABILITIES);
	       // Classes Supported - this is a binary bitmap, but support for each class 
    	   // has to be set individually.
		
	 	   TInt supportedClasses = NETWORK_MODINFO_CLASSES_SUPPORTED_POSITION_FAMILY; 
    	   TPositionClassFamily currentClass = EPositionInfoFamily;
    	   while(currentClass <= EPositionUpdateOptionsFamily)
        	{
        	aModInfo.SetClassesSupported(currentClass, supportedClasses & 1); 
        	supportedClasses >>= 1;
        	currentClass = static_cast<TPositionClassFamily>(static_cast<TInt>(currentClass) + 1);   		
        	}
	      	      
	       aModInfo.SetVersion(NETWORK_MODINFO_VERSION);
    	   posQuality.SetTimeToFirstFix(NETWORK_MODINFO_TIME_TO_FIRST_FIX);
	       posQuality.SetTimeToNextFix(NETWORK_MODINFO_TIME_TO_NEXT_FIX);
    	   posQuality.SetHorizontalAccuracy(NETWORK_MODINFO_HORIZONTAL_ACCURACY);
	       posQuality.SetVerticalAccuracy(NETWORK_MODINFO_VERTICAL_ACCURACY);
    	   posQuality.SetCostIndicator(NETWORK_MODINFO_COST_INDICATOR);
	       posQuality.SetPowerConsumption(NETWORK_MODINFO_POWER_CONSUMPTION); 
	       aModInfo.SetPositionQuality(posQuality);
		  }
	  else
	     {
	       	User::Leave(KErrArgument);
	      
	     }  
	 }

TInt T_LbsUtils::GetModIdByTechnology(RPositionServer& aServer, TPositionModuleInfo::TTechnologyType aTechnologyType, TPositionModuleId& aModuleId)
/** Searchs for a module which matches the required technology type. Once found the module id of the matched module
	is given. The first module found which supports the technology will be returned.

@param aServer			A position server. A client must have a connection with the server.
@param aTechnologyType	The technology to match.
@param aModuleId		The returned module id of the module which supports the required technology.
@return					Err code:	KErrNone it module found.
									KErrNotFound if no module is found which matches the required technology.
									Any other system errors.
*/
{
	TInt err;
	TUint numMods;

	err = aServer.GetNumModules(numMods);
	if (KErrNone == err)
		{
		TInt i = 0;
		TPositionModuleInfo	modInfo;
		
		// Look at each module until we find the required technology type.
		while (i < numMods)
			{
			err = aServer.GetModuleInfoByIndex(i, modInfo);
		
			// Found it.
			if (modInfo.TechnologyType() == aTechnologyType && modInfo.IsAvailable())
				{
				aModuleId = modInfo.ModuleId();
				
				return err;
				}
			i++;
			}
		}
		
	else
	{
		return err;
	}
	
	return KErrNotFound;
}


/**
 * Method for comparing base class member variables in Logevent types
 */
EXPORT_C TBool T_LbsUtils::Compare_LogBase(const CLbsLogEvent& aLogEventSideA, const CLbsLogEvent& aLogEventSideB)
{
	if(aLogEventSideA.Direction() != aLogEventSideB.Direction())
		return EFalse;
	
	if(aLogEventSideA.EventType() != aLogEventSideB.EventType())
		return EFalse;
	
	if(aLogEventSideA.RequestOutcome() != aLogEventSideB.RequestOutcome())
		return EFalse;

	if(aLogEventSideA.IsCostInformationAvailable() != aLogEventSideB.IsCostInformationAvailable())
		return EFalse;
	else
		{
		//check cost info for each side
		if(aLogEventSideA.CostInformation() != aLogEventSideB.CostInformation())
		return EFalse;
		}
		
	TPositionInfoBase* sideAPosInfo = reinterpret_cast<TPositionInfoBase*>(aLogEventSideA.PositionInfo());
	TPositionInfoBase* sideBPosInfo = reinterpret_cast<TPositionInfoBase*>(aLogEventSideB.PositionInfo());
	if(sideAPosInfo != NULL)
		{
		// If the actual log has a non-NULL posinfo...
		if((*aLogEventSideA.PositionInfo()).ModuleId() != (*aLogEventSideB.PositionInfo()).ModuleId())
			return EFalse;	
		
		if(!Compare_PosInfo(*aLogEventSideA.PositionInfo(), *aLogEventSideB.PositionInfo()))
			return EFalse;	
		}
		else
		{	
			// else cheach that both sides have NULL
			if(!(sideAPosInfo == NULL && sideBPosInfo == NULL))
				return EFalse;	
		}
		


return ETrue;
}
/**
 * Method for comparing SelfLocateLog
 */
EXPORT_C TBool T_LbsUtils::Compare_SelfLocateLog(const CLbsSelfLocateLogEvent& aSelfLocateLogSideA, const CLbsSelfLocateLogEvent& aSelfLocateLogSideB)
{
	//Compare base class member variables
	if(!Compare_LogBase(aSelfLocateLogSideA, aSelfLocateLogSideB))
		return EFalse;
	
	if(aSelfLocateLogSideA.LocalApp() != aSelfLocateLogSideB.LocalApp())
		return EFalse;
	
	return ETrue;
}

/**
 * Method for comparing NetworkLocateLog
 */
EXPORT_C TBool T_LbsUtils::Compare_NetworkLocateLog(const CLbsNetworkLocateLogEvent& aNetworkLocateLogSideA, const CLbsNetworkLocateLogEvent& aNetworkLocateLogSideB)
{
	//Compare base class member variables
	if(!Compare_LogBase(aNetworkLocateLogSideA, aNetworkLocateLogSideB))
		return EFalse;
	
	if(aNetworkLocateLogSideA.LocalApp() != aNetworkLocateLogSideB.LocalApp())
		return EFalse;
	
	return ETrue;
}

/**
 * Method for comparing ExternalLocateLog
 */
EXPORT_C TBool T_LbsUtils::Compare_ExternalLocateLog(const CLbsExternalLocateLogEvent& aExternalLocateLogSideA, const CLbsExternalLocateLogEvent& aExternalLocateLogSideB)
{
	//Compare base class member variables
	if(!Compare_LogBase(aExternalLocateLogSideA, aExternalLocateLogSideB))
		return EFalse;
	
	if(!Compare_PosInfo(*aExternalLocateLogSideA.ReferenceLocation(), *aExternalLocateLogSideB.ReferenceLocation()))
			return EFalse;				
	
	if(aExternalLocateLogSideA.RequestOriginator() != aExternalLocateLogSideB.RequestOriginator())
		return EFalse;
	
	if(!Compare_ExternalRequestPrivacyRequestParams(aExternalLocateLogSideA.PrivacyRequestParams(), aExternalLocateLogSideB.PrivacyRequestParams()))
		return EFalse;
	
	if(!Compare_ExternalRequestPrivacyResponseParams(aExternalLocateLogSideA.PrivacyResponseParams(), aExternalLocateLogSideB.PrivacyResponseParams()))
		return EFalse;
	
return ETrue;
}



/**
 * Method for comparing X3PLog
 */
EXPORT_C TBool T_LbsUtils::Compare_TransmitLocationLog(const CLbsTransmitLocationLogEvent& aX3PLogSideA, const CLbsTransmitLocationLogEvent& aX3PLogSideB)
{
	//Compare base class member variables
	if(!Compare_LogBase(aX3PLogSideA, aX3PLogSideB))
		return EFalse;
	
	if(aX3PLogSideA.X3PApp() != aX3PLogSideB.X3PApp())
		return EFalse;
	
	if(!Compare_Destinations(aX3PLogSideA.Destination(), aX3PLogSideB.Destination()))
		return EFalse;
	
	return ETrue;
}


/**
 * Method for comparing AssistanceDataLog
 */
EXPORT_C TBool T_LbsUtils::Compare_AssistanceDataLog(const CLbsAssistanceDataLogEvent& aAssistanceDataLogSideA, const CLbsAssistanceDataLogEvent& aAssistanceDataLogSideB)
{
	//Compare base class member variables
	if(!Compare_LogBase(aAssistanceDataLogSideA, aAssistanceDataLogSideB))
		return EFalse;
	
	if(aAssistanceDataLogSideA.RequestAssistanceDataType() != aAssistanceDataLogSideB.RequestAssistanceDataType())
		return EFalse;
	

	if(aAssistanceDataLogSideA.ReceiveAssistanceDataType() != aAssistanceDataLogSideB.ReceiveAssistanceDataType())
		return EFalse;	
	
	return ETrue;
}


/**
 * Method needed to compare external locate logs.
 */
TBool T_LbsUtils::Compare_ExternalRequestPrivacyRequestParams(const TLbsLoggingPrivacyRequestParams& aLogPrivacyRequestParamsSideA, const TLbsLoggingPrivacyRequestParams& aLogPrivacyRequestParamsSideB)
{
	if(aLogPrivacyRequestParamsSideA.iSessionId != aLogPrivacyRequestParamsSideB.iSessionId)
		return EFalse;
	
	if(!Compare_ExternalRequestInfo(aLogPrivacyRequestParamsSideA.iRequestInfo, aLogPrivacyRequestParamsSideB.iRequestInfo))
		return EFalse;
	
	if(!Compare_ExternalRequestPrivacy(aLogPrivacyRequestParamsSideA.iRequestPrivacy, aLogPrivacyRequestParamsSideB.iRequestPrivacy))
		return EFalse;
	
	
	
	return ETrue;
}


/**
 * Method needed to compare external locate logs.
 * Compares TLbsExternalRequestInfo class.
 */
TBool T_LbsUtils::Compare_ExternalRequestInfo(const TLbsExternalRequestInfo& aExternalRequestInfoSideA, const TLbsExternalRequestInfo& aExternalRequestInfoSideB)
{
	TLbsRequesterId requesterIdSideA, requesterIdSideB;
	TLbsClientName clientNameSideA, clientNameSideB;
	TLbsClientExternalId clientExternalIdSideA, clientExternalIdSideB;
	
	aExternalRequestInfoSideA.GetRequesterId(requesterIdSideA);
	aExternalRequestInfoSideA.GetClientName(clientNameSideA);
	aExternalRequestInfoSideA.GetClientExternalId(clientExternalIdSideA);	
	
	aExternalRequestInfoSideB.GetRequesterId(requesterIdSideB);
	aExternalRequestInfoSideB.GetClientName(clientNameSideB);
	aExternalRequestInfoSideB.GetClientExternalId(clientExternalIdSideB);	
	
	// If these values are different, then return false
	if(requesterIdSideA.Compare(requesterIdSideB))	// Should return 0 if equal
		return EFalse;
	
	if(clientNameSideA.Compare(clientNameSideB))
		return EFalse;
		
	if(clientExternalIdSideA.Compare(clientExternalIdSideB))
		return EFalse;
	
	
	return ETrue;
}


/**
 * Method needed to compare external locate logs.
 * Compares TLbsNetPosRequestPrivacy class.
 */
TBool T_LbsUtils::Compare_ExternalRequestPrivacy(const TLbsNetPosRequestPrivacy& aExternalRequestPrivacySideA, const TLbsNetPosRequestPrivacy& aExternalRequestPrivacySideB)
{
	if(aExternalRequestPrivacySideA.RequestAdvice() != aExternalRequestPrivacySideB.RequestAdvice())
		return EFalse;
	
	if(aExternalRequestPrivacySideA.RequestAction() != aExternalRequestPrivacySideB.RequestAction())
		return EFalse;
	
	return ETrue;
}


/**
 * Method needed to compare external locate logs.
 * Compares TPrivacyResponseParams class.
 */
TBool T_LbsUtils::Compare_ExternalRequestPrivacyResponseParams(const TLbsLoggingPrivacyResponseParams& aExternalRequestPrivacyResponseSideA, const TLbsLoggingPrivacyResponseParams& aExternalRequestPrivacyResponseSideB)
{
	if(aExternalRequestPrivacyResponseSideA.iSessionId != aExternalRequestPrivacyResponseSideB.iSessionId)
		return EFalse;
	
	if(aExternalRequestPrivacyResponseSideA.iResult != aExternalRequestPrivacyResponseSideB.iResult)
		return EFalse;
		
	return ETrue;
}

/**
 * Method needed to Transmit to 3rd party destinations
 * Compares TDesC16.
 */
TBool T_LbsUtils::Compare_Destinations(TDesC16& aDestinationFromRealLog, TDesC16& aDestinationFromExpectedLog)
	{
	if(aDestinationFromRealLog != aDestinationFromExpectedLog)
		return EFalse;
	
	return ETrue;
	}
	

