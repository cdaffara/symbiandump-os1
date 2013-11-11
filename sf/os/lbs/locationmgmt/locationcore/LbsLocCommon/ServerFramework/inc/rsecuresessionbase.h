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
// rsecureasbase.h
// Definition of base class for secure sessions
// 
//

/**
 @file
 @internalTechnology
*/


#ifndef __LBSSECURESESSIONBASE_H__
#define __LBSSECURESESSIONBASE_H__

#include <e32std.h>
#include "tserverstartparams.h"
#include "lbsmessageenums.h"

NONSHARABLE_CLASS(RSecureSessionBase) : public RSessionBase
	{
public:
	TInt Connect(TServerStartParams& aParams);
	virtual void Close();
	virtual TVersion Version() = 0;
	virtual TInt StartServer(TServerStartParams& aParams);
	};
	
NONSHARABLE_CLASS(RSecureSubSessionBase) : public RSubSessionBase
	{
public:
	// template code MUST be inlined
	template <class S, class T>
	inline TInt Open(RSecureSessionBase& aServer, S& aSubSessionType, T& aOpenData);
	//TInt Open(RSecureSessionBase& aServer, const TInt& aOpenData);
	virtual void Close();	
	};

template<class S, class T>
TInt RSecureSubSessionBase::Open(RSecureSessionBase& aServer, S& aSubSessionType, T& aOpenData)
	{
	TPckg<S> subSessPckg(aSubSessionType);
	TPckg<T> openPckg(aOpenData);
	//TPckg<TInt> openPckg(aOpenData);
	TIpcArgs args(&subSessPckg, &openPckg);
	return CreateSubSession(aServer, ESecureSubSessionBaseOpen, args);	
	}


#endif	// __LBSSECURESESSSIONBASE_H__	
