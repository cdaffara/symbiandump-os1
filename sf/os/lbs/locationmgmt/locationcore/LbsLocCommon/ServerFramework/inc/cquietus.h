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
//

#ifndef __LBSQUIETUS_H__
#define __LBSQUIETUS_H__

#include <e32base.h>

/** 
@internalTechnology
@released

CActive object, starts the process to kill any
process based around a CActiveScheduler.
*/

class CQuietus : public CActive
	{
public:
	static CQuietus* NewL();
	~CQuietus();
	void CoupDeGrace();
private:
	CQuietus(); // make this private.
	// from CActive
	void RunL();
	void DoCancel(); 
	};
	
#endif // __LBSQUIETUS_H__
