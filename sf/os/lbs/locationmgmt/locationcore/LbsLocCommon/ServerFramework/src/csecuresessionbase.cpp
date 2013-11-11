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
// csesuresessionbase.cpp
// 
//

#include "csecuresessionbase.h"
#include "csecureserverbase.h"
#include "lbsmessageenums.h"
#include <e32debug.h>
// Lbs
#include "lbsdevloggermacros.h"
/** 
constructor, call the CSession2 base class constructor

@internalTechnology
@released
 */	
CSecureSessionBase::CSecureSessionBase() : CSession2()
	{
	LBSLOG(ELogP1,"CSecureSessionBase::CSecureSessionBase");
	}

/** 
destructor, decrease the server session count, which will inturn start shutdown timer.

@internalTechnology
@released
 */	
CSecureSessionBase::~CSecureSessionBase()
	{
	LBSLOG(ELogP1,"CSecureSessionBase::~CSecureSessionBase");
	const CServer2* baseServer = Server();
	reinterpret_cast<CSecureServerBase*>(const_cast<CServer2*>(baseServer))->DecSession(); // reduce the server session count - start shutdown timer etc.
	}
	

/** 
function is called by CSession2 to increase the server session count, which will inturn reset shutdown timer.

@internalTechnology
@released
 */	
void CSecureSessionBase::CreateL()
	{
	const CServer2* baseServer = Server();
	reinterpret_cast<CSecureServerBase*>(const_cast<CServer2*>(baseServer))->IncSession(); // increase the server session count - stop shutdown timer etc.		
	}
	
/** 
Disconnect the session.

@param aMessage A reference to RMessage2 object
@internalTechnology
@released
 */	
void CSecureSessionBase::Disconnect(const RMessage2& aMessage)
	{
	LBSLOG(ELogP1,"CSecureSessionBase::Disconnect");
	CSession2::Disconnect(aMessage);
	// used for a breakpoint
	}
	
/** 
Session message dispatch function 

@param aMessage A reference to RMessage2 object
@internalTechnology
@released
 */	
void CSecureSessionBase::ServiceL(const RMessage2& aMessage)
	{
	DispatchL(aMessage); // if this leaves it hits CSession2::ServiceError (or the derived version of it)
	}
		
/** 
Session error message dispatch function, this is called when error happens, it dispatches the error locally and 
then pass the error message to CSession2

@param aMessage A reference to RMessage2 object
@param aError Error code
@internalTechnology
@released
 */	
void CSecureSessionBase::ServiceError(const RMessage2& aMessage, TInt aError)
	{
	DispatchError(aMessage, aError); // provide the opportunity to do something locally
	// now take the base class actions
	CSession2::ServiceError(aMessage, aError); // will complete aMessage with aError if not already completed.
	}

/** 
Nothing here. a place for extra subsession construction functionalities

@internalTechnology
@released
 */	
void CSecureSessionBase::ConstructSubSessionL()
	{
	// does nothing in the default base class
	}

/** 
Nothing here. a place for extra subsession destruction functionalities

@internalTechnology
@released
 */	
void CSecureSessionBase::DestroySubSession()
	{
	// does nothing in the default base class
	}
