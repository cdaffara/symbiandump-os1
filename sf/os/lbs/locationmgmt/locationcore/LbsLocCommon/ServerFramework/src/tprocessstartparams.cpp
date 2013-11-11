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
// tprocessstartparams.h
// 
//

#include "tprocessstartparams.h"
#include <e32math.h>

/** Initialise start process parameters
@internalTechnology
@released
 */
TProcessStartParams::TProcessStartParams() : iRendezvousRequired(EFalse)
	{
	SetType();
	}
	
/** set the rendezous requirement

@internalTechnology
@released
 */
 void TProcessStartParams::SetRendezvousRequired(TBool aRendezvousRequired)
	{
	iRendezvousRequired = aRendezvousRequired;
	}
	
/** Get the rendezous requirement

@return TBool, Rendezvous property
@internalTechnology
@released
 */
 TBool TProcessStartParams::GetRendezvousRequired() const
	{
	return iRendezvousRequired;
	}
	
/** set the process file name

@param aExeName The process file name
@internalTechnology
@released
 */
 void TProcessStartParams::SetProcessFileName(const TDesC& aExeName)
	{
	iProcessFileName = aExeName;
	}
	
/** Set the process name

@param aProcessName The process name
@internalTechnology
@released
 */
 void TProcessStartParams::SetProcessName(const TDesC& aProcessName)
	{
	iProcessName = aProcessName;
	}	

/** Get the process file name

@return The process file name
@internalTechnology
@released
 */
 const TDesC& TProcessStartParams::GetProcessFileName() const
	{
	return iProcessFileName;
	}
	
/** Get the process name

@return The process name
@internalTechnology
@released
 */
 const TDesC& TProcessStartParams::GetProcessName() const
	{
	return iProcessName;
	}
		
/** Get the command line buffer

@return The descriptor to the commandline buffer
@internalTechnology
@released
 */
 TPtrC TProcessStartParams::GetAsCommandLine() const
	{
	TPtrC p(reinterpret_cast<const TText*>(this), sizeof(TProcessStartParams)/sizeof(TText));
	return p;
	}
	
/** 
Fix the type of these params to KProcessStartParamsTypeUid

@internalTechnology
@released
 */
void TProcessStartParams::SetType()
	{
	TStartParamsBase::SetType(KProcessStartParamsTypeUid);
	}	
