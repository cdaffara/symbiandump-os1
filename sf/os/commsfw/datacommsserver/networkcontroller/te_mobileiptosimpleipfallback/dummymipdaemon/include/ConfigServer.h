/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Defines the CConfigServer class which implements the basic 
* test configuration daemon server.
* 
*
*/



/**
 @file
 @internalComponent
*/

#ifndef __CONFIGSERVER_H__
#define __CONFIGSERVER_H__

#include <e32base.h>
#include <e32svr.h>
#include <es_sock.h>
#include "ExpireTimer.h"

class CConfigSession;
class CConfigServer : public CServer2, MExpireTimer
/**
 * The CConfigServer class
 *
 * Implements a Symbian OS server for the test configuration daemon.
 *
 * @internalComponent
 */
	{
public:
	static CConfigServer* NewL();
	~CConfigServer();

	RSocketServ& ESock();
	void SessionConnected() const;
	void Close(CConfigSession* aConfigSession);
	void TimerExpired();

protected:
	CConfigServer();
	void ConstructL();

private:
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
	/** Socket server handle. */
	RSocketServ iEsock;
	/** A shutdown timer. Used to stay-up for 10 seconds after the last disconnect. */
	CExpireTimer* iTimer;
	};

inline RSocketServ& CConfigServer::ESock()
/**
 * Returns handle to ESock connection
 *
 * @internalComponent
 */
	{
	return iEsock;
	}

#endif


