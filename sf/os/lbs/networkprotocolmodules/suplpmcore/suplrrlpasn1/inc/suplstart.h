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
#ifndef LBS_SUPL_START_H
#define LBS_SUPL_START_H

#include "suplmessagebase.h"
#include <lbs/lbsnetcommon.h>

	
/**
Encapsulation for received SUPL START messages.

@internalTechnology
*/
NONSHARABLE_CLASS(CSuplStart) : public CSuplMessageBase
	{
public:
	IMPORT_C static CSuplStart* NewL();
	IMPORT_C static CSuplStart* NewLC();
	virtual ~CSuplStart();
	
public:
	
	/** Set the capabilities parameter */
	IMPORT_C TInt SetCapabilities(TLbsNetPosCapabilities& aCapabilities);

	/**	Set the location ID parameter */
	IMPORT_C TInt SetLocationId(const CSuplLocationId& aLocationId);

	/**	Set the QoP parameter */
	IMPORT_C TInt SetQoP(TLbsNetPosRequestQuality& aQuality);
	
private:
	/** Constructor */
	CSuplStart();
	
	/** Second stage constructor */
	void ConstructL();
	
	/** Prohibit copy constructor */
	CSuplStart(const CSuplStart&);

	/** Prohibit assigment operator */
	CSuplStart& operator= (const CSuplStart&);
	};

#endif // LBS_SUPL_START_H
