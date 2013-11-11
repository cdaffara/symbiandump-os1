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
// @file atsmsreceive.h
// This contains CAtSmsReceive which received incoming messages
// 

#ifndef ATSMSRECEIVE_H
#define ATSMSRECEIVE_H

//system include 
#include <e32std.h>
#include <e32base.h>
#include <cmmsmsutility.h>

//user include 
#include "atcommandbase.h"

// CLASS DECLARATION

/**
 * CAtSmsReceiveMessage
 *
 */
class CAtSmsReceive: public CAtCommandBase
	{
public:
	/**
	* Destructor
	*
	*/	
	~CAtSmsReceive();
	
	/**
	*  static NewL  
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/
	static CAtSmsReceive* NewL(CGlobalPhonemanager& aGloblePhone, 
                               CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	*  static NewLC 
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/	
	static CAtSmsReceive* NewLC(CGlobalPhonemanager& aGloblePhone, 
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
protected:
	/**
	* Constructor
	*
	* @param aGloblePhone
	* @param aCtsyDispatcherCallback
	*/	
	CAtSmsReceive(CGlobalPhonemanager& aGloblePhone, 
                  CCtsyDispatcherCallback& aCtsyDispatcherCallback);
	
	/**
	* 2nd Constructor
	*/	
	void ConstructL();
private:
	/**
	* Parse 1st line of CMT response, of the format: "+CMT: [<alpha>], <pdu_len>"
	* Stores the <pdu_len> in aPduLen
	*/
	void CMTResponseFirstLineL();
	
	/**
	* Parse second line of CMT or DS response, of the format: "<pdu>"
	* The <pdu> is saved to aPdu. If the <pdu> is found to have a prefixed SCA
	* then that is removed before the save to aPdu.
	*/
	void CMTResponseSecondLineL();
private:
	/**
	*   when receiving a message,its states should be changed
	*/
	enum 
		{
		EReceiveReady,
		EWaitForCmtSecondLine,
		EATNotInProgress
		}iState;
	
	/**
	*  Stores the length of PDU received
	*/	
	TInt iPduLen;		
	
	/**
	*  binary format of PDU
	*/	
	RMobileSmsMessaging::TMobileSmsGsmTpdu  iPdu;
	
	/**
	*  iAttr saved attritube of message revceived 
	*/	
	RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 iAttr;
	
    /**
	*  The structure used for callback
	*/
	TSmsMsg iMsgReceive;
    
	/**
	* The return error value of AT command executed 
	*/	
	TInt iError; 
	
	}; // class CAtSmsReceive: 
#endif // ATSMSRECEIVE_H
