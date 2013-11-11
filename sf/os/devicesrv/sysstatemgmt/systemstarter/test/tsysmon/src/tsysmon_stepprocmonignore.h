// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent - Internal Symbian test code
*/
 
#ifndef __TSYSMON_STEPPROCMONIGNORE_H__
#define __TSYSMON_STEPPROCMONIGNORE_H__


#include <savenotf.h>
#include "tsysmon_stepbase.h"


//APPFWK-SYSMON-0005

_LIT(KCTestCaseProcMonIgnore, "ProcMonIgnore");

class CStepProcMonIgnore : public CTestStepBase, public MSaveObserver
	{
	
public:
	CStepProcMonIgnore();
	~CStepProcMonIgnore();
	
	//from CTestStep
	TVerdict doTestStepL();
	
	// from MSaveObserver
	void SaveL(TSaveType aSaveType);
	
private:
	static TInt CancelWait(TAny* aSelf);
	void ReportTestSuccessful();

private:
	CSaveNotifier* iNotifier;
	};

#endif
