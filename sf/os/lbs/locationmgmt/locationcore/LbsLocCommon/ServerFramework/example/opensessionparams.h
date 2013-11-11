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

#ifndef __OPENSESSIONPARAMS_H__
#define __OPENSESSIONPARAMS_H__

#include <e32std.h>
#include <e32base.h>

// class used to pass data about the sub session type to/from the client/server
class TSubSessionType
	{
public:
	enum TSubSessionTypeEnum
		{
		EExInvalidSubsession = 0,
		EExTimeSubSession = 1,
		EExServerNameSubSession = 2
		};
public:
	TSubSessionType() 
		{
		};
	TSubSessionType(const TSubSessionTypeEnum aSubSessionType) : iSubSessionType(aSubSessionType)
		{
		};
	const TSubSessionTypeEnum GetSubSessionType() const
		{
		return iSubSessionType;
		};
protected:
	TSubSessionTypeEnum iSubSessionType;
	};
	

// this is a sample calss, to to show that you can pass
// blocks of data around
class TTimeOpenSessionParams
	{
public:
	TInt 		iDummyNumberParam;
	TBuf<32> 	iDummyBufParam;	
	};
	
// this is another sample to prove that you don't need a class	
typedef TInt TServerNameOpenSessionParams;

#endif // __ OPENSESSIONPARAMS_H__
