// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SDP Server Security Policy definitions for Platform security.
// 
//

/**
 @file 
 @internalComponent
*/
 
#if !defined(__SDPSERVERSECURITYPOLICY_H__)
#define __SDPSERVERSECURITYPOLICY_H__
/** SDP Server Security Policy Definition */
const TUint KSdpServerRangeCount = 3;

/** Ranges Note that if a message is added to the list of Enums the 
last range must be modifed to the last enum + 1*/
const TInt KSdpServerRanges[KSdpServerRangeCount] = 
	{
    ESdpCreateDatabaseSubSession, /** LocalService */
	ESdpCreateAgentSubSession,	/** none */	
	ESdpServerDbgFailNext + 1, /** fail (to KMaxTInt) */
	};

/** Index numbers into KSdpServerElements[] */
const TInt KPolicyLocalServices = 0;
const TInt KPolicyPass = 1;

/**Mapping IPCs to poicy element */
const TUint8 KSdpServerElementsIndex[KSdpServerRangeCount] = 
	{
   	KPolicyLocalServices, /**	ESdpCreateDatabaseSubSession */

   	KPolicyPass,  	/**ESdpCreateAgentSubSession */
					/**ESdpCloseSubSession*/
					/**ESdpResourceCountMarkStart*/
					/**ESdpResourceCountMarkEnd*/
					/**ESdpResourceCount*/
					/**ESdpCancelRequest*/
					/**ESdpServiceSearch*/
					/**ESdpAttributeSearch*/
					/**ESdpServiceAttributeSearch*/
					/**ESdpDatabaseCreateServiceRecord*/
					/**ESdpDatabaseUpdateServiceRecordHandle*/
					/**ESdpDatabaseUpdateAttribute*/
					/**ESdpDatabaseDeleteAttribute*/
					/**ESdpDatabaseDeleteServiceRecord*/
					/**ESdpServerDbgMarkHeap*/
					/**ESdpServerDbgCheckHeap*/
					/**ESdpServerDbgMarkEnd*/
					/**ESdpServerDbgFailNext*/
	CPolicyServer::ENotSupported /** ESdpDatabaseDeleteServiceRecord + 1 to KMaxTInt */		 
				
	};

/** Individual policy elements */
const CPolicyServer::TPolicyElement KSdpServerElements[] = 
	{
		/** the EFailClient means that the if the check fails the CheckFailed method with 
		return KErrPermissionDenied */
  		{ _INIT_SECURITY_POLICY_C1(ECapabilityLocalServices), CPolicyServer::EFailClient },
  		{ _INIT_SECURITY_POLICY_PASS },
	};

/** Main policy */
const CPolicyServer::TPolicy KSdpServerPolicy = 
	{
	KPolicyLocalServices, /** Specifies connect attempts require local services */
	KSdpServerRangeCount,
	KSdpServerRanges,
	KSdpServerElementsIndex,
	KSdpServerElements,

	};
#endif //__SDPSERVERSECURITYPOLICY_H__
