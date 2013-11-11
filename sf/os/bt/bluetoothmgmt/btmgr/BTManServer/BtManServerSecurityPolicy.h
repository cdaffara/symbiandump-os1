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
//

/**
 @file
 @internalTechnology
*/
#ifndef __btmanserversecuritypolicy_h__
#define __btmanserversecuritypolicy_h__

const TUint KBtmanServerRangeCount = 17;

/**
N.B.
1.) Functions labelled 'Always fail' are not supported by BTManServer currently.
2.) Functions labelled 'Always pass' can only be issued once a valid sessionhandle is obtained
    (EBTManCreateRegistrySubSession), in which case we know that the client has the 
    LocalServices capability so we do not have to check capabilties again.
3.) All other Functions are labelled with the capability that is required to execute them.
**/

const TInt KBtmanServerRanges[KBtmanServerRangeCount] = 
	{
	EBTManCreateHostResolverSubSession,	//	0	=	Alway fail
	EBTManCreateRegistrySubSession,		//	1	=	Local Services
	EBTManRegistrySearch,				//	2	=	Always pass
	EBTRegistryRetrieveDevice,			//	3	=	Always fail
	EBTRegistryAddDevice,				//	4	=	Always pass
	EBTRegistryModifyBluetoothName,		//	5	=	Write Device Data
	EBTRegistryModifyFriendlyName,		//	6	=	Always pass
	EBTRegistryModifyNamelessDevice,	//	7	=	Write Device Data
	EBTRegistryDeleteDevices,			//	8	=	Always pass
	EBTRegistryDeleteLinkKey,			//	9	=	Write Device Data
	EBTRegistryCloseView,				//	10	=	Always pass
	EBTRegistryUpdateLocalDevice,		//	11	=	Write Device Data
	EBTRegistryGetCommPortSettings,		//	12	=	Always pass
	EBTHostResolverDeviceRequest,		//	13	=	Always fail
	EBTManSetHeapFailure,				//	14	=	Local Services
	EBTRegistryNotifyViewChange,		//	15	=	Always pass
	//	If new function enums are added, the following value needs to
	//	increase to be <last function number> + 1.
	//	This defines a final range that extends from the first invalid
	//	function number to KMaxTInt, and protects against attempts to
	//	attack the server with invalid functions.
	EBTRegistryNotifyViewChange + 1		//  16	=	CPolicyServer::ENotSupported for all other fns.
	};



/** Index numbers into KBtmanServerElements[] */
const TInt policyAlwaysFail 					=	0;
const TInt policyLocalServices 					=	1;
const TInt policyAlwaysPass						=	2;
const TInt policyWriteDeviceData				=	3;

/**Mapping IPCs to policy element */
const TUint8 KBtmanServerElementsIndex[KBtmanServerRangeCount] = 
	{
	policyAlwaysFail,				/**	EBTManCreateHostResolverSubSession	*/
	policyLocalServices,			/**	EBTManCreateRegistrySubSession	*/
									/**	EBTManCreateCommPortSettingsSubSession	*/
									/**	EBTManCreateLocalDeviceSubSession	*/
	policyAlwaysPass,				/**	EBTManRegistrySearch	*/
									/**	EBTManExtractRegistryDataIntoServer	*/
									/**	EBTManRetrieveRegistryData	*/
									/**	EBTManCloseSubSession	*/
									/**	EBTManCancelRequest	*/
	policyAlwaysFail,				/**	EBTRegistryRetrieveDevice	*/
	policyAlwaysPass,				/**	EBTRegistryAddDevice	*/
									/**	EBTRegistryGetNamelessDevice	*/
	policyWriteDeviceData,			/**	EBTRegistryModifyBluetoothName	*/
	policyAlwaysPass,				/**	EBTRegistryModifyFriendlyName	*/
	policyWriteDeviceData,			/**	EBTRegistryModifyNamelessDevice	*/
	policyAlwaysPass,				/**	EBTRegistryDeleteDevices	*/
	policyWriteDeviceData,			/**	EBTRegistryDeleteLinkKey	*/
									/**	EBTRegistryUnpairView	*/
	policyAlwaysPass,				/**	EBTRegistryCloseView	*/
									/**	EBTRegistryGetLocalDevice	*/
	policyWriteDeviceData,			/**	EBTRegistryUpdateLocalDevice	*/
	policyAlwaysPass,				/**	EBTRegistryGetCommPortSettings	*/
									/**	EBTRegistryUpdateCommPortSettings	*/
									/**	EBTRegistryDeleteCommPortSettings	*/
	policyAlwaysFail,				/**	EBTHostResolverDeviceRequest	*/
									/**	EBTHostResolverGetNextDeviceRequest	*/
									/**	EBTHostResolverDeviceModifyDevice	*/
									/**	EBTHostResolverNotifyRequest	*/
	policyLocalServices,			/**	EBTManSetHeapFailure	*/
									/**	EBTManSubSessionCount	*/
	policyAlwaysPass,               /** EBTRegistryNotifyViewChange */
	CPolicyServer::ENotSupported	/**	EBTManCreateSecManSubSession	*/
									/**	EBTManCreateSecuritySettingsSubSession	*/
									/**	EBTManCreateBasebandSecuritySubSession	*/
									/**	EBTSecManAccessRequest	*/
									/**	EBTSecuritySettingsRegister	*/
									/**	EBTSecuritySettingsUnregister	*/
									/**	EBTBasebandSecurityLinkKeyRequest	*/
									/**	EBTBasebandSecurityNewLinkKey	*/
									/**	EBTBasebandSecurityPinRequest	*/
									/**	EBTBasebandSecurityNewLinkState	*/
									/**	EBTBasebandSecurityHCIRequestHandler	*/
									/**	EBTBasebandSecurityNameRequestResponse	*/
									/**	...and onwards to KMaxTInt	*/
	};

/** Individual policy elements */
const CPolicyServer::TPolicyElement KBtmanServerElements[] = 
	{
	/** the EFailClient means that the if the check fails the CheckFailed method with return KErrPermissionDenied */
		{	_INIT_SECURITY_POLICY_FAIL },	/**	policyAlwaysFail	*/
		{	_INIT_SECURITY_POLICY_C1(ECapabilityLocalServices), CPolicyServer::EFailClient },	/**	policyLocalService	*/
	  	{	_INIT_SECURITY_POLICY_PASS },	/**	policyAlwaysPass	*/
	  	{	_INIT_SECURITY_POLICY_C1(ECapabilityWriteDeviceData), CPolicyServer::EFailClient }	/**	policyWriteDevData	*/
	};

/** Main policy */
const CPolicyServer::TPolicy KBtmanServerPolicy = 
{
CPolicyServer::EAlwaysPass, /** Specifies all connect attempts should pass */
KBtmanServerRangeCount,
KBtmanServerRanges,
KBtmanServerElementsIndex,
KBtmanServerElements,
};

#endif	//__btmanserversecuritypolicy_h__
