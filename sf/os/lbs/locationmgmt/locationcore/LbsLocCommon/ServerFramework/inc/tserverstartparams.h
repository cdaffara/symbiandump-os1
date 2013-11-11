// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of class used to define server start parameters 
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef __SERVERSTARTPARAMS_H__
#define __SERVERSTARTPARAMS_H__

#include <e32std.h>
#include <e32base.h>
#include "tstartparamsbase.h"

const TInt KServerFrameworkDefaultMessageSlots = 4;


/** 
@internalTechnology
@released

Parameters for starting server 
 */
NONSHARABLE_CLASS(TServerStartParams) : public TStartParamsBase
	{
public:
	TServerStartParams();
	void SetServerFileName(const TDesC& aExeName);
	void SetServerName(const TDesC& aServerName);
	void SetNumberOfServerSlots(TInt aSlots);
	void SetServerUids(TUid aUid1, TUid aUid2, TUid aUid3);
	const TDesC& GetServerFileName();
	const TDesC& GetServerName();
	TInt GetNumberOfServerSlots();
	TPtrC GetAsCommandLine() const;
	TUidType& GetServerUids();
private:
	// from TStartParamsBase
	void SetType();
private:
	// property and really do mean private here
	/** Server file name */
	TFileName					iServerFileName;
	/** Server name */
	TFileName					iServerName;
	/** Number of slots in server */
	TInt							iNumberOfSlots;
	/** a set of three unique identifiers (UIDs) identify the server */
	TUidType 					iUids;
	};
	
#endif // __SERVERSTARTPARAMS_H__

