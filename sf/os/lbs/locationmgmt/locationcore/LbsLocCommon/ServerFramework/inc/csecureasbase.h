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
// Definition of class used to start a LBS server process 
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef __LBSSECUREASBASE_H__
#define __LBSSECUREASBASE_H__

#include <e32base.h>

// forward classes
class TServerStartParams;

/**
@internalTechnology
@released

 Interface to create server
 */
class MCreateServerImpl
	{
public:
	virtual void CreateServerLC(TServerStartParams& aParams) = 0;
	};

/**
@internalTechnology
@released

CActiveScheduler Class.
 */
class CSecureASBase : public CActiveScheduler
	{
public:
	enum TSecureASBasePanic
		{
		ECreateServerLCNotOverridden = 0
		};
public:
	CSecureASBase();
	static TInt ServerLaunch(TServerStartParams& aParams);
	static TInt ServerMain(TServerStartParams& aParams);
protected:
	static TInt RunServerL(TServerStartParams& aParams);
	TInt CreateServer();
		
	// create the derived server object and leave it on the cleanup stack
	//virtual void CreateServerLC(TServerStartParams& aParams);
	MCreateServerImpl* GetImplLC();
	void Panic(const TSecureASBasePanic aPanic);
	};
	

	
#endif // __LBSSECUREASBASE_H__
