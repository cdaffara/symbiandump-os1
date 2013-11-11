/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef T_RMOBILECALL_DATA_H
#define T_RMOBILECALL_DATA_H

#include "datawrapperbase.h"
// For ETel API.
#include <etel.h>
// For ETel API.
#include <etelmm.h>

/**
 * Test Active Notification class
 *
 */
class CT_RMobileCallData : public CDataWrapperBase
	{
public:
	~CT_RMobileCallData();
	static	CT_RMobileCallData*	NewL();

	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

protected:
	CT_RMobileCallData();
	void	ConstructL();
	void 	InitialiseL();
private:
	void RunL(CActive* aActive, TInt aIndex); 
	
	void DoCmdHangUp(const TTEFFunction& aSection, const TInt aAsyncErrorIndex);
	void DoCmdGetCallDuration(const TTEFFunction& aSection);
	void DoCmdClose(const TTEFFunction& aSection);
	void DoCmdOpenNewCall(const TTEFFunction& aSection );
	void DoCmdOpenExistingCall(const TTEFFunction& aSection );
	void DoCmdDial(const TTEFFunction& aSection, const TInt aAsyncErrorIndex);
	void DoCmdAnswerIncomingCall(const TTEFFunction& aSection, const TInt aAsyncErrorIndex );
	void DoCmdAnswerIncomingCallPost(const TTEFFunction& aSection, const TInt aAsyncErrorIndex );
	void DoCmdDialCancel(const TTEFFunction& aSection);
	void DoCmdHold(const TTEFFunction& aSection,const TInt aAsyncErrorIndex);
	void DoCmdResume(const TTEFFunction& aSection,const TInt aAsyncErrorIndex);
	void DoCmdSwap(const TTEFFunction& aSection,const TInt aAsyncErrorIndex);
	void DoCmdTransfer(const TTEFFunction& aSection,const TInt aAsyncErrorIndex);
	void DoCmdUtilityCheckCapability(const TTEFFunction& aSection);
	void DoCmdGetCaps(const TTEFFunction& aSection);
	void DoCmdGetStatus();

	RMobileCall* GetMobileCallL(TInt aCall);
public:
	TPtrC 				iPhoneNumber;
private:
	CActiveCallback* 	iActiveCallback;
	RMobileCall 		*iMobileCall;
	RMobileCall::TCaps iCallCaps;
	TInt iMaxPhoneCalls;
	/**
	* Array to RMobileCall handles
	*/
	RPointerArray< RMobileCall > iMobileCalls;
	
	/**
	* Stores the status of the MO call before it was cancelled due to a timeout
	*/
	RCall::TStatus iTimeoutedMOCallStatus;

	};

#endif // T_RMOBILECALL_DATA_H
