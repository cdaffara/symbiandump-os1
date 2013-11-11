// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent Internal Symbian test code
*/


#if !defined(__T_PDR_H__)
#define __T_PDR_H__

#include <e32test.h>
#include <pdrport.h>
#include <c32comm.h>
#include <metafile.h>
#include <banddev.h>

#include <test/testexecutestepbase.h>
#include "TGraphicsHarness.h"
#include "prnsetupfacade.h"

class CPdrTestPrint : public CActive
	{
public:
	CPdrTestPrint(): CActive(EPriorityLow) {}
	TRequestStatus& Status();	
	void DoCancel() {}
	TRequestStatus&  Queue(); 
	void RunL(); 
	};

	
class CTPdr : public CTGraphicsBase
	{
public:
	CTPdr(CTestStep* aStep);
	~CTPdr();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	virtual void ConstructL();
private:
	void testPrint(CPrinterDevice* aDevice);
	void testPdrStore();
	void bandTest(TBandingDirection aBandingDirection, TPtrC aName);
	void testPdrBandRect();
private:
	CPdrTestPrint* iStopper;
	TInt iModelIndex;
	TInt iNumModels;
	};



class CTPdrStep : public CTGraphicsStep
	{
public:
	CTPdrStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
	};
	

_LIT(KTPdrStep,"TPdr");

#endif
