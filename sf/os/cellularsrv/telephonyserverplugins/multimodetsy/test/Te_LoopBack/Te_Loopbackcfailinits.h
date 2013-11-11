// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test out the TSY with a series of slightly strange modem initialisation responses
// 
//

/**
 @file
 @internalComponent 
*/

#ifndef __CFAILINITS_H__
#define __CFAILINITS_H__

#include "Te_LoopbackTestStepBase.h"
#include "Te_LoopBackScriptEng.h"

//
// First a script with a strange +CNMI sequence
//
const struct TScript KFailInitAScript[]= {
	{	EWait,		NULL,		1, ETimeOutEvent	},
	{	EComplete,	NULL,		0, 0				}
	};

class CTestDriveFailInit : public CTestBase
	{
public:
	static CTestDriveFailInit* NewL(const TScriptList aScriptListEntry);
	virtual TInt RunTestL();

protected:
	virtual TInt DriveETelApiL();

private:
	CTestDriveFailInit(const TScriptList aScriptListEntry);
	TScriptList iScriptListEntry;
	};

class CTestFailInit : public CATScriptEng
	{
public:
	static CTestFailInit* NewL(const TScript* aScript);
	virtual void SpecificAlgorithmL(TInt aParam);
	virtual void Complete(TInt aError);
	virtual TInt Start();

private:
	CTestFailInit(const TScript* aScript);
	void ConstructL();
	const TScript* iScript;
	};

#endif
