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
//

#include "tserverstartparams.h"
#include "e32math.h"

/** 
Initailise Server start parameters with default message slots

@internalTechnology
@released
 */
TServerStartParams::TServerStartParams() : iNumberOfSlots(KServerFrameworkDefaultMessageSlots)
	{
	SetType();
	}
	
/** 
Set server file name

@internalTechnology
@released
 */
void TServerStartParams::SetServerFileName(const TDesC& aExeName)
	{
	iServerFileName = aExeName;
	}
	
/** 
Set server name

@internalTechnology
@released
 */
 void TServerStartParams::SetServerName(const TDesC& aServerName)
	{
	iServerName = aServerName;
	}
	
/** 
set number of message slots

@internalTechnology
@released
 */
 void TServerStartParams::SetNumberOfServerSlots(TInt aSlots)
	{
	iNumberOfSlots = aSlots;
	}
	
/** 
set server Uids

@internalTechnology
@released
 */
void TServerStartParams::SetServerUids(TUid aUid1, TUid aUid2, TUid aUid3)	
	{
	TUidType uid(aUid1, aUid2, aUid3);
	iUids = uid;
	}
	
/** 
get server file name 

@internalTechnology
@released
 */
const TDesC& TServerStartParams::GetServerFileName()
	{
	return iServerFileName;
	}
	
/** 
get server name

@internalTechnology
@released
 */
const TDesC& TServerStartParams::GetServerName()
	{
	return iServerName;
	}

/** 
get number of message slots

@internalTechnology
@released
 */
TInt TServerStartParams::GetNumberOfServerSlots()
	{
	return iNumberOfSlots;
	}
		
/** 
get server start commandline buffer

@return descriptor to the commandline buffer
@internalTechnology
@released
 */
TPtrC TServerStartParams::GetAsCommandLine() const
	{ 
	TPtrC p(reinterpret_cast<const TText*>(this), sizeof(TServerStartParams)/sizeof(TText));
	TInt l = p.Length();
	return p;
	}

/** 
get server uids 

@internalTechnology
@released
 */
TUidType& TServerStartParams::GetServerUids()
	{
	return iUids;
	}


/** 
Fix the type of these params to KServerStartParamsTypeUid

@internalTechnology
@released
 */
void TServerStartParams::SetType()
	{
	TStartParamsBase::SetType(KServerStartParamsTypeUid);
	}
