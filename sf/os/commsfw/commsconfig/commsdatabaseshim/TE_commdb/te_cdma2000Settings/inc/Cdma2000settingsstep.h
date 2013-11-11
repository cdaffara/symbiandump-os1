//
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
//

/**
 @file
 @internalComponent
 
 Interface for CDMA2000 CommDB settings test.
 */
 
#ifndef __CDMA2000SETTINGSSTEP_H__
#define __CDMA2000SETTINGSSTEP_H__

#include "te_cdmacommdbsettingssvr.h"

#include <testexecutestepbase.h>
#include <commdb.h>

namespace te_Cdma2000CommDbSettings
	{
/**
 Defines CDMA2000 CommDB settings test.
 
 @internalComponent
 @test
 */
class CCdma2000SettingsTestStep: public CTestStep
	{
public:
	CCdma2000SettingsTestStep();
	~CCdma2000SettingsTestStep();
	TVerdict doTestStepL();
	
private:
	void CloseCommDbTable();
	void OpenCommDbTableL(const TPtrC& aTableName);
	TBool CheckTableTBool(const TPtrC aCommDbTableCols[]);
	TBool CheckTableTUint32(const TPtrC aCommDbTableCols[]);
	TBool CheckTableText(const TPtrC aCommDbTableCols[]);
	void DoTestSequenceL();	
	
	/** CommDB  */
	CCommsDatabase*		iCommDb;
	
	/** A view on CommDB used to access columns */
	CCommsDbTableView*	iCommDbView;	
	};
	
_LIT(KCdma2000SettingsStep, "CCdma2000SettingsStep");	
	}


#endif // __CDMA2000SETTINGSSTEP_H__
