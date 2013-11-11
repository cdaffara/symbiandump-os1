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
// csesuresessionbase.h
// CSession2 based secure session to the server
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef __LBSSECURESESSIONBASE_H__
#define __LBSSECURESESSIONBASE_H__

#include <e32base.h>

/** 
@internalTechnology
@released

CSession2 based secure session to the server
 */
class CSecureSessionBase : public CSession2
	{
public:
	CSecureSessionBase();
	virtual ~CSecureSessionBase();
	virtual void CreateL();
	virtual void Disconnect(const RMessage2& aMessage);
	virtual void ServiceL(const RMessage2& aMessage);
	virtual void ServiceError(const RMessage2& aMessage, TInt aError);
protected:
	// Helpers
	virtual void DispatchL(const RMessage2& aMessage) = 0;
	virtual void DispatchError(const RMessage2& aMessage, const TInt aError) = 0; // force people to think about errors!
	virtual void ConstructSubSessionL();
	virtual void DestroySubSession();
	};
	
#endif // __LBSSECURESESSIONBASE_H__
