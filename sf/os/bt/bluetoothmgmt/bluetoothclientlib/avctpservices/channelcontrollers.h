// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#ifndef CHANNELCONTROLLERS_H
#define CHANNELCONTROLLERS_H

#include <es_sock.h>
#include <avctpservices.h>

#include "avctpcommon.h"
#include "avctpremotedevices.h"
#include "avctpserviceutils.h"

using namespace SymbianAvctp;

/**
CBaseChController

@internalComponent
*/
NONSHARABLE_CLASS(CBaseChController) : public CActive
	{
public:
	~CBaseChController();
	virtual void AttachRequest(const TBTDevAddr& aBTDevice);
	virtual void DetachRequest(const TBTDevAddr& aBTDevice);
	virtual void RefuseAttach(const TBTDevAddr& aBTDevice);
	virtual void Listen();
	virtual void Listen(const TBTDevAddr& aBTDevice);
	virtual void AgreeAttachment(const TBTDevAddr& aBTDevice);
protected:
	CBaseChController(MControlChannelNotify& aNotify, RSocketServ& aSockServer, TControlIoctls aIoctl, SymbianAvctp::TPid aPid, TInt aChannel);
	void BaseConstructL();
	
	// From CActive
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	
	virtual void Ioctl();
	virtual void NotifyIoctlCompleted(TInt aError) = 0;
	virtual void NotifyIoctlError(TInt aError) = 0;
protected:
	static const TInt				KDefaultMtu = 335;
	MControlChannelNotify& 			iNotify;
	RSocketServ&					iSockServer;
	RSocket							iTransport;
	TControlIoctls 					iIoctl;
	TPckgBuf<TControlIoctlMessage>	iIoctlBuf;
	SymbianAvctp::TPid 				iPid;
	TInt							iChannel;
	};

/**
CPrimaryChannelController

@internalComponent
*/
NONSHARABLE_CLASS(CPrimaryChannelController) : public CBaseChController
	{
public:
	static CPrimaryChannelController* NewL(MControlChannelNotify& aNotify, RSocketServ& aSockServer, SymbianAvctp::TPid aPid);
	static CPrimaryChannelController* NewLC(MControlChannelNotify& aNotify, RSocketServ& aSockServer, SymbianAvctp::TPid aPid);
	~CPrimaryChannelController();
	
	// override CBaseChController methods
	void AttachRequest(const TBTDevAddr& aBTDevice);
	void DetachRequest(const TBTDevAddr& aBTDevice);
	void RefuseAttach(const TBTDevAddr& aBTDevice);
	void Listen();
	void AgreeAttachment(const TBTDevAddr& aBTDevice);
private:
	CPrimaryChannelController(MControlChannelNotify& aNotify, RSocketServ& aSockServer, SymbianAvctp::TPid aPid);
	void ConstructL();
	
	// implements CBaseChController pure methods
	void NotifyIoctlCompleted(TInt aError);
	void NotifyIoctlError(TInt aError);
	};

/**
CSecondaryChannelController

@internalComponent
*/
NONSHARABLE_CLASS(CSecondaryChannelController) : public CBaseChController
	{
public:
	static CSecondaryChannelController* NewL(MControlChannelNotify& aNotify, RSocketServ& aSockServer, SymbianAvctp::TPid aPid);
	static CSecondaryChannelController* NewLC(MControlChannelNotify& aNotify, RSocketServ& aSockServer, SymbianAvctp::TPid aPid);
	~CSecondaryChannelController();
	void AttachRequest(const TBTDevAddr& aBTDevice);
	void DetachRequest(const TBTDevAddr& aBTDevice);
	void RefuseAttach(const TBTDevAddr& aBTDevice);
	void AttachPassively(const TBTDevAddr& aBTDevice);
	void Listen();
	void AgreeAttachment(const TBTDevAddr& aBTDevice);
private:
	CSecondaryChannelController(MControlChannelNotify& aNotify, RSocketServ& aSockServer, SymbianAvctp::TPid aPid);
	void ConstructL();
	
	void NotifyIoctlCompleted(TInt aError);
	void NotifyIoctlError(TInt aError);
	};

#endif /*CHANNELCONTROLLERS_H*/
