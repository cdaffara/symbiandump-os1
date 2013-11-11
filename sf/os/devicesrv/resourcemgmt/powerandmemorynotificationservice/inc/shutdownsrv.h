// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(SHUTDOWNSRV_H)
#define SHUTDOWNSRV_H

#include <e32base.h>
#include <savenotf.h>
#include <e32power.h>
#include <s32mem.h>

class CShutdownTimer;

/**
This class describes an interface, which is used by LaF Shutdown Manager to notify the Shutdown Server of 
device shutdown, so that the server can take appropriate power management steps. The concrete implementation 
of this interface is provided by CServShutdownServer class.
@publishedPartner
@released
*/
class MShutdownEventObserver
	{
public:
	/**
	This method has to be called, when the observed object requires the Shutdown Server to notify registered 
	clients about the shutdown event, such as MSaveObserver::ESaveData, MSaveObserver::ESaveAll, 
	MSaveObserver::EReleaseRAM,...
	@param aAction The type of the requested action
	@param aPowerOff If it is non-zero, this is the beginning of a powerdown sequence.
	@param aEvent The type of the powerdown event 
	@leave KErrNotSupported Leaves if aEvent is invalid  
	*/
	virtual void HandleShutdownEventL(MSaveObserver::TSaveType aAction,TBool aPowerOff, TPowerState aEvent = EPwStandby)=0;
	/**
	This method creates an array of CArrayFix<TThreadId> type and appends to it the	thread id-s of 
	all the registered clients. The created CArrayFix<TThreadId> instance will be pushed on the 
	cleanup stack. The caller becomes responsible for the memory allocated for this array.
	@return A pointer to a CArrayFix<TThreadId> array with the client thread id-s.
	@leave Some system-wide error codes including KErrNoMemory.
	*/
	virtual CArrayFix<TThreadId>* ClientArrayLC()=0;
	/**
	Checks if a particular client is hung in that the client has not re-registered with the 
	Shutdown Server indicating it is ready for the next stage of the shutdown.
	@param aId Client's thread id.
	@return Non-zero if the client with this thread id has no pending request.
	*/
	virtual TBool IsClientHung(TThreadId aId) const=0;
	/**
	This method returns information about the shutdown status.
	@param aPowerOff An output parameter, where power-off status will be stored. 
	                 It will be non-zero, if a powerdown sequence has been initiated.
	@param aAllSessionsHavePendingRequest An output parameter. It will be non-zero, if all clients 
	                 have pending requests to receive further events from the Shutdown Manager.
	*/
	virtual void GetShutdownState(TBool& aPowerOff, TBool& aAllSessionsHavePendingRequest) const=0;
	};

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "shutdownsess.h"
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

#endif// SHUTDOWNSRV_H
