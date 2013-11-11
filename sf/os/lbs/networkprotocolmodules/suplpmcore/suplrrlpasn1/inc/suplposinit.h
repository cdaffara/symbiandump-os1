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
#ifndef LBS_SUPL_POS_INIT_H
#define LBS_SUPL_POS_INIT_H

#include "suplmessagebase.h"
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsassistancedatabase.h>


/**
Encapsulation for received SUPL POS INIT messages.

@internalTechnology
*/
NONSHARABLE_CLASS(CSuplPosInit) : public CSuplMessageBase
	{
public:
	IMPORT_C static CSuplPosInit* NewL();
		
public:
	virtual ~CSuplPosInit();
	
public:
	
	/** Set the capabilities parameter */
	IMPORT_C TInt SetCapabilities(const TLbsNetPosCapabilities& aCapabilities);

	/** Set the requested assistance data parameter */
	IMPORT_C TInt SetRequestedAssistanceData(const TLbsAsistanceDataGroup& aDataReqMask);
	
	/**	Set the location ID parameter */
	IMPORT_C TInt SetLocationId(const CSuplLocationId& aLocationId);

	/**	Set the Position parameter */
	IMPORT_C TInt SetPosition(const TPositionInfoBase& aPosInfo);

	/** Set the hash of the received SUPL INIT */
	IMPORT_C TInt SetVer(const TDesC8& aVer);

private:
	/** Constructor */
	CSuplPosInit();

	/** second stage constructor */
	void ConstructL();
	
	/** Prohibit copy constructor */
	CSuplPosInit(const CSuplPosInit&);

	/** Prohibit assigment operator */
	CSuplPosInit& operator= (const CSuplPosInit&);
	};

#endif // LBS_SUPL_POS_INIT_H
