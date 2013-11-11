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
// Defines the avdtp media transport session
// 
//

/**
 @file
 @internalComponent
*/

#ifndef AVDTPMEDIASESSION_H
#define AVDTPMEDIASESSION_H

#include "avdtpTransportSession.h"

class CTransportChannel;
class CAvdtpSAP;
class CAvdtpProtocol;

NONSHARABLE_CLASS(CMediaSession) : public CUserPlaneTransportSession
	{
public:
	static CMediaSession* NewLC(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP, CAVStream& aStream);
	static CMediaSession* NewL(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP, CAVStream& aStream);

	virtual TInt GetOption(TUint aLevel, TUint aName, TDes8& aOption) const;
	virtual void Ioctl(TUint aLevel, TUint aName,  const TDesC8* aOption);
	virtual TInt Send(RMBufChain& aData, TUint aOptions, TSockAddr* aAddr);
	virtual void CanSend();

	virtual TInt DoActiveOpen();

private:
	CMediaSession(CAvdtpProtocol& aProtocol, CAvdtpSAP& aSAP, CAVStream& aStream);
	void ConstructL();
	void DoSend(RMBufChain& aData);
	void ReceivePacketLost();
	
	TBool iNotifyMediaPacketDroppedFlag;
	TInt iPacketsLost; // Cumulative packets lost since last IOCTL
	};

#endif //AVDTPMEDIASESSION_H
