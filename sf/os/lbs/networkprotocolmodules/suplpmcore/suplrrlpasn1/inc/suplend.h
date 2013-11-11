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
#ifndef LBS_SUPL_END_H
#define LBS_SUPL_END_H

#include "suplmessagebase.h"
#include <lbspositioninfo.h>

	
/**
Encapsulation for outgoing/received SUPL END messages.

@internalTechnology
*/
NONSHARABLE_CLASS(CSuplEnd) : public CSuplMessageBase
	{
public:
	IMPORT_C static CSuplEnd* NewL(TBool aIsOutgoingMessage);
	IMPORT_C static CSuplEnd* NewLC(TBool aIsOutgoingMessage);
	virtual ~CSuplEnd();
	
public:

	/** Set the positon parameter (for outgoing messages) */
	IMPORT_C TInt SetPosition(const TPositionInfoBase& aPosInfo);
	
	/** Set the SUPL Status Code (for outgoing messages) */
	IMPORT_C TInt SetStatusCode(TSuplStatusCode& aStatusCode);
	
	/** Set the hash of the received SUPL INIT (for outgoing messages) */
	IMPORT_C TInt SetVer(const TDesC8& aVer);

	/** Returns ETrue if the position parameter is present (for received messages) */
	IMPORT_C TBool PositionPresent();
	
	/** Populates aPosInfo with the received position information (for received messages) */
	IMPORT_C TInt GetPosition(TPosition& aPosInfo);
	
	/** Returns ETrue if the status code parameter is present (for received messages) */
	IMPORT_C TBool StatusCodePresent();

	/** Returns the status code parameter (for received messages) */
	IMPORT_C TSuplStatusCode StatusCode();

private:

	/** Constructor */
	CSuplEnd(TBool aIsOutgoingMessage);

	/** second stage constructor */
	void ConstructL();
	
	/** Prohibit copy constructor */
	CSuplEnd(const CSuplEnd&);

	/** Prohibit assigment operator */
	CSuplEnd& operator= (const CSuplEnd&);
	
	TInt DecodeTime(const TUint8* aTimeString, TTime& aTimeOut);
	};

#endif // LBS_SUPL_END_H
