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


#if !defined (__SYSTEM_STARTER_H__)
#define __SYSTEM_STARTER_H__

#include <e32base.h>

/** System starter main controller class.
@internalComponent
@deprecated
*/
class CSystemStarter: public CActive
	{
public:
	static CSystemStarter* NewL();
	static CSystemStarter* NewLC();
	~CSystemStarter();
	void Start();

private: //from CActive
	void RunL();
	virtual TInt RunError(TInt aError);
	void DoCancel();

private:
	CSystemStarter();

private:
	void RequestNotify();

	};

#endif
