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
// This file provides the implementation of the class that manages
// assistance data during location requests.
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/


#include <e32base.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancereferencelocation.h>
#include "suplassistancedatamgr.h"
#include "supldevloggermacros.h"

/** Constant to identify an empty set of assistance data.
*/
const TInt KAssistDataEmpty = 0;

/** KAssistDataReportTimer
Identity of timer
*/
const TInt KAssistDataReportTimer = 0x01;

/** KAssistDataReportTimerDurationInSec
Timer duration for when assistance data is expected from network
*/
const TInt KAssistDataReportTimerDurationInSec = 1;

/** Static constructor.
@return A new instance of the CSuplAssistanceDataManager class.
*/  
CSuplAssistanceDataManager* CSuplAssistanceDataManager::NewL(MSuplProtocolManagerObserver& aGateway)
	{
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::NewL() Begin\n");
	CSuplAssistanceDataManager* self = new (ELeave) CSuplAssistanceDataManager(aGateway);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::NewL() End\n");
	return self;
	}


/** Standard constructor.
*/  
CSuplAssistanceDataManager::CSuplAssistanceDataManager(MSuplProtocolManagerObserver& aGateway):
							iGateway(aGateway)
	{
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::CSuplAssistanceDataManager() Begin\n");
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::CSuplAssistanceDataManager() End\n");
	}


/** Standard destructor.
*/  
CSuplAssistanceDataManager::~CSuplAssistanceDataManager()
	{
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::~CSuplAssistanceDataManager() Begin\n");
	delete iAssistDataReportTimer;
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::~CSuplAssistanceDataManager() End\n");
	}


/** Private second-stage constructor.
*/  
void CSuplAssistanceDataManager::ConstructL()
	{
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::ConstructL() Begin\n");
	iAssistDataReportTimer = CLbsCallbackTimer::NewL(*this);
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::ConstructL() End\n");
	}


/** Process assistance data request.
This is called by the protocol manager to process an assistance data request
received from LBS.

The method first identifies any errors in the assistance data request mask. If any
assistance data types are unsupported, this method sends a response to LBS
for those types with error code KErrArgument.

Then, if there are any state machines ready to handle an assistance data request, the
iPendingDataMask mask is added to aDataRequestMask and then cleared. Otherwise, it is
aDataRequestMask that is added to iPendingDataMask.

The state machines that must receive the assitance data request are returned by this
method as a mask of type MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask.

@param aDataRequest The set of assistance data requested by LBS.
@return aDataRequest The set of assistance data that must be requested from the state machines
@return MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask the state machine that
		must receive the assistance data request.
*/
MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask CSuplAssistanceDataManager::ProcessDataRequest(TLbsAsistanceDataGroup& aDataRequestMask)
	{
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::ProcessDataRequest() Begin\n");

	// state machines ready for assistance data requests
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask readyMachinesMask = MLbsNetworkProtocolObserver::EServiceNone;

	// Identify invalid parts of the request.
	TLbsAsistanceDataGroup validSet = EAssistanceDataAquisitionAssistance
									 | EAssistanceDataBadSatList
									 | EAssistanceDataNavigationModel
									 | EAssistanceDataReferenceTime
									 | EAssistanceDataIonosphericModel
									 | EAssistanceDataReferenceLocation
									 | EAssistanceDataAlmanac
									 | EAssistanceDataPositioningGpsUtcModel;

	// Create a request error mask for any invalid parts of the request
	TLbsAsistanceDataGroup requestErrorMask = aDataRequestMask & ~validSet;

	if (requestErrorMask != 0)
		{
		// Send an error back to LBS for the assistance data types in the
		// error mask
		//
		RLbsAssistanceDataBuilderSet dummyAssistanceData; //ignored by LBS when error is not KErrNone
		iGateway.AssistanceDataInd(requestErrorMask, dummyAssistanceData, KErrArgument);
		}

	// Remove from the request mask any assistance data types reported to LBS
	// within the last second. Those need not be requested again. Remove also
	// invalid assitance data types
	aDataRequestMask = aDataRequestMask & ~iReportedDataMask & validSet;

	// See if there are any state machines ready to process assistance data requests
	// (they must get an assistance data request even if it is empty)
	if (iReadyMachinesMask != MLbsNetworkProtocolObserver::EServiceNone)
		{
		// Add pending assistance data to the request and clear the pending mask;
		aDataRequestMask = aDataRequestMask | iPendingDataMask;
		iPendingDataMask = KAssistDataEmpty;

		// The request will be forwarded to all the ready machines. After that
		// they are no longer ready for new assistance data requests (until they
		// reported as ready again with MachineReadyForAssistanceDataRequest())
		readyMachinesMask = iReadyMachinesMask;
		iReadyMachinesMask = MLbsNetworkProtocolObserver::EServiceNone;
		}
	else
		{
		// Add requested assistance data types to the pending ones
		iPendingDataMask = iPendingDataMask | aDataRequestMask;
		}

	// If any machine is going to get the assistance data request, remember what 
	// assistance data types are going to be retrieved by each state machine.
	if (readyMachinesMask != MLbsNetworkProtocolObserver::EServiceNone)
		{
		if (readyMachinesMask & MLbsNetworkProtocolObserver::EServiceSelfLocation)
			{
			iMoLrRequestedDataMask = aDataRequestMask;
			}
		
		if (readyMachinesMask & MLbsNetworkProtocolObserver::EServiceMobileTerminated)
			{
			iMtLrRequestedDataMask = aDataRequestMask;
			}
		}
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::ProcessDataRequest() End\n");
	return readyMachinesMask;
	}


/** Process Assistance Data Report

Called by the Protocol Manager when a state machine (aStateMachine) has reported that it
has obtained assistance data. The assistance data types reported in this call are those in
aDataReportMask. If the error code aReason is KErrNone, then the assistance data is be available
in aAssistanceDataBuilder.

The error in aReason applies to all of the assistance data types in aDataReportMask.

This method sends the Assistance Data (or error indication) to LBS.

If Reference Location is part of the reported set of assistance data, it is extracted,
converted to the format expected by LBS and sent to LBS.

@param aAssistanceData. The actual assistance data being reported when aReason is KErrNone
@param aDataReportMask. The assistanced data types being reported
@param aReason. If KErrNone, assistance data are being reported, otherwise, a failure to collect
				the assistance data types in aDataReportMask is what is being reported in this call.
@param aStateMachine. The service type of the state machine reporting the assistance data types.
					Used to identify the machine. @see MLbsNetworkProtocolObserver.
@param aSessionId. The unique ID of the session being conducted by the state machine that is reporting
					assistance data. Used when sendig reference location back to LBS.
*/
void CSuplAssistanceDataManager::AssistanceDataReport(const RLbsAssistanceDataBuilderSet& aAssistanceData,
						  const TLbsAsistanceDataGroup& aDataReportMask, TInt aReason,
						  const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aStateMachine,
						  const TLbsNetSessionId& aSessionId)
	{
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::AssistanceDataReport() Begin\n");

	TLbsAsistanceDataGroup missingDataTypes = KAssistDataEmpty;

	if (aReason != KErrNone)
		{
		// Errored assistance data should be reported to LBS.
		TLbsAsistanceDataGroup reportDataMask = KAssistDataEmpty;

		if (MLbsNetworkProtocolObserver::EServiceSelfLocation == aStateMachine)
			{
			// Report error on only those assistance data types that are not being
			// collected by any other state machines and have not been
			// reported recently
			reportDataMask = aDataReportMask & ~iMtLrRequestedDataMask & ~iReportedDataMask;
			iMoLrRequestedDataMask = KAssistDataEmpty;
			}
		else if (MLbsNetworkProtocolObserver::EServiceMobileTerminated == aStateMachine)
			{
			// Report error on only those assistance data types that are not being
			// collected by any other state machines and have not been
			// reported recently
			reportDataMask = aDataReportMask & ~iMoLrRequestedDataMask & ~iReportedDataMask;
			iMtLrRequestedDataMask = KAssistDataEmpty;
			}
		else
			{
			// This should not happen
			ASSERT(EFalse);
			}

		if (reportDataMask != KAssistDataEmpty)
			{
			iGateway.AssistanceDataInd(reportDataMask, aAssistanceData, KErrArgument);
			}
		}
	else
		{
		// Extract Reference Location from the reported assistance data if present and 
		// send it to LBS as a Location Update.
		//
		if (EAssistanceDataReferenceLocation == (aDataReportMask & EAssistanceDataReferenceLocation))
			{
			RReferenceLocationBuilder* refLocBuilderPtr;
			RLbsAssistanceDataBuilderSet& data = const_cast<RLbsAssistanceDataBuilderSet&>(aAssistanceData);
			if (KErrNone == data.GetDataBuilder(refLocBuilderPtr))
				{
 				if (refLocBuilderPtr->IsDataAvailable())
					{
					// Create a Ref Loc Reader with data from the Builder
		            RReferenceLocationReader refLocReader;
					TInt err = KErrNone;
					TRAP(err,refLocReader.OpenL());
					if (KErrNone == err)
						{
						refLocReader.DataBuffer() = refLocBuilderPtr->DataBuffer();

						// Obtain position info from the Ref Loc Reader and set it in a TPositionInfo
						if(refLocReader.FieldExists(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide))
							{
							REllipsoidPointAltitudeEllipsoideReader ellipsoidReader;
							refLocReader.GetFieldReader(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide, ellipsoidReader);
							TPositionInfo positionInfo;
							TPosition position;
							TUint tempUint;
							TInt  tempInt;
							TReal64 latitudeDegrees;
							TReal64 longitudeDegrees;
							TReal32 altitudeMeters;
							TReal aux = 0;
							TReal32 uncertainty;

							position.SetCurrentTime();
							// Get latitude and convert to degrees
							ellipsoidReader.GetField(TEllipsoidPointAltitudeEllipsoide::ELatitudeSign, tempInt);
							ellipsoidReader.GetField(TEllipsoidPointAltitudeEllipsoide::ELatitude, tempUint);
							latitudeDegrees = (tempUint * 90.0) / 8388608.0; //*90/2^23 ; ETSI TS 123.032
	                        latitudeDegrees *= (tempInt == TEllipsoidPointAltitudeEllipsoide::ESouth)? -1: 1;

							// Get longitude and convert to degreees
							ellipsoidReader.GetField(TEllipsoidPointAltitudeEllipsoide::ELongitude, tempInt);
							longitudeDegrees = (tempInt * 45.0) / 2097152.0; //*360/2^24; ETSI TS 123.032

							// Get altitude (meters) and apply sign
							ellipsoidReader.GetField(TEllipsoidPointAltitudeEllipsoide::EAltitudeDirection, tempInt);
							ellipsoidReader.GetField(TEllipsoidPointAltitudeEllipsoide::EAltitude, tempUint);
							altitudeMeters = tempUint; // ETSI TS 123.032
							altitudeMeters *= (tempInt == TEllipsoidPointAltitudeEllipsoide::EDepth)? -1: 1;
								
							position.SetCoordinate(latitudeDegrees, longitudeDegrees, altitudeMeters);

							// Get both horizontal uncertainties, convert to meters and set Horizontal Accuracy to the largest
							ellipsoidReader.GetField(TEllipsoidPointAltitudeEllipsoide::EUncertaintySemiMajor, tempUint);
							Math::Pow(aux, 1.1, tempUint);
						    uncertainty = 10*(aux - 1); // ETSI TS 123.032 : C(((1+x)^k) -1);  x = 0.1; C= 10
							ellipsoidReader.GetField(TEllipsoidPointAltitudeEllipsoide::EUncertaintySemiMinor, tempUint);
							Math::Pow(aux, 1.1, tempUint);
							uncertainty = uncertainty > (10*(aux - 1))? uncertainty : (10*(aux - 1));
							
							position.SetHorizontalAccuracy(uncertainty);

							// Get vertical uncertainty, convert to meters and set Vertical Accuracy
							ellipsoidReader.GetField(TEllipsoidPointAltitudeEllipsoide::EUncertaintyAltitude, tempUint);
							Math::Pow(aux,1.025, tempUint);
						    uncertainty = 45*(aux - 1); //ETSI TS 123.032; C(((1+x)^k) -1); C=45; x=0.025
							position.SetVerticalAccuracy(uncertainty);

							positionInfo.SetPosition(position);

							const TPositionModuleId id = {KSuplv1UidValue};
							positionInfo.SetModuleId(id);
							positionInfo.SetUpdateType(EPositionUpdateGeneral);
							positionInfo.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork);
							positionInfo.SetPositionModeReason(EPositionModeReasonNone);
							
							refLocReader.Close();
							// Send the position to LBS
							iGateway.NetworkLocationInd(aSessionId, positionInfo);
							}
						}
					}
				}

			}

		// Check if all of the assistance data types expected from the machine sending this
		// assistance data report are being reported. Send to LBS an error for the missing
		// assistance data types if they are not being collected from another machine.
		if (MLbsNetworkProtocolObserver::EServiceSelfLocation == aStateMachine)
			{
			if (aDataReportMask != iMoLrRequestedDataMask)
				{
				// Some data types are missing from the report. Pick those not being
				// taken care of by the MTLR state machine.
				missingDataTypes = iMoLrRequestedDataMask & ~aDataReportMask & ~iMtLrRequestedDataMask;
				}
				
			// No longer expecting assistance data from this machine
			iMoLrRequestedDataMask = KAssistDataEmpty;			
			}
		else if (MLbsNetworkProtocolObserver::EServiceMobileTerminated == aStateMachine)
			{		
			if (aDataReportMask != iMtLrRequestedDataMask)
				{
				// Some data types are missing from the report. Pick those not being
				// taken care of by the MOLR state machine.
				missingDataTypes = iMtLrRequestedDataMask & ~aDataReportMask & ~iMoLrRequestedDataMask;
				}
				
			// No longer expecting assistance data from this machine
			iMtLrRequestedDataMask = KAssistDataEmpty;
			}

		if (KAssistDataEmpty != missingDataTypes)
			{
			RLbsAssistanceDataBuilderSet dummyAssistanceData; //ignored by LBS when error is not KErrNone
			iGateway.AssistanceDataInd(missingDataTypes, dummyAssistanceData, KErrNotFound);
			}


		// Send received assistance data to LBS
		iGateway.AssistanceDataInd(aDataReportMask, aAssistanceData, KErrNone);

		// Remember this report for one second to guard agains this report
		// crossing in transit with a request from LBS which includes some
		// of the assitance data types being reported.
		iReportedDataMask = iReportedDataMask | aDataReportMask;
		if (iAssistDataReportTimer->IsActive())
			{
			iAssistDataReportTimer->Cancel();
			}
		iAssistDataReportTimer->EventAfter(TTimeIntervalSeconds(KAssistDataReportTimerDurationInSec), KAssistDataReportTimer);
		}
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::AssistanceDataReport() End\n");
	}

/** Set a machine as able to handle assistance data requests

This method is called by the Protocol Manager when it detects that a SUPL state machin
has sent a Location Request to LBS (which signals the point when it must/can received
an assistance data request)
*/
void CSuplAssistanceDataManager::MachineReadyForAssistanceDataRequest(const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aStateMachine)
	{
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::MachineReadyForAssistanceDataRequest() Begin\n");
	// Add this machine to the "ready" state machines mask
	iReadyMachinesMask = iReadyMachinesMask | aStateMachine;
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::MachineReadyForAssistanceDataRequest() End\n");
	}


/** Inform that a state machine is no longer active

This method is called by the Protocol Manager when it detects that a SUPL state machine
has terminated. If any assistance data were expected from this machine, an assistance data
report with KErrNotFound is sent to LBS for those assistance data types.
*/
void CSuplAssistanceDataManager::MachineTerminated(const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aStateMachine)
	{
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::MachineTerminated() Begin\n");
	TLbsAsistanceDataGroup orphanedDataRequestMask = KAssistDataEmpty;

	// Remove this machine from the "ready" state machines mask
	iReadyMachinesMask = iReadyMachinesMask &  ~aStateMachine;

	// If any assistance data was being collected from the machine (and not being collected from
	// another machine), a failure to collect those assistance data types must be reported to LBS 
	if ( (MLbsNetworkProtocolObserver::EServiceSelfLocation == aStateMachine)
		&& (KAssistDataEmpty != iMoLrRequestedDataMask))
		{
		orphanedDataRequestMask = iMoLrRequestedDataMask & ~iMtLrRequestedDataMask;
		}
	else if ( (MLbsNetworkProtocolObserver::EServiceMobileTerminated == aStateMachine)
			&& (KAssistDataEmpty != iMtLrRequestedDataMask))
		{
		orphanedDataRequestMask = iMtLrRequestedDataMask & ~iMoLrRequestedDataMask;
		}

	// Add the pending assistance data to the orphaned mask and clear the pending mask
	orphanedDataRequestMask = orphanedDataRequestMask | iPendingDataMask;
	iPendingDataMask = KAssistDataEmpty;
	
	if (KAssistDataEmpty != orphanedDataRequestMask)
		{
		RLbsAssistanceDataBuilderSet dummyAssistanceData; //ignored by LBS when error is not KErrNone
		iGateway.AssistanceDataInd(orphanedDataRequestMask, dummyAssistanceData, KErrNotFound);
		}
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::MachineTerminated() End\n");
	}

/** Timer expired callback.
This is used for clearing the reportedassistance
data mask one second after sending the assistance
data to LBS.
@param aTimerId The timer event identifier.
*/
void CSuplAssistanceDataManager::OnTimerEventL(TInt aTimerId)
	{
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::OnTimerEventL() Begin\n");
	// Perform relevant action for the expired timer
	switch (aTimerId)
		{
		// Additional assistance data timer
		case KAssistDataReportTimer:
			iReportedDataMask = KAssistDataEmpty;
			break;

		// Ignore unknown timer events
		default:
			break;
		};
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::OnTimerEventL() End\n");
	}

/** Timer callback error handler.
This is called if the timer expiry callback leaves.

@param aTimerId The timer event identifier.
@param aError Error value.
*/
TInt CSuplAssistanceDataManager::OnTimerError(TInt /*aTimerId*/, TInt aError)
	{
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::OnTimerError() Begin\n");
	SUPLLOG(ELogP1, "CSuplAssistanceDataManager::OnTimerError() End\n");
	return aError;
	}
