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

#ifndef COMPARISON_H
#define COMPARISON_H



#include "stresslet.h"
#include "test_step_comparison.h"
#include "test_step_conf.h"

#include "utils.h"

class CCompWin;
class CCrpClient;

	
class CComparison : public CStresslet
	{
private:
	class COperationTimer : public CTimer
	{
	public:
		COperationTimer(CComparison* aComp);
		void ConstructL();
		//pure virtual functions from CActive
		void DoCancel();
		void RunL();
	private:
		CComparison* iComp;
	};
	
	//delayed bitmap comparison
	class COneShotCompare : public CAsyncOneShot
	{
	public:
		COneShotCompare(TInt aPriority, CComparison& aComparison);
		static COneShotCompare* NewL(TInt aPriority, CComparison& aComparison);
		virtual void RunL();
	private:
		CComparison& iComparison;
	};
	
	friend class COneShotCompare;
	
	enum TMode
		{
		EAct,
		EMove,
		EResize
		};
	enum TAct
		{
		EACreate,
		EADestroy,
		EAFront,
		EABack,
		EAMove,
		EAResize,
		EATick,
		EAToggleVisible,
		EACount
		};
	struct TBehaviour
		{
		CCompWin* iWin;
		TPoint iPos;
		TInt iCount;
		};
public:
	static CComparison * NewLC(MTestStepReporter& aReporter);
	virtual ~CComparison();

	void Verify(CFbsBitmap * aServerBmp);
	void SetRunDataL(const TRunData& aRunData, MTestStepConfigurationContextFactory* aConfFactory);
	TBool WindowsReadyForVerification() const;
	void SetVerifyTick(TUint32 aTick);
	
private:
	void HandleRedraw(TWsRedrawEvent &aEvent);
	void HandleEvent(TWsEvent &aEvent);
	
	virtual void StartL();
	TInt Tick();

	void TickL();
	void ActL();
	void MoveL();
	void ResizeL();
	
	void DoStuffL();
	void CreateWindowL();
	void DestroyWindow();
	void MoveWindow();
	void ResizeWindow();
	void BringWindowToFrontL();
	void SendWindowToBackL();
	void TickWindowL();
	void ToggleVisible();

	TInt FindTopWindow(CCompWin* aWin);
	void DrawBitmap();
	CCompWin* RandomWindow();
	TBool BitmapsMatch(const CFbsBitmap * aBitmap1, const CFbsBitmap * aBitmap2);
	void Touch();
	
	CComparison (MTestStepReporter& aReporter);
	
	void ConstructL();
	void WriteLog();
private:
	TInt64 iFirstSeed;
	TMode iMode;
	TAct iAct;
	TBool iWasOk;
	TBehaviour iBehaviour;
	CPeriodic* iPeriodic;
	RWindowGroup* iWinGroup;
	RBlankWindow* iBackground;
	RPointerArray<CCompWin> iWindows;
	CFbsBitmap * iBitmap[2];
	CFbsBitmapDevice * iDevice[2];
	CFbsBitGc * iBmpGc;
	CFbsBitmap * iDifferenceBitmap;
	CFbsBitmapDevice * iDifferenceDevice;
	CWsScreenDevice* iScreen;
	CFbsBitmap *iScreenBitmap;
	RFs iFs;
	TTime iStartTime;
	TInt iCurrentBmp;
	TInt iLastBmp;
	TInt iTestNum;
	TInt iErrorNum;
	TUint32 iPixel1;
	TUint32 iPixel2;
	TPoint iPixelPos;
	
	TBool iMustConclude;
	TInt  iNumWindowsLeft;	//number of windows to be created before test is over
	CTestExecWatchCat* iWatchCat;
	TRunData iData;

	TBool iStuffDone;
	COperationTimer* iTimer;
	COneShotCompare *iOneShotCompare;
	};

#endif // COMPARISON_H
