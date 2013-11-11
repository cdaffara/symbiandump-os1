// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/

#ifndef __ETELMNGR_H
#define __ETELMNGR_H

#include <e32base.h>
#include <e32cons.h>
#include <etel.h>
#include <etelmm.h>
#include "rpsownnumbers.h"
#include "rpsasciirqstdefs.h"
//
// CEtelMngr.
// This is a wrapper for the Etel server. It manages the connection to the
// etel server, and invokes its api methods when a request is made.
//
// Not intended for derivation
class CEtelMngr : public  CBase
	{
public:
	static CEtelMngr* NewL();
	~CEtelMngr();
	
public:
	RTelServer& Etel() {return iEtel;}
	RMobilePhone& Phone() {return iPhone;}
	
	RMobileCall& Call(TEtelLine aLine);
	RMobileLine& Line(TEtelLine aLine);
	
	TBool DataSupported()   {return (iDataLineName.Length() > 0);}
	TBool FaxSupported()   {return (iFaxLineName.Length() > 0);}
	TRPSOwnNumbers& OwnNumbers() 	{return iOwnNumber;}
	
	TRPSOwnNumbers iOwnNumber;
	TInt CheckCallCapabilities(RMobileCall& aCall, const RMobileCall::TMobileCallControlCaps aExpected);
	
	TInt SendDataL(const TDesC8& aDataToSend);
	
private:
	void ConstructL();
	
private:
	RTelServer  	iEtel;
	RMobilePhone 	iPhone;
	
	TName iVoiceLine1Name;
	TName iVoiceLine2Name;
	TName iDataLineName;
	TName iFaxLineName;
	
	RMobileLine		iVoiceLine1;
	RMobileLine		iVoiceLine2;
	RMobileCall		iVoiceCall1;
	RMobileCall		iVoiceCall2;
	
	RMobileLine		iDataLine;
	RMobileCall		iDataCall;
	};



#endif
