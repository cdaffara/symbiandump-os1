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

#ifndef TSU_MMF_AVICTRL_TESTSUITE_H
#define TSU_MMF_AVICTRL_TESTSUITE_H

#include <testframework.h>

const TUid KAviRecordControllerUid = {0x102737E1};
const TUid KAviPlayControllerUid   = {0x102737A5};
const TUid KAviRecordFormatUid =    {0x102737E3};
_LIT8(KAviVideoCodec,"XVID");

const TInt KSizeBuf = 256;
const TInt KPrimaryScreenNo = 0;
const TInt KSecondaryScreenNo = 1;
const TInt KInvalidScreenNo = 100;
class RTestStepAviCtrlBase;
class RAsyncTestStepAviCtrlBase;

/**
CTestSuiteAviCtrl is the test suite  for the AviController
*/  
class  CTestSuiteAviCtrl : public CTestSuite
	{
public:
	void InitialiseL();
	virtual ~CTestSuiteAviCtrl();
	void AddTestStepL(RTestStepAviCtrlBase* aPtrTestStep);
	void AddTestStepL(RAsyncTestStepAviCtrlBase* aPtrTestStep);
	TPtrC GetVersion();
	const TDesC& DefaultPath() const;
protected:
	// To use the ECam Test Plug-in
	void CreateDependencyFileL();
   	void RemoveDependencyFile();	
private:
	void InitSystemPath();	
private:
	TFileName iDefaultPath;
	RFs ifsSession;
	};
	
#endif //tsu_mmf_avictrl_testsuite.h
