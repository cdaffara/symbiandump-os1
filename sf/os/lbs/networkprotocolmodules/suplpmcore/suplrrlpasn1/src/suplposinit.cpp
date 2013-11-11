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

/**
 @file
 @internalTechnology
 
*/

#include "ULP.h"
#include "suplposinit.h"
#include "supldevloggermacros.h" 

/**
Static factory constructor
*/
EXPORT_C CSuplPosInit* CSuplPosInit::NewL()
	{
	SUPLLOG(ELogP1, "CSuplPosInit::NewL() Begin\n");
	CSuplPosInit* self = new (ELeave) CSuplPosInit();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CSuplPosInit::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}
	
	
/** 
Constructor 
*/
CSuplPosInit::CSuplPosInit()
 : CSuplMessageBase::CSuplMessageBase(ESuplPosInit, ETrue)
	{
	}


/** 
Second stage constructor 
*/
void CSuplPosInit::ConstructL()
	{
	// call the base class ConstructL() to create data structures
	CSuplMessageBase::ConstructL();

	// local reference to context object
	OSCTXT* pctxt = iControl->getCtxtPtr();

	iData->message.t =  T_UlpMessage_msSUPLPOSINIT;
	iData->message.u.msSUPLPOSINIT = (ASN1T_SUPLPOSINIT*)rtxMemAllocZ(pctxt, sizeof(ASN1T_SUPLPOSINIT));
   	LeaveIfAllocErrorL();
	}
	

/** 
Destructor
*/
CSuplPosInit::~CSuplPosInit()
	{
	SUPLLOG(ELogP1, "CSuplPosInit::~CSuplPosInit() Begin\n");
	SUPLLOG(ELogP1, "CSuplPosInit::~CSuplPosInit() End\n");
	}
	

/** 
SetCapabilities()

Sets the capabilities parameter of the outgoing message

@param  aCapabilities, LBS capabilities
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CSuplPosInit::SetCapabilities(const TLbsNetPosCapabilities& aCapabilities)
	{
	__ASSERT_DEBUG(iData->message.u.msSUPLPOSINIT != NULL, User::Invariant());
	SUPLLOG(ELogP1, "CSuplPosInit::SetCapabilities() Begin\n");
	
	TInt err = PopulateSetCapabilities(aCapabilities, iData->message.u.msSUPLPOSINIT->sETCapabilities);

	SUPLLOG(ELogP1, "CSuplPosInit::SetCapabilities() End\n");
	return err;
	}


/** 
SetRequestedAssistanceData()

Sets the requested assistance data parameter 

@param  aDataReqMask, bitmask indicating requested assistance data components.
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CSuplPosInit::SetRequestedAssistanceData(const TLbsAsistanceDataGroup& aDataReqMask)
	{
	__ASSERT_DEBUG(iData->message.u.msSUPLPOSINIT != NULL, User::Invariant());
	SUPLLOG(ELogP1, "CSuplPosInit::SetRequestedAssistanceData() Begin\n");
	SUPLLOG2(ELogP1, "    - TLbsAsistanceDataGroup aDataReqMask  = 0x%08X\n", aDataReqMask);	\
	
	// pointer to message body
	ASN1T_SUPLPOSINIT* msgBody = iData->message.u.msSUPLPOSINIT;
	
	if (aDataReqMask == EAssistanceDataNone)
		{
		// no assistance data requested
		return KErrNone;
		}
	
	// indicate parameter is set
	msgBody->m.requestedAssistDataPresent = 1;
	
	if (aDataReqMask & EAssistanceDataAquisitionAssistance)
		{
		msgBody->requestedAssistData.acquisitionAssistanceRequested = ETrue;
		}

	if (aDataReqMask & EAssistanceDataBadSatList)
		{
		msgBody->requestedAssistData.realTimeIntegrityRequested = ETrue;
		}

	if (aDataReqMask & EAssistanceDataNavigationModel)
		{
		msgBody->requestedAssistData.navigationModelRequested = ETrue;

		// Navigation Model Data
		// Note that setting nSAT (number of satellites for which the SET has
		// ephemeris data available) to zero means that the SLP should ignore
		// values for gpsWeek and gpsToe. The TOE-Limit also becomes redundant.
		// Also means there is no need to include any additional satellite 
		// information. See OMA-TS-V1_0-20070615A p22-23 for further detail.
		msgBody->requestedAssistData.m.navigationModelDataPresent = 1;
		msgBody->requestedAssistData.navigationModelData.gpsWeek = 0;
		msgBody->requestedAssistData.navigationModelData.gpsToe = 0;
		msgBody->requestedAssistData.navigationModelData.nSAT = 0;
		msgBody->requestedAssistData.navigationModelData.toeLimit = 0;
		msgBody->requestedAssistData.navigationModelData.m.satInfoPresent = 0;
		}

	if (aDataReqMask & EAssistanceDataReferenceTime)
		{
		msgBody->requestedAssistData.referenceTimeRequested = ETrue;
   		}
   		
	if (aDataReqMask & EAssistanceDataIonosphericModel)
		{
		msgBody->requestedAssistData.ionosphericModelRequested = ETrue;
		}
   
	if (aDataReqMask & EAssistanceDataDgpsCorrections)
		{
		msgBody->requestedAssistData.dgpsCorrectionsRequested = ETrue;
		}
   
	if (aDataReqMask & EAssistanceDataReferenceLocation)
		{
		msgBody->requestedAssistData.referenceLocationRequested = ETrue;
		}
   
	if (aDataReqMask & EAssistanceDataAlmanac)
		{
	   msgBody->requestedAssistData.almanacRequested = ETrue;
		}

	if (aDataReqMask & EAssistanceDataPositioningGpsUtcModel)
		{
		msgBody->requestedAssistData.utcModelRequested = ETrue;
		}
	
	SUPLLOG(ELogP1, "CSuplPosInit::SetRequestedAssistanceData() End\n");
	return KErrNone;
	}


/**	
SetLocationId()

Set the location ID parameter 

Location ID - Describes the globally unique cell identification of the most current serving cell.
	Cell Info (m) The following cell IDs are supported:
		- GSM Cell Info
		- WCDMA Cell Info
		- CDMA Cell Info
	Status (m) Describes whether or not the cell info is:
		- Not Current, last known cell info
		- Current, the present cell info
		- Unknown (ie not known whether the cell id is current or not current)

Note: The Status parameter does NOT apply to WCDMA optional parameters 
(Frequency Info, Primary Scrambling Code and Measured Results List). Frequency
Info, Primary Scrambling Code and Measured Results List, if present, are always
considered to be correct for the current cell.

@param  aLocationId, describes the current location
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CSuplPosInit::SetLocationId(const CSuplLocationId& aLocationId)
	{
	SUPLLOG(ELogP1, "CSuplPosInit::SetLocationId() Begin\n");
	__ASSERT_DEBUG(iData->message.u.msSUPLPOSINIT != NULL, User::Invariant());
	
	TInt retval = PopulateLocationId(aLocationId, iData->message.u.msSUPLPOSINIT->locationId);
	SUPLLOG2(ELogP1, "CSuplPosInit::SetLocationId() End, retval = %d\n", retval);
	return retval;
	}


/**	
SetPosition()

Sets the Position parameter of the outgoing message (optional)

@param  aPosInfo, position as supplied by LBS subsystem
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CSuplPosInit::SetPosition(const TPositionInfoBase& aPosInfo)
	{
	SUPLLOG(ELogP1, "CSuplPosInit::SetPosition() Begin\n");
	__ASSERT_DEBUG(iData->message.u.msSUPLPOSINIT != NULL, User::Invariant());
	
	// flag that the optional position parameter is present
	iData->message.u.msSUPLPOSINIT->m.positionPresent = 1;

	TInt retval = PopulatePosition(aPosInfo, iData->message.u.msSUPLPOSINIT->position);
	SUPLLOG2(ELogP1, "CSuplPosInit::SetPosition() End, retval = %d\n", retval);
	return retval;
	}


/** 
SetVer()

Sets the Ver parameter, a hash of the received SUPL INIT message of the
outgoing message

@param aVer, pointer to buffer containing the hash of the SUPL INIT message
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CSuplPosInit::SetVer(const TDesC8& aVer)
	{
	SUPLLOG(ELogP1, "CSuplPosInit::SetVer() Begin\n");
	__ASSERT_DEBUG(iData->message.u.msSUPLPOSINIT != NULL, User::Invariant());
	
	ASN1T_SUPLPOSINIT& posInit = *iData->message.u.msSUPLPOSINIT;
	posInit.m.verPresent = 1;
	posInit.ver.numbits = 64;
	TPtr8 target(posInit.ver.data, 8);
	target.Copy(aVer);

	SUPLLOG(ELogP1, "CSuplPosInit::SetVer() End\n");
	return KErrNone;
	}

