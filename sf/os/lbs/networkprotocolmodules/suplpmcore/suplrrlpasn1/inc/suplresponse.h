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
//

/**
 @file
 @internalTechnology
 
*/
#ifndef LBS_SUPL_RESPONSE_H
#define LBS_SUPL_RESPONSE_H

#include "suplmessagebase.h"
#include <lbs/lbsnetcommon.h>


/**
Encapsulation for received SUPL RESPONSE messages.

@internalTechnology
*/
NONSHARABLE_CLASS(CSuplResponse) : public CSuplMessageBase
	{
public:
	IMPORT_C static CSuplResponse* NewL();
		
public:
	virtual ~CSuplResponse();
	
public:
	
	/** Populates aMethod according to the INIT message */
	IMPORT_C TInt GetPosMethod(TLbsNetPosRequestMethod& aMethod);

	/** Returns ETrue if the SlpAddress parameter is present */
	IMPORT_C TBool SlpAddressPresent();

	/** Populates aAddress with the SLP address */
	IMPORT_C TInt GetSlpAddress(CSuplSlpAddress& aAddress);
	
private:
	/** Constructor */
	CSuplResponse();

	/** second stage constructor */
	void ConstructL();
	
	/** Prohibit copy constructor */
	CSuplResponse(const CSuplResponse&);

	/** Prohibit assigment operator */
	CSuplResponse& operator= (const CSuplResponse&);
	};

#endif // LBS_SUPL_RESPONSE_H
