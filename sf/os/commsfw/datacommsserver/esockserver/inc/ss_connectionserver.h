// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SYMBIAN_SS_CONNECTIONSERVER_H
#define SYMBIAN_SS_CONNECTIONSERVER_H

#include <e32base.h>
#include <comms-infras/ss_thread.h>

#include <comms-infras/ss_metaconnprov.h>
#include <comms-infras/ss_connselect.h>
#include <comms-infras/ss_common.h>
#include <comms-infras/es_availability.h>
#include <comms-infras/api_ext_msg.h>

namespace ESock
{

_LIT(KConnectionServerName,"!ConnectionServer");

NONSHARABLE_CLASS(CConnectionServer) : public Den::CCommonServer
/**
@class CConnectionServer
@internalComponent
*/
	{
public:
	enum {EPriority=0};
public:
	static CConnectionServer* NewL(CWorkerThread* aOwnerThread);
	~CConnectionServer();

	CSession2* DoNewSessionL(const TProcessId& aProcessId, const TUidType& aProcessUid) const;
	virtual TVersion CurrentVersion() const;
	
protected:
	CConnectionServer(CWorkerThread* aOwnerThread, TInt aPriority);
    void ConstructL();

private:
// platsec	
	static const TUint PolicyRangeCount;
	static const TInt PolicyRanges[];
	static const CPolicyServer::TPolicyElement PolicyElements[];
	static const TUint8 PolicyElementsIndex[];
	static const CPolicyServer::TPolicy Policy;
	};


} // namespace ESock

#endif // SYMBIAN_SS_CONNECTIONSERVER_H

