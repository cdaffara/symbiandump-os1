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
// @file atsmssend.h
// This contains CAtSmsSend which is used to send sms message
// 

#ifndef ATSMSSEND_H
#define ATSMSSEND_H

 //system include
#include <e32std.h>
#include <e32base.h>
#include <callbacktimer.h>

// user include
#include "atcommandbase.h"

// CLASS DECLARATION
/**
 *  TSmsSendParam
 */
class TSmsSendParam
   	{
public:
	RMobilePhone::TMobileAddress iDestination;
   	TBuf8<KGsmTpduSize> iSmsTpdu;
    RMobileSmsMessaging::TMobileSmsDataFormat iDataFormat;
    RMobilePhone::TMobileAddress  iGsmServiceCentre;
    TBool iMoreMessages;
   	};
/**
 *  TSmsSendResponse 
 */
class TSmsSendResponse
	{
public:
    TInt iValRef;
    TBuf8<KGsmTpduSize>	 iSubmitReport;
	};
/**
 * CAtSmsSend
 *
 */
class CAtSmsSend : public CAtCommandBase,
                   public MTimerObserver
	{
public:
	/**
	* Destructor
	*
	*/
	~CAtSmsSend();
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtSmsSend* NewL(CGlobalPhonemanager& aGloblePhone, 
                            CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC 
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtSmsSend* NewLC(CGlobalPhonemanager& aGloblePhone, 
                             CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	* Virtual function. Inherited from CAtCommandBase
	*  Start Execute AT Command
	*/
	virtual void ExecuteCommand();
	
	/**
	* Will be called by AT Manager whenever a event was triggered
	* 
	* @param aEventSource
	* @param aStatus
	*/
	virtual void EventSignal(TAtEventSource aEventSource, TInt aStatus);
	/**
	* Virtual function. Inherited from CAtCommandBase
	* 
	* @param aResponseBuf Line buf reading from baseband 
	*/
	virtual void ParseResponseL(const TDesC8& aResponseBuf);
	/**
	* Start this request
	* 
	*/
	void StartRequest();
	/**
	* set send message parameter
	* 
	* @param  aMsgParam
	*/
	void SetMessageParam(TSmsSendParam aMsgParam);
private:
	/**
	* Constructor
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	CAtSmsSend(CGlobalPhonemanager& aGloblePhone, 
               CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	* 2nd Constructor
	*/
	void ConstructL();
	
	/**
	* confirm if there is a sca address 
	*  
	*  @return  KErrNone......
	*/
	TInt ConfirmSca();
		
    /**
     * start a timer
     */
    void BeginTimer();	
    
	/**
	* From MTimerObserver 
	*/
	virtual void TimerRun(TInt aError);
	
	/**
	* Issue writing AT Command to baseband
	* not using base class because the buffer length is KMaxAsciiPduLength
	*/
	void StartWritingPduLength();
	
	/**
	* Issue writing AT Command to baseband
	* not using base class because the buffer length is KMaxAsciiPduLength
	*/
	void StartWritingPdu();
private:
	/**
	*  state machine for EventSignal 
	*/
	enum 
		{
		ESetPDULengthComplete,
		ESendPDUComplete,
		}iState;
		
	/**
	*  A pointer for Timer
	*/
	CCallbackTimer* iCallbackTimer;	
	
	/**
	* The buffer contains the "AT" String which should be sent to BaseBand Modem
	*/
	TBuf8<KMaxAsciiPduLength> iTxBuffer;	
	
	/**
	* The structure contains sending message parameter
	*/
	TSmsSendParam iSmsSendParam;
	
	/**
	* PDU in ascii format
	*/
	TBuf8<KMaxAsciiPduLength> iMsgDataAscii; 
	
	/**
	* The response structure which read from modem
	*/
	TSmsSendResponse iSmsSendResponse;
	
	/**
	* The return error value of AT command executed 
	*/
	TInt iError;	
	
	/**
	* The error value before Execute write()
	*/
	TInt iCallbackVal;
	}; // class CAtSmsSend
#endif // ATSMSSEND_H
