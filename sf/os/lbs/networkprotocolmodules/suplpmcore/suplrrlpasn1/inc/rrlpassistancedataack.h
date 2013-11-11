// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef RRLP_ASSISTANCE_DATA_ACK_H
#define RRLP_ASSISTANCE_DATA_ACK_H

#include "rrlpmessagebase.h"


/**
Encapsulation of outgoing RRLP Assistance Data Ack message

@internalTechnology
*/
NONSHARABLE_CLASS(CRrlpAssistanceDataAck) : public CRrlpMessageBase
	{
public:
	IMPORT_C static CRrlpAssistanceDataAck* NewL();
		
public:
	virtual ~CRrlpAssistanceDataAck();
	
private:
	/** default constructor */
	CRrlpAssistanceDataAck();

	/** second stage constructor */
	void ConstructL();
	
	/** Prohibit copy constructor */
	CRrlpAssistanceDataAck(const CRrlpAssistanceDataAck&);

	/** Prohibit assigment operator */
	CRrlpAssistanceDataAck& operator= (const CRrlpAssistanceDataAck&);
	};

#endif // RRLP_ASSISTANCE_DATA_ACK_H

