// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LBSERRORS_H__
#define __LBSERRORS_H__

#include <e32def.h>

/**
LBS Panic string

@publishedAll
@released
 */
_LIT(KPosClientFault,"Lbs Client Fault");

/**
Position Client API panic codes

@publishedAll
@released
 */
enum TPositionClientPanic
	{
	/** HPGI datatype mismatch */
	EPositionGenericInfoMismatchDataType		= 0,
	/** CRequestor constructed with a null descriptor */
	EPositionRequestorConstructorGivenNullData	= 1,
	/** HPGI zero-byte buffer */
	EPositionGenericInfoZeroBufferSize			= 2,
	/** A unknown IPC request has been made on the server. */
	EPositionBadRequest							= 3,
	/** A bad descriptor has been given to the server by the client. */
	EPositionBadDescriptor						= 4,
	/** An attempt has been made to connect using an already open handle. */
	EPositionServerHandleNotClosed				= 5,
	/** An attempt has been made to communicate though a non connected handle. */
	EPositionServerBadHandle					= 6,
	/** A client has tried to use a RPositioner/RPositionServer handle that hasn't
	    been properly initialised. */
	EPositionNullPointerHolder					= 7,
	/** A client has asked for/set the supported classes in TPositionModuleInfo
	    with an invalid class type. */
	EPositionInvalidClassType					= 8,
	/** Too many items specified in a TPositionSelectionOrder. */
	EPositionSelectionOrderOverflow				= 9,
	/** The class' size doesn't match the expected value. */
	EPositionClassSizeMismatch					= 10,
	/** An unsupported object of size greater than KPositionMaxSizeQualityTrait has been
	passed to a TPositionQualityBase as a field. */
	EPositionQualityBadDataType					= 11,
	/** A negative TTimeIntervalMicroSeconds was passed to a method. */
	EPositionBadTime                            = 12,
	/** A negative value for accuracy was passed to a method. */
	EPositionBadAccuracy                        = 13,
	/** A client has attempted to instantiate a HPositionGenricInfo with 0 Max Fields. */
	EPositionGenericInfoZeroMaxFields			= 14,
	/** A client has called an asynchronous request while it is still outstanding
	on that RPositionServer / RPositioner. */
	EPositionDuplicateRequest                   = 15,
	/** A session or sub-session was closed with requests still outstanding. */
	EPositionRequestsNotCancelled               = 16,
	/** A NULL entry has been found in the requestor stack. */
	EPositionNullRequestor	                    = 17
	};


/**
Completion codes to give extended information about location fix.

@publishedAll
@released
 */
const TInt KPositionQualityLoss = 1;

/**
Completion code for partial update.

@publishedAll
@released
 */
const TInt KPositionPartialUpdate = 2;

/**
Completion code indicating that the request has returned early at the explicit request of the application.
@see RPositionerSubSessionBase::CompleteRequest()

@publishedAll
@released
 */
const TInt KPositionEarlyComplete = 3;

/**
Status code indicating that a location data source has determined it will be unable to
provide an update of the required quality. This is only reported by the data source 
module when it has reasoned that there is no point in continuing with the calculation.
It is not reported when there are transient conditions that might prevent the module
from obtaining the desired location information.

@publishedAll
@released
 */
const TInt KPositionCalculationFutile = 4;


/**
Code indicating that the assistance data response asks for a reset of the assistance data, as 
shown by the data mask.

@publishedAll
@released
 */
const TInt KPositionAssistanceDataReset = 5;


/**
Code indicating that a position is not currently available. The status code is used 
as part of the status notification of the LbsLocator API.

@publishedAll
@released
 */
const TInt KPositionNotAvailable = 6;

/**
Lbs specific error codes.

@publishedAll
@released

If __S60_ is defined in lbsvariant.h the constant is -30371 otherwise it is -17350
 */
const TInt KPositionErrBase = -30371;

/**
BufferOverflow error

@publishedAll
@released
 */
const TInt KErrPositionBufferOverflow = KPositionErrBase;

/**
Position  calculation error

@publishedAll
@released
 */
const TInt KErrPositionIncalculable = KPositionErrBase - 1;

/**
Network error

@publishedAll
@released
 */
const TInt KPositionNetworkErrorBase = -18301;

/**
Returned by the network, when it cannot perform an operation due to a failure in the network

@publishedAll
@released
 */
const TInt KErrPositionSystemFailure				= KPositionNetworkErrorBase - 0; //-18301

/**
Returned due to an protocol related error.

@publishedAll
@released
 */
const TInt KErrPositionProtocolErr					= KPositionNetworkErrorBase - 1; //-18302

/**
@publishedAll
@released
Returned by the network when the network is unable
(a) To obtain the requested location information or
(b) The desired QoS could not be satisfied or
(c) The requested LCS assistance data could not be delivered or
(d) The requested deciphering keys for broadcast assistance data could not be returned.
 */
const TInt KErrPositionMethodFailure				= KPositionNetworkErrorBase - 2; //-18303

/**
Returned by the network receiving a request about a facility which is not supported in the PLMN.

@publishedAll
@released
*/
const TInt KErrPositionFacilityNoSupport			= KPositionNetworkErrorBase - 3; //-18304

/**
@publishedAll
@released
Returned by the network when it is requested to perform an operation on a supplementary
service that violated the subscription restrictions.
The nature of the restriction or the transgressed options may be sent as parameters.
 */
const TInt KErrPositionNoService						= KPositionNetworkErrorBase - 4; //-18305

/**
@publishedAll
@released
Returned by the network when authorization failed for a informed LCSClientID. 
 */
const TInt KErrPositionUnauthorizedLCSClient		= KPositionNetworkErrorBase - 5; //-18306

/**
@publishedAll
@released
Returned by the network when an informed LCSClient is not available.
 */
const TInt KErrPositionServiceNotSubscribed		= KPositionNetworkErrorBase - 6; //-18307

/**
@publishedAll
@released
Returned when a requests is cancelled due to receiving higher priority request.
 */
const TInt KErrPositionHighPriorityReceive		= KPositionNetworkErrorBase - 7; //-18308


/**
Indicates an error due to no network coverage.

@publishedAll
@released
 */
const TInt KErrPositionNetworkUnavailable					= KPositionNetworkErrorBase - 8; //-18309


/**
Data source errors

@publishedAll
@released
 */
const TInt KPositionDataSourceErrorBase = -18350;


#endif //__LBSERRORS_H__
