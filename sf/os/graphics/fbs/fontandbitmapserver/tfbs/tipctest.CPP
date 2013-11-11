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

#include "../sfbs/UTILS.H"
#include "tipctest.h"
#include "fbsdefs.h"
#include "fbsmessage.h"

// Invalid parameters to be passed in SendCommand() calls.
const TInt KInvalidParam1 = 963955448;
const TInt KInvalidParam2 = 963955368;
const TInt KInvalidParam3 = 963955300;
const TInt KInvalidParam4 = 963955232;

CTIPCTest::CTIPCTest(CTestStep* aStep):
	CTGraphicsBase(aStep),
	iFbs(NULL),
	iFs(NULL)
	{}

CTIPCTest::~CTIPCTest()
	{
	if(iShutdownFbs)
		RFbsSession::GetSession()->SendCommand(EFbsMessShutdown);
	RFbsSession::Disconnect();
	delete iFs;
	iHeap->Close();
	}

void CTIPCTest::ConstructL()
	{
	INFO_PRINTF1(_L("Font/Bitmap Server IPC Messages Testing"));

	if(RFbsSession::Connect()==KErrNone)
		{
		RFbsSession::Disconnect();
		iShutdownFbs = EFalse;
		}
	else
		{
		FbsStartup();
		iShutdownFbs = ETrue;
		}
	RFbsSession::Connect();
	iFbs=RFbsSession::GetSession();
	
	if(!iShutdownFbs)
		{
		if(iClient.Connect() == KErrNone)
			{
			iShutdownFbs = EFalse;
			}
		else
			{
			iShutdownFbs = ETrue;
			}
		}
		
	iHeap=UserHeap::ChunkHeap(NULL,0x10000,0x10000);
	TRAPD(ret,iFs=CFontStore::NewL(iHeap));
	TEST(ret==KErrNone);
	TEST(iFs != NULL);
	
	}

void CTIPCTest::RunTestCaseL(TInt aCurTestCase)
	{
	_LIT(KCaseNumber, "CaseNumber");
	
	TBool res = iStep->GetIntFromConfig(iStep->ConfigSection(), KCaseNumber, aCurTestCase);
	
	if(!res)
		{
		return;
		}
		
	switch(aCurTestCase)
		{
	/**
		@SYMTestCaseID
		GRAPHICS-FBSERV-0603

		@SYMTestCaseDesc
		Tests the sending of commands to a FbsSession 
		server thru Inter Process Communication. Sends
		both valid and invalid commands.

		@SYMTestActions
		Commands sent to the server:
		1. EFbsMessShutdown
		2. EFbsMessFontHeightInTwips
		3. EFbsMessFontHeightInTwips
		4. EFbsMessFontHeightInPixels
		5. EFbsMessFontHeightInPixels
		6. EFbsMessAddFontStoreFile
		7. EFbsMessAddFontStoreFile
		8. EFbsMessInstallFontStoreFile
		9. EFbsMessInstallFontStoreFile
		10. EFbsMessInstallFontStoreFile
		11. EFbsMessBitmapCreate
		12. EFbsMessRasterize
		13. EFbsMessFaceAttrib
		14. EFbsMessHasCharacter
		15. EFbsMessFontNameAlias
		16. EFbsMessGetNearestFontToDesignHeightInTwips
		17. EFbsMessGetNearestFontToMaxHeightInTwips
		18. EFbsMessGetNearestFontToDesignHeightInPixels
		19. EFbsMessGetNearestFontToMaxHeightInPixels
		20. EFbsMessShapeText
		21. EFbsMessShapeDelete
		22. EFbsMessSetTwipsHeight
		23. EFbsMessGetTwipsHeight
		
		@SYMTestExpectedResults
		Test should pass
	*/
	case 1:
		INFO_PRINTF1(_L("Shutdown"));
		iFbs->SendCommand(EFbsMessShutdown);
		TestComplete();
		break;
	case 2:
		INFO_PRINTF1(_L("Height in twips with negative typeface index"));
		iFbs->SendCommand(EFbsMessFontHeightInTwips, -1);	
		TEST(EFalse); // Previous line should have paniced 
		TestComplete();
		break;	
	case 3:
		INFO_PRINTF1(_L("Height in twips with invalid size"));
		iClient.SendInvalidSize(EFbsMessFontHeightInTwips);
		TEST(EFalse);
		TestComplete();
		break;
	case 4:
		INFO_PRINTF1(_L("Height in pixels with negative typeface index"));
		iFbs->SendCommand(EFbsMessFontHeightInPixels, -1);
		TEST(EFalse);
		TestComplete();
		break;
	case 5:
		INFO_PRINTF1(_L("Height in pixels with invalid size"));
		iClient.SendInvalidSize(EFbsMessFontHeightInPixels);
		TEST(EFalse);
		TestComplete();
		break;
	case 6:
		INFO_PRINTF1(_L("Add font store file with negative length"));
		iFbs->SendCommand(EFbsMessAddFontStoreFile, 0, -1);
		TEST(EFalse);
		TestComplete();
		break;
	case 7:
		INFO_PRINTF1(_L("Add font store file with huge length"));
		iFbs->SendCommand(EFbsMessAddFontStoreFile, KInvalidParam1, KInvalidParam2, KInvalidParam3, KInvalidParam4);
		TEST(EFalse);
		TestComplete();
		break;
	case 8:
		INFO_PRINTF1(_L("Install font store file with negative length"));
		iFbs->SendCommand(EFbsMessInstallFontStoreFile, 0, -1);
		TEST(EFalse);
		TestComplete();
		break;
	case 9:
		INFO_PRINTF1(_L("Install font store file with huge length"));
		iFbs->SendCommand(EFbsMessInstallFontStoreFile, KInvalidParam1, KInvalidParam2, KInvalidParam3, KInvalidParam4);
		TEST(EFalse);
		TestComplete();
		break;
	case 10:
		INFO_PRINTF1(_L("Remove font store file with active references"));
		iFbs->SendCommand(EFbsMessRemoveFontStoreFile, 11);
		// With PDEF121246 (INC120690) fix in place, this test will always pass. Without the fix there will be a KERN 4 panic.
		TEST(ETrue); 
		TestComplete();
		break;
	case 11:
		INFO_PRINTF1(_L("Bitmap create with invalid display mode"));
		iClient.SendInvalidDisplayMode(EFbsMessBitmapCreate);
		TEST(EFalse);
		TestComplete();
		break;
	case 12:
		INFO_PRINTF1(_L("Rasterize with invalid bitmap handle"));
		iFbs->SendCommand(EFbsMessRasterize, KInvalidParam1, KInvalidParam2, KInvalidParam3, KInvalidParam4);
		TEST(EFalse);
		TestComplete();
		break;
	case 13:
		INFO_PRINTF1(_L("Face Attribute with invalid bitmap handle"));
		iFbs->SendCommand(EFbsMessFaceAttrib, KInvalidParam1, KInvalidParam2, KInvalidParam3, KInvalidParam4);
		TEST(EFalse);
		TestComplete();
		break;
	case 14:
		INFO_PRINTF1(_L("Has Character with invalid bitmap handle"));
		iFbs->SendCommand(EFbsMessHasCharacter, KInvalidParam1, KInvalidParam2, KInvalidParam3, KInvalidParam4);
		TEST(EFalse);
		TestComplete();
		break;
	case 15:
		INFO_PRINTF1(_L("Name Alias with huge alias name length"));
		iFbs->SendCommand(EFbsMessFontNameAlias, KInvalidParam1, KInvalidParam2, KInvalidParam3, KInvalidParam4);
		TEST(EFalse);
		TestComplete();
		break;
	case 16:
		INFO_PRINTF1(_L("Get nearest font to design height in twips with invalid typeface name length"));
		iClient.SendInvalidNameLength(EFbsMessGetNearestFontToDesignHeightInTwips);
		TEST(EFalse);
		TestComplete();
		break;
	case 17:
		INFO_PRINTF1(_L("Get nearest font to max height in twips with invalid typeface name length"));
		iClient.SendInvalidNameLength(EFbsMessGetNearestFontToMaxHeightInTwips);
		TEST(EFalse);
		TestComplete();
		break;
	case 18:
		INFO_PRINTF1(_L("Get nearest font to design height in pixels with invalid typeface name length"));
		iClient.SendInvalidNameLength(EFbsMessGetNearestFontToDesignHeightInPixels);
		TEST(EFalse);
		TestComplete();
		break;
	case 19:
		INFO_PRINTF1(_L("Get nearest font to max height in pixels with invalid typeface name length"));
		iClient.SendInvalidNameLength(EFbsMessGetNearestFontToMaxHeightInPixels);
		TEST(EFalse);
		TestComplete();
		break;
	case 20:
		INFO_PRINTF1(_L("Shape text with invalid bitmap font handle"));
		iFbs->SendCommand(EFbsMessShapeText, KInvalidParam1, KInvalidParam2, KInvalidParam3, KInvalidParam4);
		TEST(EFalse);
		TestComplete();
		break;
	case 21:
		INFO_PRINTF1(_L("Shape delete with invalid bitmap font handle"));
		iFbs->SendCommand(EFbsMessShapeDelete, KInvalidParam1, KInvalidParam2, KInvalidParam3, KInvalidParam4);
		TEST(EFalse);
		TestComplete();
		break;
	case 22:
		INFO_PRINTF1(_L("Set twips height with invalid font handle"));
		iFbs->SendCommand(EFbsMessSetTwipsHeight, KInvalidParam1, KInvalidParam2, KInvalidParam3, KInvalidParam4);
		TEST(EFalse);
		TestComplete();
		break;
	case 23:
		INFO_PRINTF1(_L("Get twips height with invalid font handle"));
		iFbs->SendCommand(EFbsMessGetTwipsHeight, KInvalidParam1, KInvalidParam2, KInvalidParam3, KInvalidParam4);
		TEST(EFalse);
		TestComplete();
		break;	
	case 24:
		/**
			@SYMTestCaseID	GRAPHICS-FBSERV-0630

			@SYMTestCaseDesc
			Tests that invalid font handle to EFbsMessGetGlyphs causes panic

			@SYMTestExpectedResults
			Client should panic with FBSERV -8
		*/
		INFO_PRINTF1(_L("Get glyphs from glyph atlas with invalid font handle"));
		iFbs->SendCommand(EFbsMessGetGlyphs, KInvalidParam1, KInvalidParam2, KInvalidParam3, KInvalidParam4);
		TEST(EFalse);
		TestComplete();
		break;
	case 25:
		/**
			@SYMTestCaseID	GRAPHICS-FBSERV-0670

			@SYMTestCaseDesc
			Tests that invalid font handle to EFbsMessGetGlyphMetrics causes panic

			@SYMTestExpectedResults
			Client should panic with FBSERV -8
		*/
		INFO_PRINTF1(_L("Get glyph metrics with invalid font handle"));
		iFbs->SendCommand(EFbsMessGetGlyphMetrics, KInvalidParam1, KInvalidParam2, KInvalidParam3, KInvalidParam4);
		TEST(EFalse);
		TestComplete();
		break;
	/**
		@SYMTestCaseID
		TI18N-GDI-CIT-4086

		@SYMTestCaseDesc
		Test if FBS can handle invalid messages correctly
		and panic the client as needed.

		@SYMTestActions
		26. EFbsMessGetFontTable
		27. EFbsMessReleaseFontTable
		28. EFbsMessGetGlyphOutline
		29. EFbsMessReleaseGlyphOutline
	
		@SYMTestExpectedResults
		Test should pass
	*/
	case 26:
		INFO_PRINTF1(_L("Get Font Table with wrong msg"));
		iFbs->SendCommand(EFbsMessGetFontTable, KInvalidParam1, KInvalidParam2, KInvalidParam3, KInvalidParam4);
		TEST(EFalse); 
		TestComplete();
		break;
	case 27:
		INFO_PRINTF1(_L("Release Font Table with wrong msg"));
		iFbs->SendCommand(EFbsMessReleaseFontTable, KInvalidParam1, KInvalidParam2, KInvalidParam3, KInvalidParam4);
		TEST(EFalse); 
		TestComplete();
		break;
	case 28:
		INFO_PRINTF1(_L("Get Glyph outline with wrong msg"));
		iFbs->SendCommand(EFbsMessGetGlyphOutline,KInvalidParam1, KInvalidParam2, KInvalidParam3, KInvalidParam4);
		TEST(EFalse); 
		TestComplete();
		break;
	case 29:
		INFO_PRINTF1(_L("Release Glyph outline with wrong msg"));
		iFbs->SendCommand(EFbsMessReleaseGlyphOutline, KInvalidParam1, KInvalidParam2, KInvalidParam3, KInvalidParam4);
		TEST(EFalse); 
		TestComplete();
		break;
	case 30:
		TestComplete();
		break;
		}
	}
	
RIPCTestFbservClient::RIPCTestFbservClient()
	{
	
	}
	
RIPCTestFbservClient::~RIPCTestFbservClient()
	{
	
	}
	
TInt RIPCTestFbservClient::Connect(void)
	{
	_LIT(KFbServerName, "!Fontbitmapserver");
	TVersion version(0, 0, 0);
	TInt ret = CreateSession(KFbServerName, version);
	return ret;
	}
	
void RIPCTestFbservClient::SendInvalidSize(TInt aFunctionNumber)
	{
	TPckgBuf<TSize> size;
	size().iHeight = 0;
	size().iWidth = 0;
	TIpcArgs args;
	args.Set(0, 1);
	args.Set(2, &size);
	
	SendReceive(aFunctionNumber, args);
	}
	
void RIPCTestFbservClient::SendInvalidDisplayMode(TInt aFunctionNumber)
	{
	TPckgBuf<TBmpSpec> bmpSpec;
	bmpSpec().iDispMode = EColorLast;
	TIpcArgs args = TIpcArgs();
	args.Set(0, &bmpSpec);
	
	SendReceive(aFunctionNumber, args);
	}

void RIPCTestFbservClient::SendInvalidNameLength(TInt aFunctionNumber)	
	{
	TIpcArgs args = TIpcArgs();
	_LIT(KDefault, "Default");
	TInt size = 7;
	HBufC8 *buf1 = HBufC8::NewLC(size);
	TPtr8 ptr1 = buf1->Des();
	ptr1.Copy(KDefault);
	CleanupStack::PopAndDestroy(buf1);
	args.Set(0, &ptr1);	
	SendReceive(aFunctionNumber, args);
	}


//--------------
__CONSTRUCT_STEP__(IPCTest)

