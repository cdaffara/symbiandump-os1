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

#ifndef RRLP_ASSISTANCE_DATA_H
#define RRLP_ASSISTANCE_DATA_H

#include "rrlpmessagebase.h"
#include <lbs/lbsassistancedatabase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include "lbspositioninfo.h"

/**
Encapsulation for received RRLP Assistance Data message

@internalTechnology
*/
NONSHARABLE_CLASS(CRrlpAssistanceData) : public CRrlpMessageBase
	{
public:
	/** static factory constructor */
	IMPORT_C static CRrlpAssistanceData* NewL();
		
	virtual ~CRrlpAssistanceData();
	
public:

	/** Returns ETrue if valid assistance data is present in the received message */
	IMPORT_C TBool AssistanceDataPresent();
	
	/** Populates aData with received assistance data, and aDataMask to 
	    indicate the assistance data parts present */
	IMPORT_C TInt BuildAssistanceData(TLbsAsistanceDataGroup& aDataMask, 
	                                  RLbsAssistanceDataBuilderSet& aAssistanceDataBuilderSet);
		
	/** Returns ETrue if another RRLP Assitance Data message to be delivered */
	IMPORT_C TBool MoreAssDataToBeSent();
	
protected:

	/** gets the extended reference elements */
	TInt GetExtendedReference(TRrlpReference& aRrlpRef);

private:
	/** default constructor */
	CRrlpAssistanceData();

	/** second stage constructor */
	void ConstructL();
	
	/** Prohibit copy constructor */
	CRrlpAssistanceData(const CRrlpAssistanceData&);

	/** Prohibit assigment operator */
	CRrlpAssistanceData& operator= (const CRrlpAssistanceData&);
	};

#endif // RRLP_ASSISTANCE_DATA_H

