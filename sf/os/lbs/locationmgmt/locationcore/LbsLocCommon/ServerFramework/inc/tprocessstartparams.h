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
// Definition of class used to define process start parameters 
// 
//

/**
 @file
 @internalTechnology
*/


#ifndef __LBSPROCESSSTARTPARAMS_H__
#define __LBSPROCESSSTARTPARAMS_H__

#include <e32std.h>
#include <e32base.h>
#include "tstartparamsbase.h"

/**
@internalTechnology
@released

Parameters for starting the server process
 */
NONSHARABLE_CLASS(TProcessStartParams) : public TStartParamsBase
	{
public:
	TProcessStartParams();
	void SetProcessFileName(const TDesC& aExeName);
	void SetProcessName(const TDesC& aProcessName);
	void SetRendezvousRequired(TBool aRendezvousRequired);
	TBool GetRendezvousRequired() const;
	const TDesC& GetProcessFileName() const;
	const TDesC& GetProcessName() const;
	TPtrC GetAsCommandLine() const;
private:
	// from TStartParamsBase
	void SetType();
private:
	// property and really do mean private here
	/** whether or not the process Rendezvous is required */
	TBool						iRendezvousRequired;
	/** server process file name */
	TFileName					iProcessFileName;
	/** server process name */
	TFileName					iProcessName;
	};
	
#endif // __LBSPROCESSSTARTPARAMS_H__
