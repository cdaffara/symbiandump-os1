// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: Extra test interface to the NetProtocolProxy.  It allows the test
//  to be asynchronous and continue to run other events whilst waiting for a message
//  from the NG. The test is notified when a message arrives (or if it times out)
//
//

#ifndef __CT_LBS_TEST_NG_MESSAGE_HANDLER_H__
#define __CT_LBS_TEST_NG_MESSAGE_HANDLER_H__

#include <lbs/test/lbsnetextendedprotocolproxy.h>

/** Call back function which the test should implement to receive notifications
 *   of when a message is received from the NG.
*/
class MLbsTestNgMessageHandler
	{
public:
	virtual void OnHandleNGMessage(TUint aPmId, TInt aMessageType) = 0;
	};

/** PM/NG Message Event Listener class.  This is an interface between the
 * test and the NetProtocolProxy class.  It allows the test to send messages
 * to the NetProtocolProxy and allows the test to get notified (via MLbsTestNgMessageHandler)
 * when a message from the NG arrives.  The PM ID past into this class MUST match an id present
 * in the pminfo.ini file contained in the hybrid protocol module.  This allows the net
 * protocol proxy to know what RProperty values to use for the GetAFixUtils.  This allows the
 * test to create a number of different instances of this class, all representing different PM's.
 * For the test to correctly work with different proxies, all instances of this class should be
 * given a different unique PM Id.
 */
class CLbsTestNgMessageHandler : public CActive
	{
public:
	static CLbsTestNgMessageHandler* NewL(MLbsTestNgMessageHandler& aObserver, TInt aPmId);
	~CLbsTestNgMessageHandler();
	
	//Test uses this function to make the NetProtocol Proxy listen for messages from the PM stub
	virtual void WaitForResponseL(TInt aTimeOut);
	
	//Allows the test to directly access the protocol proxy, which the test uses to directly
	// send messages to the PM stub and extract message details.
	virtual CNetProtocolProxy* ProtocolProxy();
	
	virtual TInt GetPmId();
	
protected:
	CLbsTestNgMessageHandler(MLbsTestNgMessageHandler& aObserver, TInt aPmId);
	void ConstructL();
	
	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
	
protected:
	MLbsTestNgMessageHandler& iObserver;
	
	CNetProtocolProxy* iProxy;
	
	//Contains the ID of the PM that this proxy is using
	TInt iPmId;
	
	//Request counter.  It stores the timeout values for requests that arrive when we are already busy listening.
	// Once a message arrives, it checks this array to see if another request was made.  If there is an item on
	// the array, it pops it off (FIFO) and re-makes the request.
	RArray<TInt> iRequestTimeout;
	};

#endif	//__CT_LBS_TEST_NG_MESSAGE_HANDLER_H__
