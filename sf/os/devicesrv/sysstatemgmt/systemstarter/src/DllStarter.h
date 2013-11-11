// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined (__DLLSTARTER_H__)
#define __DLLSTARTER_H__

#include <e32base.h>
#include "StartupCommand.h"

// Forward class declaration
class CSystemStartupDllInfo;  
	
/** A startup command object responsible for starting a function 
within a DLL by ordinal number

@internalComponent
@deprecated
*/
class CDllStarter : public CBase, public MStartupCommand
	{
public:
	static CDllStarter* NewL(const CSystemStartupDllInfo* aDllInfo );
	static CDllStarter* NewLC(const CSystemStartupDllInfo* aDllInfo );
	~CDllStarter();
	
	// from MStartupCommand
	void Execute(TRequestStatus& aCallerStatus);
	void Release();

private:
	CDllStarter(const CSystemStartupDllInfo* aDllInfo );	
	
private:
	const CSystemStartupDllInfo* iDllInfo;
	};

#endif // __DLLSTARTER_H__

