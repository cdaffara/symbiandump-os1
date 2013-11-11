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
// CRequestBase

#ifndef ATIPCREQUEST_H_
#define ATIPCREQUEST_H_

// system include
#include <e32base.h>

// user include
#include "atipcrequestcompleteobserver.h"

//Class forward
class CGlobalPhonemanager;

/**
 * The request base which acts a single IPC request passed by dispatcher
 * 
 */
class CRequestBase : public CBase
	{
public:
	/**
	* Destructor
	*/
	virtual ~CRequestBase();
	
	/**
	* Defince an interface of an IPC request to start this request asynchronously
	*/
	virtual void StartRequest();
	
	/**
	 * Set the IPC request complete observer
	 */
	void SetIpcRequestCompleteObserver(MIpcRequestCompleteObserver* aIpcCompleteObserver);
	
	/**
	 * Set IPC Id
	 */
	void SetIpcId(TInt aIpcId);
	
	/**
	 * Get IPC Id
	 */
	TInt IpcId();
protected:
    /**
    * The constructor
    */
	CRequestBase(CGlobalPhonemanager& aPhoneGlobals);
	
protected:
    /**
    * The global phone manager
    */	
	CGlobalPhonemanager& iPhoneGlobals;
	
	/**
	 * The Ipc Request complete observer
	 */
	MIpcRequestCompleteObserver* iIpcCompleteObserver;
	
	/**
	 * IPC Id
	 */
	TInt iIpcId;
	};

#endif /*ATIPCREQUEST_H_*/
