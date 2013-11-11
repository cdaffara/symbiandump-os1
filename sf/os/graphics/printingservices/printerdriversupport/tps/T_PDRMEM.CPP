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

#include "T_PDRMEM.H"

_LIT( KDriveName, "z:" );
_LIT( KEpsonFileName, "epson.pdr" );


class CTPdrMem : public CTGraphicsBase
	{
public:
	CTPdrMem(CTestStep* aStep);
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void TestNewL();
	void TestOpenPdrL();
	void TestCreatePrinterDeviceL();
	};

CTPdrMem::CTPdrMem(CTestStep* aStep) :
	CTGraphicsBase(aStep)
	{
	
	}

void CTPdrMem::RunTestCaseL(TInt aCurTestCase)
	{
	TInt ret;
	((CTPdrMemStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:
/**
@SYMTestCaseID			GRAPHICS-PDRMEM-0001
*/
		((CTPdrMemStep*)iStep)->SetTestStepID(_L("GRAPHICS-PDRMEM-0001"));
		TRAP(ret,TestNewL());
		TEST(ret==KErrNone);
		break;
	case 2:
/**
@SYMTestCaseID			GRAPHICS-PDRMEM-0002
*/
		((CTPdrMemStep*)iStep)->SetTestStepID(_L("GRAPHICS-PDRMEM-0002"));
		TRAP(ret,TestOpenPdrL());
		TEST(ret==KErrNone);
		break;
	case 3:
/**
@SYMTestCaseID			GRAPHICS-PDRMEM-0003
*/
		((CTPdrMemStep*)iStep)->SetTestStepID(_L("GRAPHICS-PDRMEM-0003"));
		TRAP(ret,TestCreatePrinterDeviceL());
		TEST(ret==KErrNone);
		break;
	case 4:
		((CTPdrMemStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
//		TRAP(ret,testGetNearestFontToDesignHeightInTwips());
//		TEST(ret==KErrNone);
		break;
	case 5:
		((CTPdrMemStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTPdrMemStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTPdrMemStep*)iStep)->RecordTestResultL();
	}

void CTPdrMem::TestNewL()
	{
	INFO_PRINTF1(_L("NewL"));
	
	TInt ret=KErrGeneral;
	CPrinterDriver *driver=NULL;
	TInt failRate=1;

	while (ret!=KErrNone)
		{
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
		__UHEAP_MARK;
		TRAP(ret,driver=CPrinterDriver::NewL());
		RDebug::Print(_L("driver %d failrate %d\n"),driver,failRate);
		if (ret==KErrNoMemory)
			{
			__UHEAP_MARKEND;
			__UHEAP_RESET;
			TEST(driver==NULL);
			}
		else if(ret==KErrNone)
			{
			delete driver;
			__UHEAP_MARKEND;
			__UHEAP_RESET;
			__UHEAP_SETFAIL(RHeap::ENone,failRate);
			}
		else
			User::Panic(_L("Unknown error"),ret);
		failRate++;
		}
	}

void CTPdrMem::TestOpenPdrL()
	{
	INFO_PRINTF1(_L("OpenPdrL"));
	TInt failRate=1;
	CPrinterDriver* driver=CPrinterDriver::NewL();
	TFileName filename( KDriveName );
	filename.Append( KDefaultPrinterDriverPath );
	filename.Append( KEpsonFileName );
	driver->OpenPdrL( filename );
	driver->Close();
	TInt ret=KErrGeneral;

	while (ret!=KErrNone)
		{
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
		__UHEAP_MARK;
		TRAP(ret,driver->OpenPdrL( filename ));
		RDebug::Print(_L("NumModels %d failrate %d\n"),driver->NumModels(), failRate);
		if (ret==KErrNoMemory)
			{
			driver->Close();
			__UHEAP_MARKEND;
			__UHEAP_RESET;
			TEST(driver->NumModels()==0);
			}
		else if(ret==KErrNone)
			{
			driver->Close();
			__UHEAP_MARKEND;
			__UHEAP_RESET;
			__UHEAP_SETFAIL(RHeap::ENone,failRate);
			}
		else
			User::Panic(_L("Unknown error"),ret);
		failRate++;
		}
			
	delete driver;

	}

void CTPdrMem::TestCreatePrinterDeviceL()
	{

	INFO_PRINTF1(_L("CreatePrinterDeviceL"));
	TInt failRate=1;
	CPrinterDriver* driver=CPrinterDriver::NewL();
	TFileName filename( KDriveName );
	filename.Append( KDefaultPrinterDriverPath );
	filename.Append( KEpsonFileName );
	driver->OpenPdrL( filename );
	TPrinterModelEntry entry;
	entry=driver->Model(0);
	RDebug::Print(_L("Drivers: %d\r\n"), driver->NumModels());
	RDebug::Print(_L("Model %d: %d\r\n"), 0, entry.iUid.iUid);
	CPrinterDevice* device=NULL;
	TInt ret=KErrGeneral;

	while(ret!=KErrNone)
		{
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
		__UHEAP_MARK;
		TRAP(ret,device=driver->CreatePrinterDeviceL(entry.iUid));
		RDebug::Print(_L("device %d failrate %d\n"),device, failRate);
		if (ret==KErrNoMemory)
			{
			__UHEAP_MARKEND;
			__UHEAP_RESET;
			TEST(device==NULL);
			}
		else if	(ret==KErrNone)
			{
			__UHEAP_RESET;
			__UHEAP_SETFAIL(RHeap::ENone,failRate);
			}
		else
			User::Panic(_L("Unknown error"),ret);
		failRate++;
		}

	delete driver;
  
	}

//This test is useless. 
//It fails every time when calling device->GetNearestFontToDesignHeightInTwips().
//device->GetNearestFontToDesignHeightInTwips() calls CFbsDrvDevice::LoadTypeFaceList()
//which allocates additional memory for iGenTypefaceFontsType between
//__UHEAP_MARK and __UHEAP_MARKEND macros.
//CFbsDrvDevice takes care of deleting iGenTypefaceFontsType in its destructor.
/*
LOCAL_C void testGetNearestFontToDesignHeightInTwips()
	{

	INFO_PRINTF1(_L("GetNearestFontToDesignHeightInTwips"));
	CPrinterDriver* driver=CPrinterDriver::NewL();
	driver->OpenPdrL(_L("z:\\system\\printers\\epson.pdr"));
	TInt failRate=1;
	CFont *font;
	TFontSpec spec;
	spec.iTypeface.iName=_L("NewCourier");
	spec.iHeight=100;
	spec.iFontStyle.SetPosture(EPostureUpright);
	spec.iFontStyle.SetStrokeWeight(EStrokeWeightNormal);
	spec.iFontStyle.SetPrintPosition(EPrintPosNormal);
	TPrinterModelEntry entry;
	entry=driver->Model(0);
	CPrinterDevice* device=NULL;
	TRAPD(ret,device=driver->CreatePrinterDeviceL(entry.iUid));
	TEST(ret==KErrNone);
	ret=device->GetNearestFontToDesignHeightInTwips((CFont *&) font,spec);
	TEST(ret==KErrNone);
	device->ReleaseFont(font);

	ret=KErrGeneral;

	while(ret!=KErrNone)
		{
		__UHEAP_RESET;
		__UHEAP_SETFAIL(RHeap::EDeterministic,failRate);
		__UHEAP_MARK;
		font=NULL;
		ret = device->GetNearestFontToDesignHeightInTwips((CFont *&) font,spec);
		INFO_PRINTF2(_L("Font %d failrate %d\n"),font,failRate);
		if (ret==KErrNoMemory)
			{
			__UHEAP_MARKEND;
			__UHEAP_RESET;
			TEST(font==NULL);
			}
		else if	(ret==KErrNone)
			{
//			device->ReleaseFont(font);
			font=NULL;
			__UHEAP_RESET;
			}
		else
			User::Panic(_L("Unknown error"),ret);
		failRate++;
		}

	
	delete driver;

	test.End();
	}
*/

//--------------
__CONSTRUCT_STEP__(PdrMem)

