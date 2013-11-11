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
// Declarations for methods that create default various packet context related structures
// 
//

/**
 @file
 @internalComponent
*/

#ifndef DEFAULT_CONTEXT_PARAMETERS
#define DEFAULT_CONTEXT_PARAMETERS

#include <networking/umtsnifcontrolif.h>
#include <etelpckt.h>

TName& SimTsyName();

void DefaultContextConfigGPRS(RPacketContext::TContextConfigGPRS& aParam);
TInt DefaultContextConfig_R5(RPacketContext::TContextConfig_R5& aParam);
void DefaultContextConfigR99_R4(RPacketContext::TContextConfigR99_R4& aParam);

void DefaultQoSR99_R4Requested(RPacketQoS::TQoSR99_R4Requested& aQosRequested);
void SecondQoSR99_R4Requested(RPacketQoS::TQoSR99_R4Requested& aQosRequested);

#ifdef SYMBIAN_NETWORKING_UMTSR5
void DefaultQoSR5Requested(RPacketQoS::TQoSR5Requested& aQosRequested);
void SecondQoSR5Requested(RPacketQoS::TQoSR5Requested& aQosRequested);
void DefaultQoSR5Negotiated(RPacketQoS::TQoSR5Negotiated& aQosNegotiated);
void SecondQoSR5Negotiated(RPacketQoS::TQoSR5Negotiated& aQosNegotiated);
#endif

void DefaultQoSR99_R4Negotiated(RPacketQoS::TQoSR99_R4Negotiated& aQosNegotiated);
void SecondQoSR99_R4Negotiated(RPacketQoS::TQoSR99_R4Negotiated& aQosNegotiated);

void DefaultTftInfo(TTFTInfo& aTTFTInfo);
void SecondDefaultTftInfo(TTFTInfo& aTTFTInfo);
void CombinedDefaultTftInfo(TTFTInfo& aTTFTInfo);
void DefaultContextStatus(RPacketContext::TContextStatus& aStatus);
void DefaultServiceStatus(RPacketService::TStatus& aStatus);
void DefaultDataChannel(RPacketContext::TDataChannelV2& aDataChannel);

#endif
// DEFAULT_CONTEXT_PARAMETERS
