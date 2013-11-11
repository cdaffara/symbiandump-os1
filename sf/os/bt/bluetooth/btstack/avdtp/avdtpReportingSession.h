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
// avdtpReportingSession.cpp
// Implements the avdtp reporting transport session
// The reporting session is-a transport session as defined in the AVDTP specification
// It allows the conveyance of reporting information for a stream.
// This reporting information is typically encasulated in RTCP packets.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef AVDTPREPORTINGSESSION_H
#define AVDTPREPORTINGSESSION_H

#include "avdtpTransportSession.h"

class CTransportChannel;
class CAvdtpSAP;
class CAvdtpProtocol;
class RAvdtpMessage;

NONSHARABLE_CLASS(CReportingSession) : public CUserPlaneTransportSession
	{
public:
	static CReportingSession* NewLC(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP, CAVStream& aStream);
	static CReportingSession* NewL(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP, CAVStream& aStream);

	virtual TInt Send(RMBufChain& aData, TUint aOptions, TSockAddr* aAddr);

	virtual TInt DoActiveOpen();
	virtual void CanSend();
	
private:
	CReportingSession(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP, CAVStream& aStream);
	void ConstructL();


private:
	};
	
#endif //AVDTPREPORTINGSESSION_H
