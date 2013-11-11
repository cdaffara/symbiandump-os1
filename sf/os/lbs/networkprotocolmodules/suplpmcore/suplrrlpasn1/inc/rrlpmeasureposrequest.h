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

#ifndef RRLP_MEASURE_POSITION_REQUEST_H
#define RRLP_MEASURE_POSITION_REQUEST_H

#include "rrlpmessagebase.h"
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsassistancedatabuilderset.h>


/**
Encapsulation for received CRrlpMeasurePositionRequest messages.

@internalTechnology
*/
NONSHARABLE_CLASS(CRrlpMeasurePositionRequest) : public CRrlpMessageBase
	{
public:
	IMPORT_C static CRrlpMeasurePositionRequest* NewL();
		
public:
	virtual ~CRrlpMeasurePositionRequest();
	
public:

	/** Populates aQuality and aMethod according to received PositionInstruction parameters. */
	IMPORT_C TInt GetPositionInstruct(TLbsNetPosRequestQuality& aQuality, 
								   TLbsNetPosRequestMethod&  aMethod);
	
	/** Returns ETrue if valid GPS Assistance Data is present in the received message */
	IMPORT_C TBool AssistanceDataPresent();
	
	/** Populates aData with received assistance data, and aDataMask to 
	    indicate the assistance data parts present */
	IMPORT_C TInt BuildAssistanceData(TLbsAsistanceDataGroup& aDataMask, 
	                                  RLbsAssistanceDataBuilderSet& aAssistanceDataBuilderSet);

protected:

	/** gets the extended reference elements */
	TInt GetExtendedReference(TRrlpReference& aRrlpRef);
		
private:

	/** Constructor */
	CRrlpMeasurePositionRequest();

	/** second stage constructor */
	void ConstructL();
	
	/** converts uncertainty constants to uncertainty in meters */
	TReal32 UncertaintyToMeters(TInt aUncertainty);
	
	/** Prohibit copy constructor */
	CRrlpMeasurePositionRequest(const CRrlpMeasurePositionRequest&);

	/** Prohibit assigment operator */
	CRrlpMeasurePositionRequest& operator= (const CRrlpMeasurePositionRequest&);
	
	};

#endif // RRLP_MEASURE_POSITION_REQUEST_H

