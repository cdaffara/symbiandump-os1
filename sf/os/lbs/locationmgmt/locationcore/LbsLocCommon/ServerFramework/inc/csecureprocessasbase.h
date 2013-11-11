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
// Definition of class used to start a LBS non-server process 
// 
//

/**
 @file
 @internalTechnology
*/

#ifndef __LBSSECUREPROCESSASBASE_H__
#define __LBSSECUREPROCESSASBASE_H__

#include <e32base.h>

/**
@internalTechnology
@released

CActiveScheduler Class.
 */
class CSecureProcessASBase : public CActiveScheduler
	{
public:
	/** Secure process panic type */
	enum TSecureProcessASBasePanic
		{
		ECreateRootObjectNotOverridden = 1,
		EEntryPointNotOverridden = 2	
		};
public:
	// new bits for processes
	virtual CBase* CreateRootObjectL();
	virtual TInt EntryPointL(CBase* aObj);
	virtual void Final();	
	void Panic(const TSecureProcessASBasePanic aPanic);
	};	
	
#endif //__LBSSECUREPROCESSASBASE_H__
