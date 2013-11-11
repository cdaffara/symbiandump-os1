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
// @file asynchelperrequest.h
// 

#ifndef __ASYNCHELPERREQUEST_H__
#define __ASYNCHELPERREQUEST_H__

//system include
#include <e32base.h>
//user include
#include "cltsypacketserviceshandler.h"
// forward class
class CCtsyDispatcherCallback;
class CGlobalPhonemanager;
/**
 * This is a helper class. 
 * There are sometimes, CTSY passed an asynchronous request to LTSY, and LTSY alreay know what should be returned(No need to Send AT command to Baseband).
 * But as the passed request is 'Asynchronous', so we have to somehow return this value to CTSY in an asynchronous way.
 * So we just inherit form CTimer which an Active object, and reuturn the alreay-known value to CTSY some time later.
 *  
 */
class CAsyncHelperRequest : public CTimer
	{
public:
	/**
	 * 2nd Constructor
	 *
	 * @param aGloblePhone
	 * @param aCtsyDispatcherCallback
	 * @param aPriority
	 */
	static CAsyncHelperRequest* NewL(CGlobalPhonemanager& aGloblePhone,
			                   CCtsyDispatcherCallback& aCallBack,
			                   TInt aPriority = CActive::EPriorityLow);	
	/**
	 * Destructor
	 *
	 */
	virtual ~CAsyncHelperRequest();
	
	/**
	 * Set request IPC
	 */
	void SetRequestId(const TInt aIpc);
protected:
    /**
     * constructor
     */
	CAsyncHelperRequest(CGlobalPhonemanager& aGloblePhone,CCtsyDispatcherCallback& aCallBack,TInt aPriority);
	
	/**
	 * Typicl RunL which inherit from CActive
	 */
	virtual void RunL();
private:
    /**
     * Start the timer
     */
	void StartTimer(const TTimeIntervalMicroSeconds32 aTimeout = 100000);
	
	/**
	 * Stop the timer
	 */
	void StopTimer();
protected:	
    /**
     * Call back dispatcher
     */
    CCtsyDispatcherCallback& iCallBack;
    
    /**
     * The global phone reference
     */
    CGlobalPhonemanager& iGloblePhone;	
private:
    /**
     * The IPC requtest ID which is used to identiy what callback function should be returned
     */
	TInt iRequestId;
	
	/**
	 * Store the IPC request ID
	 */
	RArray<TInt> iReqArray;
	/*
	 * The context to be set configuration
	 */
	TInt iContextId;
	};



#endif //__ASYNCHELPERREQUEST_H__
