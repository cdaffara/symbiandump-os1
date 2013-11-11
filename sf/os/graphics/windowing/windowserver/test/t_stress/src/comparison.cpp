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
 @internalComponent
*/
 

#include <e32std.h>
#include <fbs.h>

#include "test_step_logger.h"
#include "comparison.h"
#include "utils.h"
#include "compwin.h"
#include "crpwin.h"
#include "edgedwin.h"
#include "coordinatewin.h"
#include "backedupwin.h"
#include "enormouswin.h"
#include "animatedwin.h"
#include "spritewin.h"
#include "panic.h"


const TInt KSteps = 10;

CComparison::COperationTimer::COperationTimer(CComparison* aComp):CTimer(0), iComp(aComp)
{}

void CComparison::COperationTimer::ConstructL()
	{
	CTimer::ConstructL();
	}

void CComparison::COperationTimer::DoCancel()
	{}

void CComparison::COperationTimer::RunL()
	{
	iComp->Tick();
	}


/*-------------------------------------------------------------------------*/
// COneShotCompare
CComparison::COneShotCompare* CComparison::COneShotCompare::NewL(TInt aPriority, CComparison& aComparison)
	{
	return new(ELeave)CComparison::COneShotCompare(aPriority, aComparison);
	}

CComparison::COneShotCompare::COneShotCompare(TInt aPriority, CComparison& aComparison)
	:CAsyncOneShot(aPriority), iComparison(aComparison)
	{
	}

void CComparison::COneShotCompare::RunL()
	{
	iComparison.iScreen->CopyScreenToBitmap(iComparison.iScreenBitmap);
	iComparison.SetVerifyTick(User::NTickCount());
	iComparison.Verify(iComparison.iScreenBitmap);
	}
/*-------------------------------------------------------------------------*/

_LIT(KConfigurationWindowSuffix, "-Window");

/*******************************************************************************
This is the stresslet itself
*******************************************************************************/
CComparison * CComparison::NewLC(MTestStepReporter& aReporter)
	{
	CComparison * self = new (ELeave) CComparison(aReporter);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CComparison::ConstructL()
	{
	iWatchCat = CTestExecWatchCat::NewL( CExecutionContext::ECtxRandomAndRecord );
	}

CComparison::CComparison (MTestStepReporter& aReporter):
	CStresslet(aReporter), iDifferenceBitmap(NULL), iDifferenceDevice(NULL), iOneShotCompare(NULL)
	{
	}

/**
 Sets stress-test configuration (read from ini file).
 @param aRunData configuration data
 @param aConfFactory a factory interface for creating "section" readers. These
 are used to supply configuration data for each window type.
 */
void CComparison::SetRunDataL(const TRunData& aRunData, MTestStepConfigurationContextFactory* aConfFactory)
	{
	iData = aRunData;
	User::LeaveIfError(iFs.Connect());
	iFs.MkDirAll(iData.loggingPath);
	iWatchCat->SetLoggingPathL(iData.loggingPath);

	//each window type can have its own configuration
	MTestStepConfigurationContext* context; //context created by the factory argument
	RBuf section; //section name (window-type + "-window"
	section.Create(32);
	CleanupStack::PushL(&section);

	struct STestStepConfEntry
		{
		const TPtrC8 iType;	//section name
		void (*iFunc)(const MTestStepConfigurationContext*); //configuration loader
		} confLoaders[] = 
		{
			{KCommonWindowType(),    CCompWin::LoadConfiguration},
			{KAnimatedWindowType(),   CAnimatedWin::LoadConfiguration},
			{KBackedupWindowType(),   CBackedupWin::LoadConfiguration},
			{KCoordinateWindowType(), CCoordinateWin::LoadConfiguration},
			{KCrpWindowType(),        CCrpWin::LoadConfiguration},
			{KEdgedWindowType(),      CEdgedWin::LoadConfiguration},
			{KEnormousWindowType(),   CEnormousWin::LoadConfiguration},
			{KSpritedWindowType(),    CSpritedWin::LoadConfiguration}
		};

	for (int i=0; i<sizeof(confLoaders)/sizeof(STestStepConfEntry); i++)
		{
		section.Copy(confLoaders[i].iType);
		section.Append(KConfigurationWindowSuffix);
		context = aConfFactory->GetConfigurationContextLC(section);
		confLoaders[i].iFunc(context);
		CleanupStack::PopAndDestroy(context);
		}
	
	CleanupStack::Pop(&section);
	}

void CComparison::StartL()
	{
	iStartTime.UniversalTime();
	if (iData.randomSeed > -1)
		{
		//use seed from ini file
		iFirstSeed = iData.randomSeed;
		}
	else 
		{
		// randomize seed by time
		iFirstSeed = iStartTime.Int64();
		}
	TRnd::SetSeed(iFirstSeed);
	
	iWasOk = ETrue;
	iMode = EAct;
	iErrorNum = 0;
	iWinGroup=new(ELeave) RWindowGroup(Session());
	User::LeaveIfError(iWinGroup->Construct( (TUint32)(iWinGroup) ));
	iWinGroup->SetOrdinalPosition(0,0);

	//make sure at least minNumWindows are created, or if it's zero no limit
	iNumWindowsLeft = iData.minNumWindows;

	iScreen = new (ELeave) CWsScreenDevice(Session());	
	User::LeaveIfError(iScreen->Construct(0));
	TDisplayMode screenMode = iScreen->DisplayMode();
	
	iScreenBitmap = new (ELeave) CFbsBitmap;
	iScreenBitmap->Create(TSize(iData.windowWidth, iData.windowHeight), screenMode);
	
	iBackground = new(ELeave) RBlankWindow(Session());
	iBackground->Construct(*iWinGroup,reinterpret_cast<TUint32>(iBackground));
	iBackground->SetColor(KRgbBlack);
	iBackground->SetExtent(TPoint(0,0), TSize(iData.windowWidth, iData.windowHeight));
	iBackground->SetOrdinalPosition(0);
	iBackground->Activate();

	iCurrentBmp = 0;
	iLastBmp = 1;
	iBmpGc = CFbsBitGc::NewL();
	for (TInt bmp = 0; bmp < 2; ++bmp)
		{
		iBitmap[bmp] = new (ELeave) CFbsBitmap;
		iBitmap[bmp]->Create(TSize(iData.windowWidth, iData.windowHeight), screenMode); //EColor16MU); //
		iDevice[bmp] = CFbsBitmapDevice::NewL(iBitmap[bmp]);
		//clear bitmap background
		iBmpGc->Activate(iDevice[bmp]);
		iBmpGc->Reset();
		iBmpGc->SetPenStyle(CGraphicsContext::ENullPen);
		iBmpGc->SetBrushColor(KRgbBlack);
		iBmpGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		TPoint origin(0,0);
		TRect rect(origin, TSize(iData.windowWidth, iData.windowHeight));
		iBmpGc->DrawRect(rect);
		}

	iDifferenceBitmap = new (ELeave) CFbsBitmap;
	iDifferenceBitmap->Create(TSize(iData.windowWidth, iData.windowHeight), screenMode); //EColor16MU); //
	iDifferenceDevice = CFbsBitmapDevice::NewL(iDifferenceBitmap);

	iTimer = new(ELeave) COperationTimer(this);
	iTimer->ConstructL();
	CActiveScheduler::Add (iTimer);
	iTimer->After(iData.initPeriod);
	}

/**
 Simulate a Left key stroke
 */
void CComparison::Touch()
	{
	User::ResetInactivityTime();

	TRawEvent keyDown;  
	keyDown.Set(TRawEvent::EKeyDown,EStdKeyLeftArrow);
	Session().SimulateRawEvent(keyDown);
	
	TRawEvent keyUp;
	keyUp.Set(TRawEvent::EKeyUp,EStdKeyLeftArrow);
	Session().SimulateRawEvent(keyUp);
	}

/**
 Non leaving version of TickL that reports failures
 */
TInt CComparison::Tick()
	{
	TRAPD(err, TickL());
	if (err)
		{
		REPORT_EVENT( EFalse );
		}
	return err;
	}

/**
 Called periodically 
 */
void CComparison::TickL()
	{
	if (iMustConclude)
		{
		WriteLog();
		ConcludeNow();
		return;
		}
	if (0 == iTestNum % 100)
		{
		Touch();
		}
	DoStuffL();
	if (iStuffDone)
		{
		++iTestNum;
		}
	}

/**
 Dispatch work to selected operation by mode
 */
void CComparison::DoStuffL()
	{
	iStuffDone = EFalse;
	switch (iMode)
		{
		case EAct:
			ActL();
			break;
		case EMove:
			MoveL();
			break;
		case EResize:
			ResizeL();
			break;
		};
	if (iWasOk && iData.compareBitmaps && iStuffDone)
		{
		Session().Flush();
		// schedule a delayed compare, will take place after all updates to windows
		if (iOneShotCompare == NULL)
			{
			iOneShotCompare = CComparison::COneShotCompare::NewL(EPriorityMuchLess, *this);
			}
		iOneShotCompare->Call();
		}
	else
		{
		if (iTestNum > iData.maxRunCycles && iNumWindowsLeft == 0)
			{
			iMustConclude = ETrue;
			}
		iTimer->After(iData.period);
		}
	Session().Flush();

	Session().Finish(); 
	}
	
/**
 Performs a random operation on a random window
 Operation can be one of create, destroy, move, bring to front/back,
 resize, tick, toggle-visibility
 */
void CComparison::ActL()
	{
	TInt action = TRnd::rnd(EACount);
	if (iWindows.Count() == 0)
		{
		action = 0;
		}
	
	iBehaviour.iWin = RandomWindow();
	iAct = static_cast<TAct>(action);
	switch(iAct)
		{
		case EACreate:
			CreateWindowL();
			break;
		case EADestroy:
			DestroyWindow();
			break;
		case EAMove:
			MoveWindow();
			break;
		case EAFront:
			BringWindowToFrontL();
			break;
		case EABack:
			SendWindowToBackL();
			break;
		case EAResize:
			ResizeWindow();
			break;
		case EATick:
			TickWindowL();
			break;
		case EAToggleVisible:
			ToggleVisible();
			break;
		} //lint !e787 enum constant TAct::EACount not used within switch
	}

void CComparison::MoveL()
	{
	if (iBehaviour.iCount < 1)
		{
		iMode = EAct;
		}
	else
		{
		TPoint pos = iBehaviour.iWin->Pos() + iBehaviour.iPos;
		iBehaviour.iWin->SetPos(pos);
		--iBehaviour.iCount;
		iStuffDone = ETrue;
		}
	}

void CComparison::ResizeL()
	{
	if (iBehaviour.iCount < 1)
		{
		iMode = EAct;
		}
	else
		{
		TSize size = iBehaviour.iWin->Size();
		size.iWidth += iBehaviour.iPos.iX;
		size.iHeight += iBehaviour.iPos.iY;
		iBehaviour.iWin->SetSize(size);
		--iBehaviour.iCount;
		iStuffDone = ETrue;
		}
	}

/**
 Sets the position for a future window-move command, the actual move will be 
 done in the next step 
 */
void CComparison::MoveWindow()
	{
	__UHEAP_MARK;
	if (iBehaviour.iWin)
		{
		TPoint pos = TPoint(TRnd::rnd(KPosLimit), TRnd::rnd(KPosLimit));
		pos.iX -= iBehaviour.iWin->Size().iWidth / 2;
		pos.iY -= iBehaviour.iWin->Size().iHeight / 2;
		iBehaviour.iPos = pos - iBehaviour.iWin->Pos();
		iBehaviour.iCount = KSteps;
		iBehaviour.iPos.iX /= iBehaviour.iCount;
		iBehaviour.iPos.iY /= iBehaviour.iCount;
		iMode = EMove;
		}
	__UHEAP_MARKEND;
	}

/**
 Sets the size for a future window-resize command, the actual resize will be 
 done in the next step 
 */
void CComparison::ResizeWindow()
	{
	__UHEAP_MARK;
	if (iBehaviour.iWin)
		{
		TPoint newsize = TPoint(TRnd::rnd(KPosLimit), TRnd::rnd(KPosLimit));
		TPoint oldsize;
		oldsize.iX = iBehaviour.iWin->Size().iWidth;
		oldsize.iY = iBehaviour.iWin->Size().iHeight;
		iBehaviour.iPos = newsize - oldsize;
		iBehaviour.iCount = KSteps;
		iBehaviour.iPos.iX /= iBehaviour.iCount;
		iBehaviour.iPos.iY /= iBehaviour.iCount;
		iMode = EResize;
		}
	__UHEAP_MARKEND;
	}

void CComparison::BringWindowToFrontL()
	{
	__UHEAP_MARK;
	CCompWin* win = iBehaviour.iWin;
	if (win)
		{
		win->BringToFrontL();
		TInt pos = FindTopWindow(win);
		if (pos >= 0)
			{
			iWindows.Remove(pos);
			iWindows.Append(win);
			}
		iStuffDone = ETrue;
		}
	__UHEAP_MARKEND;
	}
	
void CComparison::SendWindowToBackL()
	{
	__UHEAP_MARK;
	TInt ord = iBackground->OrdinalPosition();
	if (ord > 0)
		{
		CCompWin* win = iBehaviour.iWin;
		if (win)
			{
			TInt pos = FindTopWindow(win);
			if (pos >= 0)
				{
				iWindows.Remove(pos);
				iWindows.Insert(win, 0);
				win->Window()->SetOrdinalPosition(ord - 1);
				}
			else
				{
				win->SendToBackL();
				}
			iStuffDone = ETrue;
			}
		}
	__UHEAP_MARKEND;
	}

TInt CComparison::FindTopWindow(CCompWin* win)
	{
	return iWindows.Find(win);
	}
	
/**
 Returns a random window (which may be a NULL window)
*/
CCompWin * CComparison::RandomWindow()
	{
	if (iWindows.Count() == 0)
		{
		return 0;
		}
	
	TInt num = TRnd::rnd(iWindows.Count() + 1);
	
	if (num == iWindows.Count())
		{
		return 0;
		}
	else
		{
		return iWindows[num]->RandomWindow();
		}
	}

void CComparison::CreateWindowL()
	{
	CCompWin* parent = iBehaviour.iWin;
	CCompWin* win = CCompWin::NewLC(Session(), iWinGroup, parent, WindowGc());
	iBehaviour.iWin = win;
	if (!parent)
		{
		iWindows.AppendL(win);
		}
	CleanupStack::Pop(win);
	if (iNumWindowsLeft > 0)	//decrement window count
		{
		--iNumWindowsLeft;
		}
	iStuffDone = ETrue;
	}	

void CComparison::DestroyWindow()
	{
	CCompWin* win = iBehaviour.iWin;
	if (win)
		{
		TInt num = iWindows.Find(win);
		if (num != KErrNotFound)
			{
			iWindows.Remove(num);
			}
		delete win;
		iBehaviour.iWin = 0; // rby added
		iStuffDone = ETrue;
		}
	}	

void CComparison::TickWindowL()
	{
	CCompWin* win = iBehaviour.iWin;
	if (win)
		{
		if (win->TickL())
			{
			iStuffDone = ETrue;
			}
		}
	}
	
void CComparison::ToggleVisible()
	{
	__UHEAP_MARK;
	CCompWin* win = iBehaviour.iWin;
	if (win)
		{
		win->ToggleVisible();
		iStuffDone = ETrue;
		}
	__UHEAP_MARKEND;
	}

void CComparison::HandleRedraw(TWsRedrawEvent &aEvent)
	{
	__UHEAP_MARK;
	if (aEvent.Handle() != reinterpret_cast<TUint32>(iBackground))
		{
		reinterpret_cast<CCompWin*>(aEvent.Handle())->HandleRedraw(aEvent);
		}
	Session().Flush();
	__UHEAP_MARKEND;
	}

void CComparison::HandleEvent(TWsEvent &/*aEvent*/)
	{	
	}

/**
 Compares two bitmaps. Comparison is done on a pixel-by-pixel basis.
 If the bitmaps are of different sizes, the smaller axis of each is used for
 comparison (top-left subregions).
 First different pixel is stored in internally.
 @return ETrue if bitmaps match, EFalse otherwise.
*/
TBool CComparison::BitmapsMatch(const CFbsBitmap * aBitmap1, const CFbsBitmap * aBitmap2)
	{
	TDisplayMode mode1 = aBitmap1->DisplayMode();
	TDisplayMode mode2 = aBitmap2->DisplayMode();
	
	TSize bmpSize = aBitmap1->SizeInPixels();
	TSize bmpSize2 = aBitmap2->SizeInPixels();
	if (bmpSize2.iWidth < bmpSize.iWidth)
		{
		bmpSize.iWidth = bmpSize2.iWidth;
		}
	if (bmpSize2.iHeight < bmpSize.iHeight)
		{
		bmpSize.iHeight = bmpSize2.iHeight;
		}
	
	TRgb c1;
	TRgb c2;
	for (TInt y=0; y < bmpSize.iHeight; y++)
		{
		for (TInt x=0; x < bmpSize.iWidth; x++)
			{
			TPoint point(x, y);
			aBitmap1->GetPixel(c1, point);
			aBitmap2->GetPixel(c2, point);
			if (c1 != c2)
       			{
       			iPixel1 = c1.Value();
       			iPixel2 = c2.Value();
       			iPixelPos = point;
				return EFalse;
				}
			}		
		}
	return ETrue;		
	}
void CComparison::Verify(CFbsBitmap * aServerBmp)
	{	
	//Copy the currentBmp to lastBmp
	TInt dataStride = iBitmap[iCurrentBmp]->DataStride();
	TInt sizeInByte = dataStride * iBitmap[iCurrentBmp]->SizeInPixels().iHeight;
	TUint32* bitmapAddressSource = iBitmap[iCurrentBmp]->DataAddress();
	TUint32* bitmapAddressTarget = iBitmap[iLastBmp]->DataAddress();
	memcpy(bitmapAddressTarget, bitmapAddressSource, sizeInByte);
	// This method is only used by the animation and sprite windows; ignores 
	// comparisons for times that are too close to the sprite change. But as the 
	// difference error drifts, there is no point relying on it.
	if (!WindowsReadyForVerification())
		{
		Tick();
		return;
		}

	if (iWasOk)
		{
		DrawBitmap();
		
		const TBool bmpMatch = BitmapsMatch(aServerBmp, iBitmap[iCurrentBmp]);
		REPORT_EVENT( bmpMatch );
		
#ifdef __WINSCW__		
		if ( !bmpMatch )
			{
			__DEBUGGER();
			}
#endif
		
		if (!iData.saveOnlyDifferent || !bmpMatch)
			{
			TBuf<128> fileName;
			fileName.Append(iData.loggingPath);
			fileName.Append(_L("Stresslet_Comparison_"));
			if (iData.saveOnlyDifferent)
				{
				fileName.AppendNumFixedWidthUC((TUint)iErrorNum, EDecimal, 3);
				}
			else
				{
				fileName.AppendNumFixedWidthUC((TUint)iTestNum, EDecimal, 3);
				}
			TInt baseLength = fileName.Length();
			
			//previous is saved only when saving different bitmaps 
			//otherwise it is always the same as the previous expected
			if (iData.saveOnlyDifferent)
				{
				fileName.Append(_L("_Previous.mbm"));
				__ASSERT_ALWAYS(KErrNone == iBitmap[iLastBmp]->Save(fileName), Panic(EPanic6));
				}
			
			fileName.SetLength(baseLength);
			fileName.Append(_L("_Expected.mbm"));
			__ASSERT_ALWAYS(KErrNone == iBitmap[iCurrentBmp]->Save(fileName), Panic(EPanic7));
			
			fileName.SetLength(baseLength);
			fileName.Append(_L("_Screen.mbm"));
			__ASSERT_ALWAYS(KErrNone == aServerBmp->Save(fileName), Panic(EPanic8));

			//store difference between expected and screen bitmaps (XOR image)
			if (iData.saveDifferenceBitmap)
				{
				iBmpGc->Activate(iDifferenceDevice);
				iBmpGc->Reset();
				iBmpGc->SetDrawMode(CGraphicsContext::EDrawModePEN);
				iBmpGc->BitBlt(TPoint(0,0), iBitmap[iCurrentBmp]);
				iBmpGc->SetDrawMode(CGraphicsContext::EDrawModeXOR);
				iBmpGc->BitBlt(TPoint(0,0), aServerBmp);
				iBmpGc->Reset();
				fileName.SetLength(baseLength);
				fileName.Append(_L("_Difference.mbm"));
				__ASSERT_ALWAYS(KErrNone == iDifferenceBitmap->Save(fileName), Panic(EPanic19));
				}
			
			fileName.SetLength(baseLength);
			fileName.Append(_L("_info.txt"));
			RFile file;
			TInt err = file.Create(iFs, fileName, EFileWrite);
			if (err != KErrNone)
				err = file.Replace(iFs, fileName, EFileWrite);
			if (err == KErrNone)
				{
				TBuf8<128> info;
				info.Append(_L8("Seed = "));
				info.AppendNum(iFirstSeed);
				info.Append(_L8("\n"));
				file.Write(info);
				info.SetLength(0);
				TTime now;
				now.UniversalTime();
				info.Append(_L8(" Runtime = "));
				info.AppendNum(now.MicroSecondsFrom(iStartTime).Int64() / 1000);
				info.Append(_L8(" ms\r\n"));
				file.Write(info);
				info.SetLength(0);
				info.Append(_L8("Action = ["));
				info.AppendNum((TInt64)iAct);
				info.Append(_L8("]   Mode = ["));
				info.AppendNum((TInt64)iMode);
				info.Append(_L8("]   Test = ["));
				info.AppendNum((TInt64)iTestNum);
				info.Append(_L8("]\r\n"));
				file.Write(info);
				info.SetLength(0);
				info.Append(_L8("Pixel at ["));
				info.AppendNum((TInt64)iPixelPos.iX);
				info.Append(_L8(","));
				info.AppendNum((TInt64)iPixelPos.iY);
				info.Append(_L8("] mismatch screen 0x"));
				info.AppendNum((TInt64)iPixel1, EHex);
				info.Append(_L8(" != bitmap 0x"));
				info.AppendNum((TInt64)iPixel2, EHex);
				info.Append(_L8("\r\n\r\n"));
				file.Write(info);
				
				TPoint zero(0,0);
				for (TInt num = 0; num < iWindows.Count(); ++num)
					{
					iWindows[num]->Dump(file, zero, 0, iBehaviour.iWin);
					info.SetLength(0);
					info.Append(_L8("\r\n"));
					file.Write(info);
					}				
				file.Close();
				}
			else
				{
				//failed create logfile
				__ASSERT_ALWAYS(EFalse, Panic(EPanic9));
				}
			
			if (!bmpMatch)
				{
				++iErrorNum;
				}
			}
		}	
	if ((iTestNum > iData.maxRunCycles && iNumWindowsLeft == 0) || !iWasOk)
		{
		iMustConclude = ETrue;
		}
	iTimer->After(iData.period);
	}

/**
Write information(eg. seed, starttime, endtime and time elapse) to Stresslet_Log.txt file 
*/
void CComparison::WriteLog()
	{
	TBuf<128> fileName;
	fileName.Append(iData.loggingPath);
	fileName.Append(_L("Stresslet_Log.txt"));
	RFile file;
	TInt err = file.Create(iFs, fileName, EFileWrite);
	if (err != KErrNone)
		{
		err = file.Replace(iFs, fileName, EFileWrite);
		}
	if (err == KErrNone)
		{
		TBuf8<128> info;
		info.Append(_L8("Seed = "));
		info.AppendNum(iFirstSeed);
		info.Append(_L8("\r\n"));
		file.Write(info);
		info.SetLength(0);
		TBuf<40> dateTimeString;
	    _LIT(KFormat2,"%D%M%Y%/0%1%/1%2%/2%3%/3 %:0%H%:1%T%:2%S.%C%:3");
	    iStartTime.FormatL(dateTimeString,KFormat2);
		info.Append(_L8("StartTime = "));
		info.Append(dateTimeString);
		info.Append(_L8("\r\n"));
		TTime now;
		now.UniversalTime();
		now.FormatL(dateTimeString,KFormat2);
		info.Append(_L8("Endtime = "));
		info.Append(dateTimeString);
		info.Append(_L8("\r\n"));
		info.Append(_L8("Elapse = "));
		info.AppendNum(now.MicroSecondsFrom(iStartTime).Int64() / 1000);
		info.Append(_L8(" ms\r\n"));
		file.Write(info);
		file.Close();
		}
	}
/**
 Returns true if all visible windows are ready for verification
 */
TBool CComparison::WindowsReadyForVerification() const
	{
	TBool res = ETrue;
	TInt idx = 0;
	while ( idx < iWindows.Count() && res )
		{
		if ( iWindows[ idx ]->IsVisible() )
			{
			res = iWindows[ idx ]->QueryReadyForVerification();
			}
		idx++;
		}
	return res;
	}

/**
 Calls all windows to draw themselves on a bitmap
 */
void CComparison::DrawBitmap()
	{
	iBmpGc->Activate(iDevice[iCurrentBmp]);
	iBmpGc->Reset();

	// clear background area
	iBmpGc->SetPenStyle(CGraphicsContext::ENullPen);
	iBmpGc->SetBrushColor(KRgbBlack);
	iBmpGc->SetBrushStyle(CGraphicsContext::ESolidBrush);

	TPoint origin(0,0);
	TRect rect(origin, TSize(iData.windowWidth, iData.windowHeight));

	//create a region containing all screen, subtract all visible windows
	RRegion backClipping(rect);
	if (!iData.clearAllBackground)
		{
		for (TInt num = 0; num < iWindows.Count(); ++num)
			{
			if (iWindows[num]->IsVisible())
				iWindows[num]->SubSelfFromRegion(backClipping, rect, origin);
			}
		}
	
	//clip drawing to background only & clear
	iBmpGc->SetClippingRegion(backClipping);
	iBmpGc->DrawRect(rect);
	//go back to no clipping
	iBmpGc->CancelClipping();
	backClipping.Close();

	for (TInt num = 0; num < iWindows.Count(); ++num)
		{
		if (iWindows[num]->IsVisible())
			{
			iWindows[num]->ClearBitmapBackground(iBmpGc, rect, origin);
			iWindows[num]->DrawBitmap(iBmpGc, rect, origin);
			}
		}
	}
	
void CComparison::SetVerifyTick(TUint32 aTick)
	{
	for (TInt num = 0; num < iWindows.Count(); ++num)
		{
		if (iWindows[num]->IsVisible())
			iWindows[num]->SetVerifyTick(aTick);
		}
	}

CComparison::~CComparison()
	{
	iFs.Close();
	
	if (iBackground)
		{
		iBackground->Close();
		delete iBackground;
		}	
	
	iWindows.ResetAndDestroy();
	if (iWinGroup)
		{
		iWinGroup->Close();
		delete iWinGroup;
		}

	delete iOneShotCompare; //delayed compare

	delete iDifferenceDevice;
	delete iDifferenceBitmap;
	
	for (TInt bmp = 0; bmp < 2; ++bmp)
		{
		delete iDevice[bmp];
		delete iBitmap[bmp];
		}
	
	delete iBmpGc;
	delete iTimer;

	delete iScreenBitmap;
	delete iScreen;
	}
