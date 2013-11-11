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


#ifndef _ATGPRSCONFIG_H
#define _ATGPRSCONFIG_H

#include <e32base.h>
#include "et_tsy.h"
#include "et_phone.h"
#include "ATBASE.H"
#include <etelpckt.h>




class CATGPRSSetConfig : public CATCommands
/**
Sends the AT command to configure a specific context or to
set the specific context settings.
Both CATGPRSSetConfig and CATGPRSGetConfig inherits from the CATCommands class. 
This class transmits the following commands: AT+CGQREQ=, AT+CGDCONT=, AT+CGQMIN=
@internalComponent
*/
	{
public:
	static CATGPRSSetConfig* NewL(TInt aCid, CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	~CATGPRSSetConfig();
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aConfig);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);
	
private:
	CATGPRSSetConfig(TInt aCid, CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	void ConstructL();
	virtual void EventSignal(TEventSource aSource);
	virtual void Complete(TInt aErr, TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	TInt MakeupCGDCONT();

private:
	RPacketContext::TContextConfigGPRS*	iContextConfigGPRS;	//< Context settings.
	TInt								iCid;				//< Context id.
	enum 
		{
		EATNotInProgress,
		EWaitForSetCGDCONTComplete,
		EWaitForSetCGDCONTOK
		}iState;												//< Enum for class states.
	};

/**
 * Sends the AT command to get the current context configuration
 * CATGPRSGetConfig inherits from the CATBase class. 
 * This class transmits the following commands: AT+CGQREQ=, AT+CGDCONT=, AT+CGQMIN=
 */
class CATGPRSGetConfig : public CATCommands
	{
public:
	static CATGPRSGetConfig* NewL(TInt aCid, CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	
	~CATGPRSGetConfig ();
	virtual void Start(TTsyReqHandle aTsyReqHandle, TAny* aConfig);
	virtual void Stop(TTsyReqHandle aTsyReqHandle);

private:
	CATGPRSGetConfig (TInt aCid, CATIO* aIo, CTelObject* aTelObject, CATInit* aInit, CPhoneGlobals* aPhoneGlobals);
	void ConstructL();
	virtual void EventSignal(TEventSource aSource);
	virtual void Complete(TInt aErr, TEventSource aSource);
	virtual void CompleteWithIOError(TEventSource aSource,TInt aStatus);
	void ParseCGDCONTResponseL();

private:
	RPacketContext::TContextConfigGPRS*	iContextConfigV1;	//< Context settings.
	TInt								iCid;				//< Context id.
	enum 
		{
		EATNotInProgress,
		EWaitForGetCGDCONTComplete,
		EWaitForGetCGDCONTOK
		}iState;												//< Enum for class states.
	};
#endif
