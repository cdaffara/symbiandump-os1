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
#ifndef LBS_SUPL_INIT_H
#define LBS_SUPL_INIT_H

#include "suplmessagebase.h"
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsloccommon.h>

/**
Encapsulation for received SUPL INIT messages.

@internalTechnology
*/
NONSHARABLE_CLASS(CSuplInit) : public CSuplMessageBase
	{
public:
	
	/** SLP Mode define */
	typedef TUint32 TSuplSlpMode;
	
	/** SLP Mode enumeration */
	enum _TSuplSlpMode
		{
		ESuplSlpModeProxy    = 0,
		ESuplSlpModeNonProxy = 1
		};

public:
	IMPORT_C static CSuplInit* NewL();
	virtual ~CSuplInit();

public:

	/** Populates aMethod according to SUPL INIT posMethod parameter */
	IMPORT_C TInt GetPosMethod(TLbsNetPosRequestMethod& aMethod);
	
	/** Returns ETrue if optional notification parameter is present */
	IMPORT_C TBool NotificationPresent();
	
	/** Populates aPrivacy according to SUPL INIT notification parameter */
	IMPORT_C TInt GetNotificationType (TLbsNetPosRequestPrivacy& aPrivacy);
		
	/** Returns true if either optional requestor id or client name parameters are present */
	IMPORT_C TBool ExternalRequestInfoPresent();
	
	/** Populates aRequestInfo according to SUPL INIT requestor id parameter */
	IMPORT_C TInt GetExternalRequestInfo (TLbsExternalRequestInfo& aRequestInfo);

	/** Returns true if optional SLP address parameter is present */
	IMPORT_C TBool SlpAddressPresent();

	/** Populates aAddress according to SUPL INIT SLP address parameter */
	IMPORT_C TInt GetSlpAddress(CSuplSlpAddress& aAddress);

	/** Returns true if optional QoP parameter is present */
	IMPORT_C TBool QopPresent();
	
	/** Populates aAddress according to SUPL INIT QoP parameter */
	IMPORT_C TInt GetQop (TLbsNetPosRequestQuality& aQuality);

	/** returns the SLP mode specified*/
	IMPORT_C TSuplSlpMode SlpMode();
	
	/** returns the encoded hash of the received SUPL INIT message */
	IMPORT_C TPtrC8 GetVerL(const TDesC8 &aHSlp);

	/** Stores a copy of the unencoded received message data */
	void SetReceivedMessageL(const TDesC8& aData);
	
private:

	/** Constructor */
	CSuplInit();
	
	/** second stage constructor */
	void ConstructL();

	/** Prohibit copy constructor */
	CSuplInit(const CSuplInit&);

	/** Prohibit assigment operator */
	CSuplInit& operator= (const CSuplInit&);
	
private:

	// storage for the un-decoded received message.
	HBufC8* iMessage;

	// storage for the hash of the received SUPL INIT message
	TBuf8<8> iVer;
	};

#endif // LBS_SUPL_INIT_H

