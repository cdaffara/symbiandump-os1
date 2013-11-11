// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalAll 
*/

#ifndef __ATGPRSQOSPROFILE_H
#define __ATGPRSQOSPROFILE_H

#include <e32base.h>
#include "et_tsy.h"
#include "et_phone.h"
#include "ATBASE.H"
#include <etelpckt.h>




class CATGprsSetProfile : public CATCommands
/**
Sends the AT command to set the specific QoS profile settings.
Both CATGPRSSetProfile and CATGPRSGetProfile inherits from the CATCommands class. 
This class transmits the following commands: AT+CGQREQ=, AT+CGQMIN=
@internalComponent
*/
	{
public:
	static CATGprsSetProfile* NewL(TInt aCid, CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	~CATGprsSetProfile();
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aConfig);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
	
protected:
	CATGprsSetProfile(TInt aCid, CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	void ConstructL();
	virtual void EventSignal(TEventSource aSource);
	virtual void Complete(TInt aErr, TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);

private:
	void MakeupCGQMIN();
	void MakeupCGQREQ();
	RPacketQoS::TQoSGPRSRequested*	iGprsReqProfile;	//< Requested QoS Profile to be set
	TInt iCid;	// context Id used when setting qos profile
	enum // internal states for QoS SetProfile state machine
		{
		EATNotInProgress,
		EWaitForSetCGQMINComplete,
		EWaitForSetCGQMINOK,
		EWaitForSetCGQREQComplete,
		EWaitForSetCGQREQOK
		}iState;
	};


class CATGprsGetProfile : public CATCommands
/**
Sends the AT command to get the specific QoS profile settings.
This class transmits the following commands: AT+CGQREQ?, AT+CGQMIN?
@internalComponent
*/
	{
public:
	static CATGprsGetProfile* NewL(TInt aCid, CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	
	~CATGprsGetProfile ();
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aConfig);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);

protected:
	CATGprsGetProfile (TInt aCid, CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	void ConstructL();
	virtual void EventSignal(TEventSource aSource);
	virtual void Complete(TInt aErr, TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);

private:
	void ParseCGQMINResponseL();
	void ParseCGQREQResponseL();
	RPacketQoS::TQoSGPRSNegotiated* iGprsNegProfile;	
	TInt iCid; // context Id used when getting qoS profile
	enum 
		{
		EATNotInProgress,
		EWaitForGetCGQMINComplete,
		EWaitForGetCGQMINOK,
		EWaitForGetCGQREQComplete,
		EWaitForGetCGQREQOK
		}iState;												//< Enum for class states.
	};
#endif
