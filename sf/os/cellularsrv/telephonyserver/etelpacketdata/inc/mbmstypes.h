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
// Common MBMS Broadcast Variables
// 
//



/**
 @file
 @publishedAll
 @released
*/

#ifndef _MBMSTYPES_H_
#define _MBMSTYPES_H_

/**
 * This name will be used by the TSY to identify the request for MBMS context
 *
 * @publishedAll
 * @released
 */
_LIT(KMBMSContextName,"MBMS_CONTEXT");

/**
 * Holds the list of MBMS sessions identifiers
 */
typedef RArray<TUint> TMbmsSessionIdList;

/**
 * Holds the MBMS session identifier
 */
typedef TUint TMbmsSessionId;

class TTmgi 
/** 
 * Holds the Temporary mobile group identity information
 * @see section 15.2 of 3GPP TS 23.007
 */
	{
public:
	inline TUint GetServiceId() const	{ return iServiceID; }
	inline void SetServiceId(TUint aServiceID)	{ iServiceID = aServiceID; }
	inline TUint16 GetMCC() const	{ return iMCC; }
	inline void SetMCC(TUint16 aMCC) 	{ iMCC = aMCC; }
	inline TUint16 GetMNC() const { return iMNC; }
	inline void SetMNC(TUint16 aMNC)	{ iMNC=aMNC; }

private:
	/** MBMS Service ID uniquely identifies an MBMS bearer service within a PLMN. */	
	TUint iServiceID;
	/** Mobile Country Code Information */	
	TUint16 iMCC;
	/** Mobile Network Code Information */	
	TUint16 iMNC;
	};
	
/** 
 * Holds the MBMS Access Bearer Information. 
 */
enum TMbmsScope
	{
	/** MBMS Bearer is 2G. */	
	E2GBearer,
	/** MBMS Bearer is 3G. */	
	E3GBearer			
	};

	
/** 
 * Holds the MBMS Service priority Information
 */
enum TMbmsServicePriority
	{
	/** Emergency MBMS service priority. */	
	EMbmsEmergencyServicePriority, 
	/** High MBMS service priority. */
	EMbmsHighServicePriority,
	/** Medium MBMS service priority. */
	EMbmsMediumServicePriority,
	/** Lowest MBMS service priority. */
	EMbmsLowServicePriority
	};
	
/** 
 * Holds the MBMS Service mode Information
 */
enum TMbmsServiceMode
	{
	/** MBMS broadcast service mode. */
	KBroadcast = 0x01,
	/** MBMS Selected Broadcast mode. */
	KSelectedBroadcast = 0x02
	};
	
/** 
 * Holds the MBMS service availability status Information 
 */
enum TMbmsAvailabilityStatus
	{
	/** MBMS availability status is unknown. */
	EMbmsAvailabilityUnknown,
	/** MBMS availability status is unavailable. */
	EMbmsUnavailable,
	/** MBMS availability status is available. */
	EMbmsAvailable
	};

/** 
 * Holds the MBMS Network service status Information 
 */
enum TMbmsNetworkServiceStatus
	{
	/** MBMS Network status is unknown. */
	EMbmsSupportUnknown,
	/** MBMS Network status is supported. */
	EMbmsSupported,
	/** MBMS Network status is not supported. */
	EMbmsNotSupported
	};

/** 
 * Holds the Different MBMS Action Information 
 */
enum TMbmsAction
	{
	/** Add the mbms entries. */ 
	EAddEntries,
	/** Removes the mbms entries. */ 
	ERemoveEntries,
	/** Removes all the mbms entries. */ 
	ERemoveAllEntries
	};
	
#endif	
