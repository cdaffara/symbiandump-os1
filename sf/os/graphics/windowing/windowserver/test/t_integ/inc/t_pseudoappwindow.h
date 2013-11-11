// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __T_PSEUDOAPPWINDOW_H__
#define __T_PSEUDOAPPWINDOW_H__

#include <w32std.h>		//RWsSession, CWsScreenDevice etc

#include "t_pseudoapptestresults.h"

enum TWindowChange
	{
	ENoChange = 0,
	EIncremental,
	ECyclic
	};

enum TWindowChangeType
	{
	ENoChanges,
	EWindowSize,
	EWindowPosition,
	EWindowPositionAndSize,
	EWindowPositionAndSizeWithOpacity
	};

typedef enum TPixelFormat
	{
	EFormatRgb,
	EFormatYuv
	}TPixelFormat;

typedef enum TParamChange
	{
	ENoParamChange,
	EFirstFrame,
	EBitmapScale,
	ESizeChange,
	EPositionChange,
	EExtentChange,
	EOpacityChange
	} TParamChange;

class TDrawParameters
	{
public:
	TDrawParameters();
	~TDrawParameters();
	TBool operator==(const TDrawParameters& x) const;

public:
	TBool iRedraw;
	TInt iBitmapScale;
	TInt iBitmapAlpha;
	TSize iWindowSize;
	TPoint iWindowPos;
	};

class CTestWindow : public CBase
	{
public:
	inline RWindow* Window() {return &iWindow;};
	IMPORT_C virtual void Rotate(TSize aScreenSize);
	IMPORT_C virtual void DrawL();
	~CTestWindow();

	void SetPosition();
	void SetSize();
	void SetExtent();

protected:
	IMPORT_C CTestWindow(TInt aScreenNo, TDisplayMode aMode);

private:
	void ConstructL(TInt aScreenNo, TDisplayMode aMode);

protected:
	RWsSession        iSession;
	CWsScreenDevice*  iScreen;
	CWindowGc*        iWindowGc;
	RWindow           iWindow;
	RWindowGroup      iGroup;
	TDisplayMode      iMode;
	CArrayFixFlat<TDrawParameters>* iLoadsaParameters;
	TGceTestResults* iGceTestResults;
	TInt iFrameCounter;
	};

class CTestEmptyWindow : public CTestWindow
	{
public:
	IMPORT_C static CTestEmptyWindow* NewL(TInt aScreen, TDisplayMode aMode, const TSize& aScreenSize);
	IMPORT_C virtual void Rotate(TSize aScreenSize);
	IMPORT_C virtual void DrawL();
	~CTestEmptyWindow();

protected:
	IMPORT_C CTestEmptyWindow(TInt aScreenNo, TDisplayMode aMode);

private:
	void ConstructL(const TSize& aScreenSize);

private:
	TInt iWindowDrawn;
	};

class CTestEcomWindow : public CTestWindow
	{
public:
	IMPORT_C static CTestEcomWindow* NewL(TInt aScreenNo, TDisplayMode aMode, const TPtrC& aMultiBitmapEcomFile,
	                                      TInt aMultiBitmapEcomFileSize, TGceTestResults* aGceTestResults,
	                                      CArrayFixFlat<TDrawParameters>* aLoadsaParameters);
	~CTestEcomWindow();
	IMPORT_C virtual void Rotate(TSize aScreenSize);
	IMPORT_C virtual void DrawL();


protected:
	IMPORT_C CTestEcomWindow(TInt aScreenNo, TDisplayMode aMode);

private:
	void ConstructL(const TPtrC& aMultiBitmapEcomFile, TInt aMultiBitmapEcomFileSize, TGceTestResults* aGceTestResults,
	                CArrayFixFlat<TDrawParameters>* aLoadsaParameters);
	TParamChange Redraw();

private:
	TPtrC iMultiBitmapEcomFile;
	TInt iMultiBitmapEcomFileSize;
	TInt iBitmapCounter;
	};

class CTestUiWindow : public CTestWindow
	{
public:
	IMPORT_C static CTestUiWindow* NewL(TInt aScreenNo, TDisplayMode aMode, const TPtrC& aMultiBitmapUiFile, TInt aMultiBitmapUiFileSize,
	                             CArrayFixFlat<TRect>* aRectArray, TInt aFrameDuration, TInt aMultiBitmapUiFileTransparency,
	                             TGceTestResults* aGceTestResults,
	                             CArrayFixFlat<TDrawParameters>* aLoadsaParameters);

	IMPORT_C virtual void Rotate(const TSize& aScreenSize);
	IMPORT_C virtual void DrawL();
	~CTestUiWindow();

protected:
	IMPORT_C CTestUiWindow(TInt aScreenNo, TDisplayMode aMode);

private:
	void ConstructL(const TPtrC& aMultiBitmapUiFile, TInt aMultiBitmapUiFileSize, CArrayFixFlat<TRect>* aRectArray,
	                TInt aFrameDuration, TInt aMultiBitmapUiFileTransparency, TGceTestResults* aGceTestResults,
	                CArrayFixFlat<TDrawParameters>* aLoadsaParameters);
	TParamChange Redraw();
	void CalcNewWindowExtent(TSize aScreenSize, TInt aIncrement, TInt aDirection);
	void SetBitmapTransparency(CFbsBitmap* aBitmap, TUint8 aAlphaValue, TInt aAlphaByteLocation);

	void ParamChangeCyclicPositionAndSize(TDrawParameters& aDrawParams, TInt aFrameCounter);
	void ParamChangeCyclicPosition(TDrawParameters& aDrawParams, TInt aFrameCounter);
	void CalcNewWindowLocation(TSize aScreensize, TInt aIncrement, TInt aDirection,TDrawParameters& aDrawParams);
	void SetBitmapAlpha(CFbsBitmap* aBitmap);

protected:
	TBool iSimUiDrawn;
	TPtrC iMultiBitmapUiFile;
	TInt iMultiBitmapUiFileSize;
	CArrayFixFlat<TRect>* iRectArray;
	TInt iMultiBitmapUiFileTransparency;
	TInt iFrameDuration;
	};

#endif //__T_PSEUDOAPPWINDOW_H__
