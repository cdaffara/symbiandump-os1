// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#include "tspriteperf.h"

const TInt KIterationsToTest = 60; 		// Number of iterations to run tests

RTAnim::RTAnim() : RAnim()
	{
	}

RTAnim::RTAnim(RAnimDll& aDll) : RAnim(aDll)
	{
	}

CTSpritePerf::~CTSpritePerf()
	{
	}

CTSpritePerf::CTSpritePerf()
	{
	SetTestStepName(KTSpritePerfName);
	}

/**
Override of base class virtual

@return - TVerdict code
*/
TVerdict CTSpritePerf::doTestStepPreambleL()
	{
	User::LeaveIfError(iWs.Connect());
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
		
	return TestStepResult();
	}


TVerdict CTSpritePerf::doTestStepPostambleL()
	{
	iWs.Close();
		
	return TestStepResult();
	}

/**	Override of base class pure virtual
	Our implementation only gets called if the base class doTestStepPreambleL() did
	not leave. That being the case, the current test result value will be EPass.

	@return - TVerdict code
*/
TVerdict CTSpritePerf::doTestStepL()
	{
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0128"));
	SpriteAnimOverWholeScreenL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0129"));
	SpriteAnimUnderTranslucentWindowsL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0130"));
	SpriteAnimWithSemitransparentMaskOverWholeScreenL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0131"));
	SpriteAnimWithSemitransparentMaskUnderTranslucentWindowsL();
	RecordTestResultL();

	SetTestStepID(_L("GRAPHICS-UI-BENCH-0143"));
	OpaqueFloatingSpriteNonOverlapUpdateAreaL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0144"));
	SemitransparentFloatingSpriteNonOverlapUpdateAreaL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0145"));
	OpaqueFloatingSpriteOverlapUpdateAreaL();
	RecordTestResultL();
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0146"));
	SemitransparentFloatingSpriteOverlapUpdateAreaL();
	RecordTestResultL();
	return TestStepResult();
	}

/** Set up the window server environment and load the animation DLL which will be used for 
  	the sprite performance test cases.
    The RWsSession is connected to the window server, the CWsScreenDevice and the RWindowGroup
    members are constructed. Also the client-side interface to the server-side animation DLL is
    constructed and the animation DLL is loaded. 
 */
void CTSpritePerf::SetUpWindowEnvironmentL(RAnimDll* aAnimDll)
	{
	SetScreenModeL(EColor16MA);
	
	if (aAnimDll)
		{
		TInt err = aAnimDll->Load(KAnimDLLName);
		if (err)
			{
			INFO_PRINTF3(_L("DLL file %S was not loaded properly, leave with error code %i"),KAnimDLLName,err);
			User::Leave(err);
			}
		}
	
	iWsScreenDev = new(ELeave) CWsScreenDevice(iWs);
	User::LeaveIfError(iWsScreenDev->Construct());
	TSize screenSize = iWsScreenDev->SizeInPixels();
	
	iWinGroup = new(ELeave) RWindowGroup(iWs);
	User::LeaveIfError(iWinGroup->Construct(reinterpret_cast<TUint32>(iWinGroup),iWsScreenDev));
	}

/** Populate the RWindow array member by constructing RWindow objects. Set the background colour of the windows according to the
   	tranparency flag. Make the windows visible, activate them and draw them.
  
  	@param aUseTransparency the transparency flag of the windows
 */
void CTSpritePerf::ConstructArrayOfWindowsL(TBool aUseTransparency)
	{
	TRect rect(TPoint(0,0),iWsScreenDev->SizeInPixels());
	TInt i;
	TUint32 winID;
	for (i=0;i<ENumWins;++i)
		{
		iWins[i]=new(ELeave) RWindow(iWs);
		winID = reinterpret_cast<TUint32>(iWinGroup+i+1);
		if (i==0)
			User::LeaveIfError(iWins[i]->Construct(*iWinGroup,winID)); //iWinGroup is the parent of iWins[0]
		else
			User::LeaveIfError(iWins[i]->Construct(*iWins[0],winID)); //iWins[0] is the the parent for every subsequent window
		rect.Resize(-rect.Width()/10,-rect.Height()/10);
		iWins[i]->SetExtent(TPoint(0,0),rect.Size());
		if (aUseTransparency)
			{	
			iWins[i]->SetTransparencyAlphaChannel();
			iBackColour[i]= TRgb(240,(200*i)/ENumWins,170-(20*i)/ENumWins,(60*(i+1))/ENumWins); //assign a different background colour to each window in a pseudo-random manner
			iWins[i]->SetBackgroundColor(iBackColour[i]); //semi-transparent window (R,G,B,Alpha)
			}
		else
			{
			iBackColour[i]= TRgb(180,(200*i)/ENumWins,170-(20*i)/ENumWins,255);
			iWins[i]->SetBackgroundColor(iBackColour[i]); //opaque window
			}
			
		iWins[i]->SetVisible(ETrue);
		iWins[i]->Activate();	
		iWins[i]->Invalidate();
		iWins[i]->BeginRedraw();
		iWins[i]->EndRedraw();
		}
			
	iWs.Flush();
	iWs.Finish();
	}

/** Release the resources that the window server environment, the window construction and the client-side
  	interface to animation DLL have allocated.
 */
void CTSpritePerf::ReleaseWindowsAndEnvironment()
	{
	for (TInt i=0;i<ENumWins;++i)
		{
		if (iWins[i])
			iWins[i]->Close();
		delete iWins[i];
		iWins[i]=NULL;
		}
	
	if(iWinGroup)
		iWinGroup->Close();
	delete iWinGroup;
	iWinGroup=NULL;
	
	delete iWsScreenDev;
	iWsScreenDev=NULL;
	}


/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0128

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to draw the bitmap of a sprite over the whole screen

@SYMTestActions
Set up the window server environment, load the animation DLL and construct an array of opaque windows.
Construct an opaque sprite having the window group as parent. Append a single member (appropriately initialised) to it 
and construct a sprite animation linked to the sprite. Over a specific number of iterations perform some 
draw operations on the sprite animation. Record the time the draw requests and the actual drawing require 
and release the resources.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTSpritePerf::SpriteAnimOverWholeScreenL()
	{
	_LIT(KTestName, "SpriteAnimOverWholeScreenL");
	SpriteAnimTestL(KTestName, ETrue, EFalse);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0129

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to draw the bitmap of a sprite under transparent windows

@SYMTestActions
Set up the window server environment, load the animation DLL and construct an array of translucent windows.
Construct an opaque sprite having the bottom window as parent. Append a single member (appropriately initialised) to it.
and construct a sprite animation linked to the sprite. Over a specific number of iterations perform some draw 
operations on the sprite animation. Record the time the draw requests and the actual drawing require and release the
resources.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTSpritePerf::SpriteAnimUnderTranslucentWindowsL()
	{
	_LIT(KTestName, "SpriteAnimUnderTranslucentWindowsL");
	SpriteAnimTestL(KTestName, EFalse, EFalse);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0130

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to draw the bitmap of a sprite over the whole screen, when the bitmap mask is partially transparent

@SYMTestActions
Set up the window server environment, load the animation DLL and construct an array of opaque windows.
Construct a semitransparent sprite having the window group as parent. Append a single member (appropriately initialised)
to it and construct a sprite animation linked to the sprite. Over a specific number of iterations perform some draw
operations on the sprite animation. Record the time the draw requests and the actual drawing require and release the resources.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTSpritePerf::SpriteAnimWithSemitransparentMaskOverWholeScreenL()
	{
	_LIT(KTestName, "SpriteAnimWithSemitransparentMaskOverWholeScreenL");
	SpriteAnimTestL(KTestName, ETrue, ETrue);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0131

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to draw the bitmap of a sprite under translucent windows,  when the bitmap mask is partially transparent

@SYMTestActions
Set up the window server environment, load the animation DLL and construct an array of translucent windows.
Construct a semitransparent sprite having the bottom window as parent. Append a single member (appropriately initialised)
to it and construct a sprite animation linked to the sprite. Over a specific number of iterations perform some draw
operations on the sprite animation. Record the time the draw requests and the actual drawing require and release the resources.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTSpritePerf::SpriteAnimWithSemitransparentMaskUnderTranslucentWindowsL()
	{
	_LIT(KTestName, "SpriteAnimWithSemitransparentMaskUnderTranslucentWindowsL");
	SpriteAnimTestL(KTestName, EFalse, ETrue);
	}

static void CleanupWindows(TAny* aPtr)
	{
	RWindow** wins = static_cast<RWindow**>(aPtr);
	for (TInt i=0;i<CTSpritePerf::ENumWins;++i)
		{
		if (wins[i])
			{
			wins[i]->Close();
			delete wins[i];
			wins[i]=NULL;
			}
		}
	}

static void CleanupWindowGroup(TAny* aPtr)
	{
	RWindowGroup** windowGroup = static_cast<RWindowGroup**>(aPtr);
	if (*windowGroup)
		{
		(*windowGroup)->Close();
		delete *windowGroup;
		*windowGroup=NULL;
		}
	}

static void CleanupScreenDevice(TAny* aPtr)
	{
	CWsScreenDevice** screenDevice = static_cast<CWsScreenDevice**>(aPtr);
	delete *screenDevice;
	*screenDevice=NULL;	
	}

/*
   Set up the window server environment, load the animation DLL and construct an array of (opaque/translucent) windows.
   Construct an opaque/semitransparent sprite with a single member, initialise it and construct a sprite animation linked to the sprite. 
   Over a specific number of iterations perform some draw operations on the sprite animation. Record the time 
   the draw requests and the actual drawing require and release the resources.
  
   @param aTestName the name of the test case
   @param aOverWholeScreen the flag deciding whether the windows are opaque or tranparent and thus deciding whether 
  		  the sprite is drawn over the whole screen or under transparent windows
   @param aTransparentMask the transparency flag of the sprite bitmap mask
 */
void CTSpritePerf::SpriteAnimTestL(const TDesC& aTestName,TBool aOverWholeScreen, TBool aTransparentMask)
	{
	RAnimDll animDll(iWs);
	CleanupClosePushL(animDll);
	
	TCleanupItem cleanupScreenDevice(CleanupScreenDevice,&iWsScreenDev);
	CleanupStack::PushL(cleanupScreenDevice);
	TCleanupItem cleanupWindowGroup(CleanupWindowGroup,&iWinGroup);
	CleanupStack::PushL(cleanupWindowGroup);
	SetUpWindowEnvironmentL(&animDll);
	
	TCleanupItem cleanupWindows(CleanupWindows,iWins);
	CleanupStack::PushL(cleanupWindows);
	ConstructArrayOfWindowsL(!aOverWholeScreen);
	
	RWsSprite sprite(iWs);
	CleanupClosePushL(sprite);
	if (aOverWholeScreen)
		User::LeaveIfError(sprite.Construct(*iWinGroup,TPoint(10,10),ESpriteNoChildClip));
	else
		User::LeaveIfError(sprite.Construct(*iWins[0],TPoint(10,10),ESpriteNoChildClip));

	CFbsBitmap* bitmap=new(ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(TSize(40,42),EColor16MA));

	CFbsBitmap* bitmap2 = NULL; //used as bitmap mask for sprite member of opaque sprite 
	CFbsBitmap* bitmap3 = NULL; //used as bitmap mask for sprite member of semitransparent sprite 
	
	if (!aTransparentMask)
		{ 
		//opaque sprite uses a bitmap mask set to white
		bitmap2=new(ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap2);
		User::LeaveIfError(bitmap2->Create(TSize(40,42),EColor16MA));
		}
	else	
		{ 
		//semitransparent sprite uses a bitmap mask, in which 
		// *the bottom right quarter is set to black making the sprite fully transparent in this area
		// *the remaining region is set to a gray shade allowing the sprite to be semitransparent in this area
		bitmap3=new(ELeave) CFbsBitmap();
		CleanupStack::PushL(bitmap3);
		User::LeaveIfError(bitmap3->Create(TSize(40,42),EColor16MA));
		
		CFbsBitmapDevice *device=CFbsBitmapDevice::NewL(bitmap3);
		CleanupStack::PushL(device);
		CFbsBitGc *gc;
		User::LeaveIfError(device->CreateContext(gc));
		CleanupStack::PushL(gc);

		gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		gc->SetPenSize(TSize());
		TSize bitmapSize=bitmap3->SizeInPixels();
		TSize size=bitmapSize;
		size.SetSize(size.iWidth/2,size.iHeight/2);
		TPoint point=size.AsPoint();
		gc->SetBrushColor(TRgb(128,128,128));
		gc->DrawRect(TRect(TPoint(),bitmapSize));
		gc->SetBrushColor(TRgb(0,0,0));
		gc->DrawRect(TRect(point,size));
		}
			
	TSpriteMember member;
	member.iBitmap=bitmap;
	if (aTransparentMask)
		member.iMaskBitmap=bitmap3;
	else
		member.iMaskBitmap=bitmap2;
	member.iInvertMask=EFalse;
	member.iDrawMode=CGraphicsContext::EDrawModePEN;
	member.iOffset=TPoint();
	member.iInterval=TTimeIntervalMicroSeconds32(200000);
	
	sprite.AppendMember(member);
	
	RTAnim spriteAnim(animDll);
	CleanupClosePushL(spriteAnim);
	TPtrC8 des(NULL,0);
	TPoint pos(10,20);
	User::LeaveIfError(spriteAnim.Construct(sprite,ESpriteAnimType,des));
	User::After(500000);
		
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iProfiler->StartTimer();
		// draw on sprite member's bitmap and update the sprite member
		spriteAnim.Command(EADllDraw1);
		spriteAnim.Command(EADllDraw2);
		spriteAnim.Command(EADllDraw3);
		iWs.Flush();
		iProfiler->MarkResultSetL();
		User::After(100000);
		spriteAnim.Command(EADllDrawBlank);
		iWs.Flush();
		User::After(100000);
		}
	
	iProfiler->ResultsAnalysis(aTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
	
	if (!aTransparentMask)
		CleanupStack::PopAndDestroy(4); //sprite, bitmaps (2), testanim
	else
		CleanupStack::PopAndDestroy(6); //sprite, bitmaps (2), device, gc, testanim
	CleanupStack::Pop(3); //cleanup items(3)
	CleanupStack::PopAndDestroy(1); //anim Dll
	ReleaseWindowsAndEnvironment();
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0143

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to draw the bitmap of an opaque sprite, when it lies over an area that is not updated.

@SYMTestActions
Set up the window server environment and construct an array of opaque windows.
Construct an opaque floating sprite and place it over an area that will not be followingly updated.
Append a single member to it. Over a specific number of iterations update a screen area. Record the time
the actual drawing requires and release the resources.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTSpritePerf::OpaqueFloatingSpriteNonOverlapUpdateAreaL()
	{
	_LIT(KTestName, "OpaqueFloatingSpriteNonOverlapUpdateAreaL");
	FloatingSpriteTestL(KTestName, ETrue, EFalse);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0144

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to draw the bitmap of a semitransparent sprite, when it lies over an area that is not updated.

@SYMTestActions
Set up the window server environment and construct an array of opaque windows.
Construct a semitransparent floating sprite and place it over an area that will not be followingly updated.
Append a single member to it. Over a specific number of iterations update a screen area. Record the time
the actual drawing requires and release the resources.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTSpritePerf::SemitransparentFloatingSpriteNonOverlapUpdateAreaL()
	{
	_LIT(KTestName, "SemitransparentFloatingSpriteNonOverlapUpdateAreaL");
	FloatingSpriteTestL(KTestName, EFalse, EFalse);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0145

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to draw the bitmap of an opaque sprite, when it lies over an area that is updated.

@SYMTestActions
Set up the window server environment and construct an array of opaque windows.
Construct an opaque floating sprite and place it over an area that will be followingly updated.
Append a single member to it. Over a specific number of iterations update a screen area. Record the time
the actual drawing requires and release the resources.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTSpritePerf::OpaqueFloatingSpriteOverlapUpdateAreaL()
	{
	_LIT(KTestName, "OpaqueFloatingSpriteOverlapUpdateAreaL");
	FloatingSpriteTestL(KTestName, ETrue, ETrue);
	}

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-0146

@SYMPREQ PREQ1841

@SYMTestCaseDesc
Tests how long it takes to draw the bitmap of a semitransparent sprite, when it lies over an area that is updated.

@SYMTestActions
Set up the window server environment and construct an array of opaque windows.
Construct a semitransparent floating sprite and place it over an area that will be followingly updated.
Append a single member to it. Over a specific number of iterations update a screen area. Record the time
the actual drawing requires and release the resources.

@SYMTestExpectedResults
Test should pass and display average test time per iteration
*/
void CTSpritePerf::SemitransparentFloatingSpriteOverlapUpdateAreaL()
	{
	_LIT(KTestName, "SemitransparentFloatingSpriteOverlapUpdateAreaL");
	FloatingSpriteTestL(KTestName, EFalse, ETrue);
	}

/*
   Set up the window server environment and construct an array of opaque windows.
   Construct an opaque/semitransparent sprite attached to the window group, thus being a floating sprite.
   Append a single member to the sprite. Over a specific number of iterations update a screen area by rotating
   the background colour of the windows (excluding top window). Record the time the actual drawing requires
   in total (for redrawing the windows and the sprite) and release the resources.
  
   @param aTestName the name of the test case
   @param aIsOpaqueSprite the flag deciding whether the sprite is opaque or semitranparent
   @param aOverlapUpdateArea the flag deciding whether the sprite overlaps the update area or not
 */
void CTSpritePerf::FloatingSpriteTestL(const TDesC& aTestName, TBool aIsOpaqueSprite, TBool aOverlapUpdateArea)
	{
	TCleanupItem cleanupScreenDevice(CleanupScreenDevice,&iWsScreenDev);
	CleanupStack::PushL(cleanupScreenDevice);
	TCleanupItem cleanupWindowGroup(CleanupWindowGroup,&iWinGroup);
	CleanupStack::PushL(cleanupWindowGroup);
	SetUpWindowEnvironmentL(NULL);
	
	TCleanupItem cleanupWindows(CleanupWindows,iWins);
	CleanupStack::PushL(cleanupWindows);
	ConstructArrayOfWindowsL(EFalse);
	
	RWsSprite sprite(iWs);
	CleanupClosePushL(sprite);
	TPoint point=TPoint();
	if (aOverlapUpdateArea)
		{
		//sprite is set to be right over the area off the top window, where the other windows overlap
		TSize topWinSize= iWins[ENumWins-1]->Size();
		point=TPoint(0,topWinSize.iHeight);
		}
	User::LeaveIfError(sprite.Construct(*iWinGroup,point,ESpriteNoChildClip));
		
	CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(iWsScreenDev->SizeInPixels(), iWsScreenDev->DisplayMode()));
	User::LeaveIfError(bitmap->Load(TEST_BITMAP_NAME,0));
	TSize bitmapSize=bitmap->SizeInPixels();
	
	CFbsBitmap* bitmap2 = new(ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap2);
	User::LeaveIfError(bitmap2->Create(bitmapSize, iWsScreenDev->DisplayMode())); //blank bitmap
	
	TSpriteMember member;
		
	member.iBitmap=bitmap;
	if (aIsOpaqueSprite)
		member.iMaskBitmap=bitmap2;
	else
		member.iMaskBitmap=bitmap;
	member.iInvertMask=EFalse;
	member.iDrawMode=CGraphicsContext::EDrawModePEN;
	member.iOffset=TPoint();
	member.iInterval=TTimeIntervalMicroSeconds32(200000);
	User::LeaveIfError(sprite.AppendMember(member));
	User::LeaveIfError(sprite.Activate());  //make the sprite visible
	User::After(1000000);
	
	iProfiler->InitResults();
	for (TInt count=KIterationsToTest; count>=0; --count)
		{
		iProfiler->StartTimer();
		//cause a screen area to be updated by rotating the background colour of the windows in the array 
		//apart from the colour of the top window  
		for (TInt i=0;i<ENumWins-1;i++)
			{
			TRgb tempBackColour = iBackColour[0];
			if (i!=ENumWins-1)
				iBackColour[i]=iBackColour[(i+1)%(ENumWins-1)];
			else
				iBackColour[ENumWins-2]=tempBackColour;
			iWins[i]->SetBackgroundColor(iBackColour[i]);
			iWins[i]->Invalidate();
			iWins[i]->BeginRedraw();
			iWins[i]->EndRedraw();
			}
		iWs.Flush();
		iWs.Finish();
		iProfiler->MarkResultSetL();
		User::After(100000);
		}
		
	iProfiler->ResultsAnalysis(aTestName, 0, EColor16MA, EColor16MA, KIterationsToTest);
	
	CleanupStack::PopAndDestroy(3); //sprite, bitmaps(2)
	CleanupStack::Pop(3); //cleanup items(3)
	ReleaseWindowsAndEnvironment();
	}
