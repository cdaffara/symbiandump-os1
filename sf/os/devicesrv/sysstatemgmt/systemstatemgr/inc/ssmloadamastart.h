// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SSMLOADAMASTART_H__
#define __SSMLOADAMASTART_H__

#include <e32base.h>

/**
Shim dll used to remove the dependency of loadamastart.dll which is common symbian component 
on the amastart.dll which is optional replaceable. loadamastart.dll will load amastart.dll.

See CLoadAmaStart in the sysstart component.

@internalTechnology
@released
*/
class CSsmLoadAmaStart: public CBase
	{
public:
	virtual void StartL(const TUid aDscId) = 0;
	virtual void Start(const TUid aDscId, TRequestStatus& aRequestStatus) = 0;
	virtual void StartCancel() = 0;
	};

#endif // __SSMLOADAMASTART_H__
