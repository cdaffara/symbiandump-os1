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
// Defines the avdtp recovery transport session
// The recovery session is-a transport session as defined in the AVDTP specification
// It allows the conveyance of recovery information for a stream.
// This recovery information is typically encasulated in RTP-FEC packets.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef AVDTPRECOVERYSESSION_H
#define AVDTPRECOVERYSESSION_H

#include "avdtpTransportSession.h"

class CTransportChannel;
class CAvdtpSAP;
class CAvdtpProtocol;
class RAvdtpMessage;

NONSHARABLE_CLASS(CRecoverySession) : public CUserPlaneTransportSession
	{
public:
	static CRecoverySession* NewLC(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP, CAVStream& aStream);
	static CRecoverySession* NewL(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP, CAVStream& aStream);

	virtual TInt Send(RMBufChain& aData, TUint aOptions, TSockAddr* aAddr);

	virtual TInt DoActiveOpen();
	virtual void CanSend();

private:
	CRecoverySession(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP, CAVStream& aStream);
	void ConstructL();


private:
	};
	
#endif //AVDTPRECOVERYSESSION_H
