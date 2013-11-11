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

/**
 @file
 @internalAll 
*/

#ifndef DIALISVAO_H
#define DIALISVAO_H

#include <e32base.h>	
#include <e32std.h>
#include <etel3rdparty.h>

/**
 * Wrapper active object to use with CTelephony to dial a call.
 */
class CDialIsvAO : public CActive
	{
public:

	~CDialIsvAO();
	static CDialIsvAO* NewL();
	static CDialIsvAO* NewLC();
	void Dial();
	
	TInt Status() { return iError; }

private:

	CDialIsvAO();
	void ConstructL();

private: // From CActive

	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private: // Owned
	
	CTelephony* iTelephony;
	CTelephony::TCallId iCallId;
	
	CActiveScheduler* iScheduler; 
	
	TInt iError;
	
	}; // class CDialIsvAO

#endif // DIALISVAO_H
