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
// UPS proprietary messages
// 
//

/**
 @file
 @internalTechnology
*/


#ifndef SYMBIAN_UPSMESSAGES_H
#define SYMBIAN_UPSMESSAGES_H

#include <elements/nm_common.h>				// Messages::KNullMessageId
#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_nodemessages_internal.h>

class UpsMessage
/**
UPS message realm

@internalTechnology
*/
	{	
public:
	enum { ERealmId =  0x10285AE8 };
    
private:
	enum 
		{
		EPolicyCheckRequest = Messages::KNullMessageId + 1,
		EPolicyCheckResponse,					// 2
		EUPSStatusChange						// 3
		};

public:
	typedef ESock::TMessageSigUpsParameters<EPolicyCheckRequest, ERealmId> TPolicyCheckRequest;
	typedef Messages::TMessageSigNumber<EPolicyCheckResponse, ERealmId> TPolicyCheckResponse;
	typedef ESock::TMessageSigUpsStatusChangeParameters<EUPSStatusChange, ERealmId> TUPSStatusChange;
	};

#endif
// SYMBIAN_UPSMESSAGES_H

