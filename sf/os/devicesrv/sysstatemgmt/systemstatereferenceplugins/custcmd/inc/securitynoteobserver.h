/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Declaration of CStrtSecurityNoteObserver class.
*
*/




/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#ifndef __SECURITYNOTEOBSERVER_H__
#define __SECURITYNOTEOBSERVER_H__

#include <e32base.h>

NONSHARABLE_CLASS(CSecurityNoteObserver) : public CActive
	{
public:
	static CSecurityNoteObserver* NewL();
	~CSecurityNoteObserver();

private:
	//from CBase
	CSecurityNoteObserver(const TUid& aCategory, const TUint aKey);
	TBool PropertyChanged(const TInt aCurrentValue);
	void StartL();

	//from CActive
	void DoCancel();
	void RunL();
	void Activate();
	
private:
	// Category of the P&S key to observe.
	TUid iCategory;
	
	// Identifies the P&S key to observe.
	TUint iKey;
	
	// Publish & subscribe interface for the SIM indicator flag key.
	RProperty iProperty;
	};

#endif //__SECURITYNOTEOBSERVER_H__
