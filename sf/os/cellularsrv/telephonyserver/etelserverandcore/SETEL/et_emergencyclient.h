// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__ET_EMERGENCYCLIENT_H__)
#define _ET_EMERGENCYCLIENT_H__

#if !defined(__ET_PHONE_H)
#include "ET_PHONE.H"
#endif

// Number of clients who can call RPhone::SetEmergencyClient API
const TInt KNumberofEmergencyClients=5;

// Min and Max heap size for Emergency circuit switched voice call client
const TInt KEmergencyCSVoiceCallClientHeapMinSize=sizeof(CReqEntry)+sizeof(CBuffer)+0x200;
const TInt KEmergencyCSVoiceCallClientHeapMaxSize=2*KEmergencyCSVoiceCallClientHeapMinSize;


// Min and Max heap size for Emergency Location services client

// NotifyMtlr, NotifyMeasurementControl, SendMtlrResponse and SendMeasurementReport are the APIs which are going to use the
// pre-allocated heap memory. For pre-allocation of heap memory, the data sizes of these APIs are considered.
// NotifyMtlr API occupies 0x278*0x2 bytes (0x2 is the allowed numberofslots which the TSY can return to the ETel server for repost)
// NotifyMeasurementControl API occupies 0x5B0*0x2 bytes(0x2 is the allowed numberofslots which the TSY can return to the ETel server for repost)
// SendMtlrResponse occupies 0x4 bytes
// SendMeasurementReport API occupies 0x25C bytes
// As a precaution 0x21C bytes is also considered,which is 10% of the sum total bytes occupied by the APIs and data sizes of
// CBuffer, CReqEntry and HETelBufC8

/**
The formula used is as follows : 

((sizeof(TNotifyMtlrV7) * 2) + (sizeof(CBuffer) + sizeof(CReqEntry) + sizeof(HEtelBufC8))) + 
((sizeof(TMeasurementControlV7) * 2) + (sizeof(CBuffer) + sizeof(CReqEntry) + sizeof(HEtelBufC8))) + 
(sizeof(TMtlrResponseV7) + (sizeof(CBuffer) + sizeof(CReqEntry) + sizeof(HEtelBufC8))) + 
(sizeof(TMeasurementReportV7) + (sizeof(CBuffer) + sizeof(CReqEntry) + sizeof(HEtelBufC8))) +
10% of total size
*/
const TInt KEmergencyLCSClientHeapMinSize=((0x278*0x2) + (sizeof(CBuffer)+sizeof(CReqEntry)+sizeof(HEtelBufC8))) +
										  ((0x5B0*0x2) + (sizeof(CBuffer)+sizeof(CReqEntry)+sizeof(HEtelBufC8))) +
										  ((0x4) + (sizeof(CBuffer)+sizeof(CReqEntry)+sizeof(HEtelBufC8))) +
										  ((0x25C) + (sizeof(CBuffer)+sizeof(CReqEntry)+sizeof(HEtelBufC8))) + 0x21C;

const TInt KEmergencyLCSClientHeapMaxSize=2*KEmergencyLCSClientHeapMinSize;

#endif



