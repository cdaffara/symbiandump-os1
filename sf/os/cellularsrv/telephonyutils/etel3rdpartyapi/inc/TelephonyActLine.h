/*
* Copyright (c) 2009 Sony Ericsson Mobile Communications AB
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Sony Ericsson Mobile Communications AB - initial contribution.
* Nokia Corporation - additional changes.
* 
* Contributors:
* 
* Description:
* TelephonyActNetwork Active Object class, used by CTelephonyFunctions class.
*
*/


/**
 @file
*/

#include <etel.h>
#include <etelmm.h>
#include "Etel3rdParty.h"

class CTelephony;
class CTelephonyFunctions;	// forward reference
class CAsyncRequestBaseAct;

class CNotifyLineStatusAct : public CAsyncRequestBaseAct
/**
This is an active object responsible for interacting with Multimode ETel
*/
	{
	
public:		
	static CNotifyLineStatusAct* NewL(CTelephonyFunctions* aTelephonyFunctions, CTelephonyFunctions::TOperations aPendingOperation, MIncomingCallNameSubject& aIncomingCallNameSubject);
	~CNotifyLineStatusAct();
	void NotifyLineStatus(TDes8* aLineStatus);
	void CancelFromClient();
			
protected:	
	void Complete();
	TInt RunError(TInt aLeaveCode);
	void DoCancel();
	
private:	
	CNotifyLineStatusAct(CTelephonyFunctions* aTelephonyFunctions, CTelephonyFunctions::TOperations aPendingOperation, MIncomingCallNameSubject& aIncomingCallNameSubject);
	CTelephonyFunctions* iTelephonyFunctions;
	CTelephony::TCallStatusV1* iISVCallStatus; 
	RMobileCall:: TMobileCallStatus iMMCallStatus;
	CTelephonyFunctions::TOperations iPendingOperation;
	/**
	Reference to a CTelephonyFunctions active object which 
	impletements the MIncomingCallNameSubject interface. 
	*/
	MIncomingCallNameSubject& iIncomingCallNameSubject;
	/**
	Indicates whether a client is interested in changes in the line 
	status. Set to ETrue if the client has requested to be notified of
	line status changes, EFalse otherwise.
	*/
	TBool iLineStatusRequestedByClient;
	/**
	Possible states this AO can be in.
	*/
	enum TNotifyLineStatusActState
		{
		ENotifyLineStatusActOperating,
		ENotifyLineStatusActCleanUp
		};
	/**
	The state this AO is in.
	*/
	TNotifyLineStatusActState iState; 
	};
	
