// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// MCommEngineObserve & MCommReadLineNotifier
// 

#ifndef __MCOMMOBSERVER_H__
#define __MCOMMOBSERVER_H__

//symtem include

//user include 
#include "ltsycommondefine.h"

/**
 * Define the engine's observer
 * 
 */
class MCommEngineObserver
	{
public:
	/**
	 * Notify the observer if any event happens
	 * 
	 */
	virtual void ReadOrWriteComplete(TAtEventSource aEvent, TInt aStatus) = 0;
	};

/**
 * Define the notifer of one line string which has been found
 * 
 */
class MCommReadLineNotifier
	{
public:
	/**
	 * Notify one line of string was found
	 */
	virtual TBool NotifyOneLineFoundL(TInt aStatus, const TDesC8& aLineBuf)=0;
	};	

#endif