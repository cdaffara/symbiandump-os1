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
// @file initialisepdpcontext.h
// This contains CInitialisePdpContext which is defines the founctions for loan comm port.
// 

#ifndef __INITIALISEPDPCONTEXT_H__
#define __INITIALISEPDPCONTEXT_H__

#include <callbacktimer.h>

// user include
#include "atcommandcontrolbase.h"
#include "cltsypacketserviceshandler.h"

/**
 *  CInitialisePdpContext inherits from the MTimerObserver class. 
 * This class provide the common functionalities for Initialise Pdp Context.
 */
class CInitialisePdpContext : public MTimerObserver,
                              public CBase
{
public:
	static CInitialisePdpContext* NewL(CGlobalPhonemanager& aGloblePhone, 
		                               CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * Destructor
	 *
	 */
	~CInitialisePdpContext();

	void StartRequest();
	/*
	 * Set the parameter of AT command 
	 * 
	 * @param aContext The context to be set configuration
	 */
	void SetContext(const TInt aContextId);
	/**
	 * This function will stop the TSY from using the comport 
	 * and loan it to the client.
	 */
	void LoanCommPort();
	
protected:	
	/**
	 * Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 */
	CInitialisePdpContext(CGlobalPhonemanager& aGloblePhone, 
		                  CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	/**
	 * 2nd Constructor
	 */
	void ConstructL();
private:
	//From MTimerObserver
    virtual void TimerRun(TInt aError);	
private:
	
	CGlobalPhonemanager& iPhoneGlobals;
	CCtsyDispatcherCallback& iCtsyDispatcherCallback;
	CCallbackTimer* iCallbackTimer;	
	/*
	 * The context to be set configuration
	 */
	TInt iContextId;
    };

#endif //__INITIALISEPDPCONTEXT_H__
