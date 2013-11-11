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
// Declarations for methods that compare various packet context related structures
// 
//

/**
 @file
 @internalComponent
*/

#ifndef COMPARE_CONTEXT_PARAMETERS
#define COMPARE_CONTEXT_PARAMETERS

#include <networking/umtsnifcontrolif.h>
#include <etelpckt.h>

TBool CompareContextConfigGPRS(const RPacketContext::TContextConfigGPRS &aFirst, const RPacketContext::TContextConfigGPRS &aSecond);
TBool CompareContextParameters(const TContextParameters& aFirst, const TContextParameters& aSecond);
TBool CompareContextConfigR5(const RPacketContext::TContextConfig_R5 &aFirst, const RPacketContext::TContextConfig_R5 &aSecond);
TBool CompareContextConfigR99R4(const RPacketContext::TContextConfigR99_R4 &aFirst, const RPacketContext::TContextConfigR99_R4 &aSecond);

TBool ComparePacketFilterV2(const RPacketContext::TPacketFilterV2 &aFirst, const RPacketContext::TPacketFilterV2 &aSecond);
TBool CompareDataChannel(const RPacketContext::TDataChannelV2& aFirst, const RPacketContext::TDataChannelV2& aSecond);
TBool CompareTFTInfo(TTFTInfo& aTFTInfo, RPacketContext& aPacketContext);
TBool CompareContextInfo(const RPacketService& aPacketService, const TDesC& aContextName, RPacketContext::TContextStatus aContextStatus);

TBool CompareUMTSQoSNeg(const RPacketQoS::TQoSR99_R4Negotiated &aFirst, const RPacketQoS::TQoSR99_R4Negotiated &aSecond);
TBool CompareUMTSQoSNeg(const RPacketQoS::TQoSR99_R4Requested &aFirst, const RPacketQoS::TQoSR99_R4Requested &aSecond);

#ifdef SYMBIAN_NETWORKING_UMTSR5
TBool CompareUMTSQoSReq(const RPacketQoS::TQoSR5Requested &aFirst, const RPacketQoS::TQoSR5Requested &aSecond);
#endif

#endif
// COMPARE_CONTEXT_PARAMETERS
