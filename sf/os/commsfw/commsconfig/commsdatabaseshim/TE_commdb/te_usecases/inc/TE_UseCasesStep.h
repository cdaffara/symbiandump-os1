//
// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/** 
 * @file TE_EncrptStep.cpp.h
 *
 * This defines the CTS_EncrptStep class which is the base class for all 
 * the EncrptStep test step classes
 *
 */
 
#if (!defined __TE_USECASESSTEP_H__)
#define __TE_USECASESSTEP_H__

#include <e32std.h>
#include <e32svr.h>
#include <cdbstore.h>

#include "TE_UseCasesServer.h"

// Connection Configuration Lits
_LIT(KNameDefault, "default");

// Test step names
_LIT(KCreatingDatabaseAndView,  "CreatingDatabaseAndView"); 
_LIT(KUsecaseReading,  "UsecaseReading");  
_LIT(KUsecaseWriting,  "UsecaseWriting"); 
_LIT(KUsecaseOverridingATableEntry  ,  "UsecaseOverridingATableEntry"); 
_LIT(KUsecaseOverridingThePreferenceTable  ,  "UsecaseOverridingThePreferenceTable"); 
_LIT(KUsecaseUsingATemplate  ,  "UsecaseUsingATemplate"); 

//Error strings
_LIT(KEGeneric, "Test left!");

class TGlobalData;
class CTE_UseCasesSuite;

/* CTE_ConnPrefStep class declaration
 * Provides the base class for all ConnPref test steps
 *
 */
class CTE_UseCasesStep : public CTestStep
	{
public:
	CTE_UseCasesStep();
	virtual ~CTE_UseCasesStep();
	enum TVerdict Test(TInt aResult);

	void ExpLogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
		TRefByValue<const TDesC> aFmt,...);
	
	//TGlobalData* iGlobals;
	
	TGlobalData* Get();
	void Set(TGlobalData * aGlobals);

	CTE_UseCasesSuite* iOwnerSuite;	///< Pointer to suite which owns this test step
	
	};

class TGlobalData
{
public:
	CCommsDbTableView* iView;
	CCommsDatabase* iDb;
};

#endif /* __TE_USECASESSTEP_H__ */
