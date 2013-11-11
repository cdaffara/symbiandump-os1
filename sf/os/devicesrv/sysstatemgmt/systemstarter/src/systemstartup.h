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
 
#if !defined (__SYSTEMSTARTUP_H__)
#define __SYSTEMSTARTUP_H__

#include <e32base.h>
#include <f32file.h>

#include "startup.h"


class CResourceFileReader;

/**
This class locates the next stage (eg critical dynamic) in the resource 
file using a CResourceFileReader object. 
@internalComponent 
@deprecated
*/
class CSystemStartup : public CBase, public MStartup
	{
public:
	static CSystemStartup* NewL(TInt aBootMode);
	static CSystemStartup* NewLC(TInt aBootMode);
	~CSystemStartup();

public:	// from MStartup
	const MStartupStateInfo* GetNextStateL() const;

private:
	CSystemStartup();
	void ConstructL(TInt aBootMode);

private: // data
	CResourceFileReader* 	iReader;
	RFs 					iFs;
	};

#endif // __SYSTEMSTARTUP_H__
