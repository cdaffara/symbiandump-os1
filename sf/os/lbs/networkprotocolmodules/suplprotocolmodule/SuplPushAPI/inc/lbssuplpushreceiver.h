// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Location Based Services SUPL Push Receiver API
// 
//

/**
 @file
 @publishedPartner
 @deprecated
*/
#ifndef SUPL_PUSH_RECEIVER_H
#define SUPL_PUSH_RECEIVER_H


//-------------------------------------------------------------------------------
#include <e32base.h>
#include <lbs/lbssuplpushcommon.h>

//-------------------------------------------------------------------------------
class CLbsSuplPushRecImpl;

//-------------------------------------------------------------------------------
/**
The MLbsSuplPushRecObserver class provides a call-back type of interface that must be
used together with the CLbsSuplPushRec class. It is used by the SUPL Protocol Module.
A class using the CLbsSuplPushRec interface must derive from MLbsSuplPushRecObserver
and implement appropriate virtual methods in order to receive notifications about
incoming SUPL INIT messages.

@publishedPartner
@deprecated

@see CLbsSuplPushRec
*/
class MLbsSuplPushRecObserver
	{
public:
	IMPORT_C virtual TVersion Version() const;

	/**
	Receive notification about an incoming SUPL INIT message. 
	
	@param aChannel  [In] The channel the call-back is related to.
	@param aReqId    [In] An Id of the request the call-back is related to.
	@param aMsg      [In] A buffer containing a SUPL INIT message.
	
	@see CLbsSuplPushRec::SuplInitComplete
	@see CLbsSuplPush::SuplInit
	*/
	virtual void OnSuplInit(TLbsSuplPushChannel aChannel, TLbsSuplPushRequestId aReqId, TDesC8& aMsg) = 0;

protected:			
	/**
	Should not be used. Provides for future expansion of the observer interface. 
	Currently not implemented
	*/  
	IMPORT_C virtual TAny* ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2);
	};


//-------------------------------------------------------------------------------
/**
The CLbsSuplPushRec class provides an interface to allow SUPL INIT messages to be
received from SMS or WAP Push frameworks. It is used by the SUPL Protocol Module.
SUPL INIT messages are sent into the LBS subsystem using the CLbsSuplPush class.

@publishedPartner
@deprecated
@see MLbsSuplPushRecObserver
@see CLbsSuplPush
*/
NONSHARABLE_CLASS(CLbsSuplPushRec) : public CBase
	{
public:
	IMPORT_C static CLbsSuplPushRec* NewL(MLbsSuplPushRecObserver& aObserver);
	virtual ~CLbsSuplPushRec();
	
public:
	IMPORT_C TInt SuplInitComplete(TLbsSuplPushChannel aChannel, 
								   TLbsSuplPushRequestId aReqId, 
								   TInt aError, 
								   TInt aReserved);
	
private:
	CLbsSuplPushRec();
	void ConstructL(MLbsSuplPushRecObserver& aObserver);
	
	//** Prohibit copy constructor */
	CLbsSuplPushRec(const CLbsSuplPushRec&);
	//** Prohibit assigment operator */
	CLbsSuplPushRec& operator= (const CLbsSuplPushRec&);

private:
	/** CLbsSuplPushRecImpl* Impl is the internal handle to the Implementation */
	CLbsSuplPushRecImpl* iImpl;
	};

#endif //SUPL_PUSH_RECEIVER_H
