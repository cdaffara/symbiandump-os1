// Copyright (c) 1996-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "T_PDRLST.H"


class CTPdrLst : public CTGraphicsBase
	{
public:
	CTPdrLst(CTestStep* aStep);
	~CTPdrLst();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	virtual void ConstructL();
private:
	void Test1();
private:
	CPdrModelList* iModelList;
	};

CTPdrLst::CTPdrLst(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	
	}

CTPdrLst::~CTPdrLst()
	{
	// clean up
	INFO_PRINTF1( _L( "- Destroy the CPdrModelList" ) );
	delete iModelList;
	}

void CTPdrLst::ConstructL()
	{
	INFO_PRINTF1( _L( "Testing PdrList" ) );
	INFO_PRINTF1( _L( "- Create a CPdrModelList" ) );
	iModelList = CPdrModelList::NewL();
	}


void CTPdrLst::Test1()
	{
	INFO_PRINTF1( _L( "- Check that there are no models in the list" ) );
	TInt numModels = iModelList->ModelCount();
	TEST(numModels == 0);

	INFO_PRINTF1( _L( "- Add a directory to the search path" ) );
	TFileName dir( _L( "z:" ) );
	dir.Append( KDefaultPrinterDriverPath );
	TRAPD(ret, iModelList->AddDirectoryL(dir) ); 
	INFO_PRINTF2( _L( "ret %d\n" ), ret );
	TEST( ret == KErrNone );

	INFO_PRINTF1( _L( "- Scan the search path for models" ) );
	TRAP( ret, (void)iModelList->ScanForModelsL() );
	TEST( ret == KErrNone );
	numModels = iModelList->ModelCount();
	INFO_PRINTF2( _L( "   Models found: %d\n" ), numModels );

	TPrinterModelEntry entry;
	for ( TInt i = 0; i < numModels; i++ )
		{
		entry = (*iModelList)[i];
		INFO_PRINTF2( _L( "ModelName %S\n" ), &entry.iModelName );
		}

	INFO_PRINTF1( _L( "- Create a new printer driver" ) );
	RDebug::Print( _L( "Drivers: %d\r\n" ), numModels );
	CPrinterDriver* driver = NULL;
	TRAP( ret, driver = iModelList->CreatePrinterDriverL( numModels - 1 ) );
	TEST( ret == KErrNone );
	
	INFO_PRINTF1( _L( "- Get the model name of the new printer device from PdrStore" ) );
	TBuf<256> buf = driver->PrinterDevice()->Model().iModelName.Ptr();
	INFO_PRINTF2( _L( "   Name of current model: %S\n" ), &buf );

	// tidy up
	delete driver;
	}

void CTPdrLst::RunTestCaseL(TInt aCurTestCase)
// Test the model list interface to PdrStore.
//
    {
	((CTPdrLstStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
/**
@SYMTestCaseID			GRAPHICS-PDRLST-0001
*/
		((CTPdrLstStep*)iStep)->SetTestStepID(_L("GRAPHICS-PDRLST-0001"));
		Test1();
		break;	
	case 2:
		((CTPdrLstStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTPdrLstStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;	
		}
	((CTPdrLstStep*)iStep)->RecordTestResultL();
    }

//--------------
__CONSTRUCT_STEP__(PdrLst)


