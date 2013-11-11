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



/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#ifndef __TSURFACEUPDATEINTEG_H__
#define __TSURFACEUPDATEINTEG_H__

#include <test/testexecutestepbase.h>
#include <test/tgraphicsharness.h>
#include "surfaceupdateserver.h"
#include "surfaceupdateclient.h"
#include <graphics/surface.h>


class CTWindowGroup;
class CTWindowTreeNode;
class CTRedrawHandler;
class CSurfaceHelper;
class CTWindow;

/**
 * Main class for installing a test environment and running test cases. 
 */
class CTSurfaceUpdateInteg : public CTGraphicsBase
	{
public:
	CTSurfaceUpdateInteg(CTestStep* aStep);
	~CTSurfaceUpdateInteg();
	void ConstructL();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void TestCase1L();
	void TestCase2L();
	void TestCase3();
	
	CTWindowGroup* CreateGroupL(CWsScreenDevice* aScreenDevice);
	void RegisterWindowL(CTWindowTreeNode* aWindow);
	TBool CompareAllScreens();
	TBool Compare(TInt aScreenNumber, const TRect& aRectSrc, CFbsBitmap* aOriginalBitmap);
	void FillRefBitmap(TRgb aColor);
private:
	TInt iNumOfScreens;
	RWsSession iWs;
	RSurfaceUpdateSession iSurfaceUpdate; 
	
	RPointerArray <CWsScreenDevice> iWsDevs;
	RPointerArray <CWindowGc> iGcs;

	RPointerArray <CTWindowGroup> iGroups;
	RPointerArray <CTWindowTreeNode> iWindows;
	CTRedrawHandler* iRedrawHandler;
	CSurfaceHelper* iSurfaceHelper;
	TSurfaceId iSurface; //doesn't own
	TSurfaceId iSurfaceAbove; //doesn't own
	TSurfaceId iSurfaceAbove1; //bigger size, doesn't own
	TSurfaceId iSurfaceDoubleBuffered; //doesn't own
	RPointerArray<CFbsBitmap> iBitmaps; //the list of bitmaps where each bitmap corresponds to particular screen
	CTWindow *iWindowAbove1; //on first screen, doesn't own
	CTWindow *iWindowAbove11; //on first screen, bigger size, doesn't own
	CTWindow *iWindowAbove2; //on second screen, doesn't own
	CTWindow *iWindowSingleBuffured1; //on first screen, doesn't own
	CTWindow *iWindowSingleBuffured2; //on second screen, doesn't own
	CTWindow *iWindowDoubleBuffured1; //on first screen, doesn't own
	CTWindow *iWindowDoubleBuffured2; //on second screen, doesn't own
	
	
	CFbsBitmap* iRefBitmap; //used for comparison
	CFbsBitmapDevice* iBitmapDevice;
	CGraphicsContext* iGraphicsContext;
	};

class CTSurfaceUpdateIntegStep : public CTGraphicsStep
	{
public:
	CTSurfaceUpdateIntegStep();
	inline TBool IsScreenCaptureSupported() const;

private:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
private:
	TBool iScreenCapture; //signifies if screen capture is supported or not 
	};

inline TBool CTSurfaceUpdateIntegStep::IsScreenCaptureSupported() const
	{
	return iScreenCapture;
	}


struct TSwitchDisplayAndReleaseBuffer
	{
	TInt iBuffer; //buffer for submission
	TInt iScreen; //screen to be connected/disconnected
	TSurfaceId iSurfaceId;
	};

/**
 * The class executes specified commands in a separate thread.
 * Will be used when a main thread is blocked and waits for notifications to arrive. 
 */
class CCommandDispatcher : public CBase
	{
public:	
enum CommandName
	{
	EDisconnectDisplayAndReleaseBuffer,
	EConnectDisplayAndReleaseBuffer
	};
	CCommandDispatcher(CommandName aCommandName, TAny* aPram) :
		iCommandName(aCommandName), iParam(aPram){}
	TInt Start();
private:
	static TInt ThreadFunction(TAny*);
	void ExecuteCommandL();
private:
	CommandName iCommandName;
	TAny* iParam;
	};

_LIT(KTSurfaceUpdateIntegStep,"TSurfaceUpdateInteg");


#endif	// __TSURFACEUPDATEINTEG_H__
