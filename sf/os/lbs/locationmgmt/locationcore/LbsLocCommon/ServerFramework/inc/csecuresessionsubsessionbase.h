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
// Definition of secure subsession base class
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef __LBSSECURESESSIONSUBSESSIONBASE_H__
#define __LBSSECURESESSIONSUBSESSIONBASE_H__

#include <e32base.h> // for CObject etc.
#include "csecuresessionbase.h"
#include "csecureserverbase.h"


/** 
@internalTechnology
@released

Subsession base class 
 */
class CSecureSessionSubSessionBase : public CSecureSessionBase
	{
public:
	CSecureSessionSubSessionBase();
	virtual ~CSecureSessionSubSessionBase();
	virtual void CreateL();
	virtual void CreateSubSessionL(const RMessage2& aMessage);
	virtual void DestroySubSession(const RMessage2& aMessage);
protected:
	virtual void DispatchMessageL(const RMessage2& aMesssage);
	const CSecureServerBase* Server() const;
	// Helpers
	virtual void DispatchL(const RMessage2& aMessage);
	virtual void DispatchError(const RMessage2& aMessage, const TInt aError); // force people to think about errors!
	virtual void ServiceMessageL(const RMessage2& aMessage) = 0;
	virtual void ServiceMessageError(const RMessage2& aMessage, const TInt aError) = 0;
protected:
	// property
	/** the array to store subsession object handles */
	CObjectIx* 	iObjectIx;
	/** the array container to store subsessions */
	CObjectConIx* 	iContainerIx;
	/** array to store subsessions */
	CObjectCon* 	iObjectCon;
private:
	// property - really do mean private here
	/** indicate whether the base construct has been called */
	TBool iBaseCreateCalled;
	};
	

	
#endif // __LBSSECURESESSIONSUBSESSIONBASE_H__
