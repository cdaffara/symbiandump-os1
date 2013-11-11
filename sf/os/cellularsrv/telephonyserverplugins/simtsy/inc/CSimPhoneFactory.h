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
// Simulator Phone Factory Class and DLL entry point
// 
//

/**
 @file
 @internalAll 
*/


#ifndef __CSIMPHONEFACTORY_H__
#define __CSIMPHONEFACTORY_H__

#include "et_phone.h"

/**
* This class 'CSimPhoneFactory' is the Factory class for the Simulator TSY. 
* It has the primary role of creating the CPhoneBase-derived object 'CSimPhone' 
* (which mirrors the client side RPhone and RMobilePhone) and passing its pointer back to ETel. 
*
* Once a client has opened a handle on the phone object (CSimPhone in this TSY), 
* they can then go on to create the objects such as line (CSimLine), 
* call (CSimCall) objects, sms (CSimSmsMessaging), packet (CSimPacketService) etc
* 
* The client can then access all the other functionality in the TSY via these objects
*
*/

class CSimPhoneFactory: public CPhoneFactoryBase
	{
public:
	CSimPhoneFactory();
	~CSimPhoneFactory();
	static CSimPhoneFactory* NewL();

	virtual CPhoneBase* NewPhoneL(const TDesC& aName);
	virtual TInt EnumeratePhones();
	virtual TInt GetPhoneInfo(const TInt aIndex, RTelServer::TPhoneInfo& aPhoneInfo);
	virtual TBool IsSupported(const TInt aMixin);
private:
	CPhoneBase* iPhoneBaseRef;
	TBool CheckSupportAgainstVersion(const TInt& aVersion);
	};

#endif
