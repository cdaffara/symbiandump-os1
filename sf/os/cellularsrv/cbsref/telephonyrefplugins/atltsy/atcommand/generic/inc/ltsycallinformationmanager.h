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
// This contains CLtsyCallInformationManager which manager call information
// 

#ifndef LTSYCALLINFORMATIONMANAGER_H_
#define LTSYCALLINFORMATIONMANAGER_H_

//system include

//user include
#include "ltsycallinformation.h"

//const define
const TInt KLtsyMaxCallCount = 8;

// forward class declarations
class CATHangupCommandRecords;

/**
 * This class is handling all the call related information.
 * 
 * This call information manager store the call information and manage the call status.
 * 
 */
class CLtsyCallInformationManager : public CBase
	{
public:
    /**
     * Factory funciton to create a new object of CLtsyCallInfomationManager
     */
	static CLtsyCallInformationManager* NewL();
	
	/**
     * Factory funciton to create a new object of CLtsyCallInfomationManager
     */
	static CLtsyCallInformationManager* NewLC();
	
	/**
	 * The destructor
	 */
	virtual ~CLtsyCallInformationManager();
	
	/**
	 * Clear all the call information related to the specific CallID
	 */
	void ResetCallInformationByCallId(TInt aCallId);
	
	/**
	 * Clear all the call information stored by call information manager
	 * 
	 * This function will be called when Hangup terminates all calls is called
	 */
	void ResetAllCalInformation();
	
	/**
	 * Get the call information by specific CallID
	 */
	TLtsyCallInformation& GetCallInformationByCallId(TInt aCallId);
	
	/**
	 * Get the call Id which is not currently used
	 */
	TInt FindUnUesedCallId() const;
	
	/**
	 * Check the callId is used or not
	 */
	TBool IsHaveUsedCallId() const;
	
	/**
	 * Find the call ID of 1st incoming call
	 */
	TInt FindIncomingCallId() const;
	
	/**
	 * Find the call Id by its status which is dialling and altering
	 */
	TInt FindDialingOrAlertingCallId() const;
	
	/**
	 * Get the call Id of 2nd incoming call.
	 * Waiting call means that an incoming call is already processed by the client. and just wait
	 * the client to hanle this 2nd incoming call
	 */
	TInt FindWaitingCallId() const;
	
	/**
	 * Set the incoming ring call flag.
	 * If received "Ring" or "+CRing", set this flag to be ETrue. and set it to be EFalse, if this call has been connected.
	 */
	void SetIncomingCallFlag(TBool aIncomingCallFlag);
	
	/**
	 * Get the incoming call flag
	 */
	TBool GetIncomingCallFlag() const;
	
	/**
	 * Get the call ID assigned to emergence call
	 */
	TInt FindEmergencyCallId() const;
	
	/**
	 * Get Hang up command records
	 * 
	 */
	CATHangupCommandRecords& GetHangupCommandRecords();	
	
protected:
	/**
	 * Constructor
	 */
	CLtsyCallInformationManager();
	
	/**
	 * 2nd constructor
	 */
	void ConstructL();
	
private:
    /**
     *Call Information array 
     */
	TLtsyCallInformation iCallInfo[KLtsyMaxCallCount];
	
	/**
	 * Incoming call status flag
	 */
	TBool iIncomingCallFlag;
	
	/**
	 * Own. 
	 * The store to store the hanup information related to AT hangup command
	 */
	CATHangupCommandRecords* iHangupCommandRecords;		
	};

#endif /*LTSYCALLINFORMATIONMANAGER_H_*/
