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
// etel driver namespace
// 
//

/**
 @file 
 @internalComponent
*/

#ifndef ETELDRIVERNMSPACE_H
#define ETELDRIVERNMSPACE_H

namespace EtelDriver
{

/** input operations, which are handled by etel driver input */
enum TEtelInput
	{
	EOpenPhone,
	
	ESetQoS,
	ESetTft,
	EChangeTft,
	ECreate1ryPdpContext,
	ECreate2ryPdpContext,
	ECreateMbmsPdpContext,
	ESessionUpdate,

	EActivatePdp,
	ENegQoSGet,
	EModifyActive,
	EContextDelete,
	EActivateMbmsPdp,
	
	ESentinelInput
	};

/** input operations, which are handled by etel driver input */
enum TEtelDriverStrategy
	{
	EOpenPhoneStrategy = 0,
	
	ESetQoSStrategy,
	ESetTftStrategy,
	EChangeTftStrategy,
	
	ECreate1ryPdpContextStrategy,
	ECreate2ryPdpContextStrategy,
	ECreateMbmsPdpContextStrategy,
	ESessionUpdateStrategy,
	
	EActivateMbmsPdpStrategy,
	EActivatePdpStrategy,
	EGetNegQoSStrategy,
	EModifyActiveStrategy,
	EContextDeleteStrategy,

	ESentinelStrategy
	};

	typedef TPckg<RPacketQoS::TQoSR99_R4Requested>  	TQoSR99_R4RequestedPckg;
	typedef TPckg<RPacketQoS::TQoSR99_R4Negotiated>  	TQoSR99_R4NegotiatedPckg;

#ifdef SYMBIAN_NETWORKING_UMTSR5 
	typedef TPckg<RPacketQoS::TQoSR5Requested>  	TQoSR5RequestedPckg;
	typedef TPckg<RPacketQoS::TQoSR5Negotiated>  	TQoSR5NegotiatedPckg;
#endif 
// SYMBIAN_NETWORKING_UMTSR5 

	typedef TPckg<RPacketContext::TContextConfigGPRS>  	TContextConfigGPRSPckg;
	typedef TPckg<RPacketContext::TPacketFilterV2>		TPacketFilterV2Pckg;
	typedef TPckg<RPacketMbmsContext::TContextConfigMbmsV1>   TContextConfigMbmsV1Pckg;
	

_LIT(KTxtSpudTel, "SPUDTEL");

} // namespace EtelDriver

	
#endif // ETELDRIVERNMSPACE_H
