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
// Test various cases of Wserv panicing client apps
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "TPANIC.H"
#include "../tlib/testbase.h"
#include <w32debug.h>
#define TEST_BITMAP _L("Z:\\WSTEST\\WSAUTOTEST.MBM")

class RWsSessionHacker : public RWsSession
	{
public:
	inline RWsBuffer *WsBuffer() const {return(iBuffer);};
	inline TInt PanicItSendReceive(TInt aFunction,const TIpcArgs& aArgs) const {return SendReceive(aFunction,aArgs);};
	inline TInt PanicItSend(TInt aFunction,const TIpcArgs& aArgs) const {return Send(aFunction,aArgs);};
	inline TInt PanicItSendReceive(TInt aFunction) const {return SendReceive(aFunction);};
	inline TInt PanicItSend(TInt aFunction) const {return Send(aFunction);};
	};

class RWsBufferHacker // copy of original data structure to access buffer data
	{
public:
	RWsSession* iSession;
	CWsGraphic::CManager* iManager;
	TBool iAutoFlush;
	TPtr8 iBuf;
	RWsBuffer* iNext;
	TInt iPreviousHandle;
	TInt iBufSize;
	TInt iMaxBufSize;
	TInt iDirectAcessCount;
	RArray<TInt> iBitmapArray;
	TBool iInvalidBitmapArray;
	};

CTPanic::CTPanic(CTestStep* aStep):
	CTWsGraphicsBase(aStep)
	{
	}
	
CTPanic::~CTPanic()
	{
	}

LOCAL_C TInt DoDeletedParentTest(TInt aInt, TAny *aScreenNumber)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	// point to correct screen
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(screen->Construct((TInt)aScreenNumber));
	
	RWindowGroup group(ws);
	User::LeaveIfError(group.Construct(888));
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
	switch(aInt)
		{
		case 1:
			RWindow win1(ws);
			User::LeaveIfError(win1.Construct(group,1));
			RWindow win2(ws);
			User::LeaveIfError(win2.Construct(win1,2));
			win1.Close();
			win2.SetExtent(TPoint(1,2),TSize(3,4));
			break;
		}
	ws.Flush();
	return(EWsExitReasonBad);
	}

LOCAL_C void ReuseWindow(RWsSession& aWs,RWindowGroup& aGroup,RWindow aCopyWin,RWindow* aPtrWin)
	{
	aPtrWin->Close();
	RWindow win(aWs);
	User::LeaveIfError(win.Construct(aGroup,17));
	aCopyWin.SetExtent(TPoint(1,2),TSize(3,4));	
	}

LOCAL_C void ReuseGroupWindow(RWsSession& aWs,RWindowGroup aCopyWin,RWindowGroup* aPtrWin)
	{
	aPtrWin->Close();
	RWindowGroup group(aWs);
	User::LeaveIfError(group.Construct(889));
	group.EnableReceiptOfFocus(EFalse);	
	aCopyWin.EnableReceiptOfFocus(EFalse);	
	}

LOCAL_C void ReuseSprite(RWsSession& aWs,RWindow& aWin,RWsSprite aCopySprite,RWsSprite* aPtrSprite)
	{
	aPtrSprite->Close();
	RWsSprite sprite(aWs);
	sprite.Construct(aWin,TPoint(0,0),0);
	aCopySprite.SetPosition(TPoint(22,22));	
	}

LOCAL_C void ReusePointerCursor(RWsSession& aWs,RWsPointerCursor aCopyCursor,RWsPointerCursor* aPtrCursor)
	{
	aPtrCursor->Close();
	RWsPointerCursor cursor(aWs);
	cursor.Construct(0);
	aCopyCursor.Activate();	
	}

LOCAL_C TInt DoHandleReUse(TInt aInt, TAny *aScreenNumber)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	CWsScreenDevice *scrdev=new(ELeave) CWsScreenDevice(ws);
	scrdev->Construct((TInt)aScreenNumber);
	RWindowGroup group(ws);
	User::LeaveIfError(group.Construct(888));
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
	RWindow win(ws);
	User::LeaveIfError(win.Construct(group,1));
	switch(aInt)
		{
		case 2:			//WS_HANDLE_WINDOW
			ReuseWindow(ws,group,win,&win);
			break;
		case 3:			//WS_HANDLE_GROUP_WINDOW
			ReuseGroupWindow(ws,group,&group);
			break;
		case 4:			//WS_HANDLE_SPRITE
			{
			RWsSprite sprite(ws);
			sprite.Construct(win,TPoint(0,0),0);
			ReuseSprite(ws,win,sprite,&sprite);
			}
			break;
		case 5:			//WS_HANDLE_POINTER_CURSOR
			{
			RWsPointerCursor cursor(ws);
			cursor.Construct(0);
			ReusePointerCursor(ws,cursor,&cursor);
			}
			break;
		}
	ws.Flush();
	return(EWsExitReasonBad);
	}

LOCAL_C TInt DoScreenDevicePanicTest(TInt aInt, TAny *aScreenNumber)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());

	CWsScreenDevice *scrdev=new(ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(scrdev->Construct((TInt)aScreenNumber));
	
	RWindowGroup group(ws);
	User::LeaveIfError(group.Construct(888));
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
	RWindow win(ws);
	User::LeaveIfError(win.Construct(group, 1));
	win.Activate();
	CFbsBitmap *bitmap=new(ELeave) CFbsBitmap;
 	switch(aInt)
		{
		case 1:
			scrdev->CopyScreenToBitmap(bitmap);
			break;
		case 2:
			scrdev->CopyScreenToBitmap(bitmap,TRect(0,0,10,10));
			break;
		default:
			return(EWsExitReasonFinished);
		}
	ws.Flush();
	return(EWsExitReasonBad);
	}

LOCAL_C TInt DoOpcodeTests(TInt aInt, TAny *aScreenNumber)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());

	CWsScreenDevice *scrdev=new(ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(scrdev->Construct((TInt)aScreenNumber));
	
	RWindowGroup group(ws);
	User::LeaveIfError(group.Construct(888));
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
	RWindow win(ws);
	User::LeaveIfError(win.Construct(group, 1));
	win.Activate();
	CWindowGc *gc;
	scrdev->CreateContext(gc);
 	switch(aInt)
		{
		case 1:
			ws.TestWrite(ws.WsHandle(),9999,NULL,0);
			break;
		case 2:
			gc->Activate(win);
			win.BeginRedraw();
			ws.TestWrite(gc->WsHandle(),9999,NULL,0);
			/* This only panics if the command is processed immediately.  If it goes into the redraw
			store then it will be unable to panic the client untill an additional buffer has been received,
			hence the double flush.
			*/
			win.EndRedraw();
			ws.Finish();
			win.BeginRedraw();
			win.EndRedraw();
			break;
		case 3:
			ws.TestWrite(scrdev->WsHandle(),9999,NULL,0);
			break;
		case 4:
			{
			CWsBitmap *bitmap=new(ELeave) CWsBitmap(ws);
			bitmap->Create(TSize(10,10),EGray4);
			ws.TestWrite(bitmap->WsHandle(),9999,NULL,0);
			}
			break;
		case 5:
			ws.TestWrite(win.WsHandle(),9999,NULL,0);
			break;
		case 6:
			ws.TestWrite(group.WsHandle(),9999,NULL,0);
			break;
		case 7:
			{
			RWsSprite sprite(ws);
			sprite.Construct(win,TPoint(0,0),0);
			ws.TestWrite(sprite.WsHandle(),9999,NULL,0);
			}
			break;
		default:
			return(EWsExitReasonFinished);
		}
	ws.Flush();
	return(EWsExitReasonBad);
	}

TInt DoGraphicsPanicTest(RWsSession& aWs, RWindow& aRWin, RDrawableWindow* aDrawWin, CWindowGc* aGc, CFbsFont* aFont, TInt aTest, TInt aSubTest, TBool aInRedraw, TBool aNeedsValidating)
	{
	if (aInRedraw || aNeedsValidating)
		{
		aRWin.BeginRedraw();
		if (!aInRedraw)
			{ // TransWin without redraw active needs the begin/end to make the redraw store active
			aRWin.EndRedraw(); // or else all graphics will simply be ignored and no panics will occur
			}
		}
	aWs.Flush();
	TRect rect01(0,0,1,1);
	TPoint point00;
	switch(aTest)
		{
	case 1:
		aGc->UseFont(aFont);
		switch(aSubTest)
			{
		case 0:
			{
			TWsGcCmdBoxText boxText(rect01,0,CGraphicsContext::ELeft,0,0x800000,1);
			aWs.TestWrite(aGc->WsHandle(),EWsGcOpDrawBoxText,&boxText,sizeof(boxText));
			}
			break;
		case 1:
			{
			TWsGcCmdDrawText dt(point00,600);
			aWs.TestWrite(aGc->WsHandle(),EWsGcOpDrawText,&dt,sizeof(dt));
			}
			break;
		case 2:
			{
			TWsGcCmdBoxTextOptimised1 dt(rect01,0,600);
			aWs.TestWrite(aGc->WsHandle(),EWsGcOpDrawBoxTextOptimised1,&dt,sizeof(dt));
			}
			break;
		case 3:
			{
			TWsGcCmdDrawTextVertical dt(point00,600,EFalse);
			aWs.TestWrite(aGc->WsHandle(),EWsGcOpDrawTextVertical,&dt,sizeof(dt));
			}
			break;
		case 4:
			{
			TWsGcCmdBoxTextVertical dt(rect01);
			dt.length=600;
			aWs.TestWrite(aGc->WsHandle(),EWsGcOpDrawBoxTextVertical,&dt,sizeof(dt));
			}
			break;
		case 5:
			return(EWsExitReasonFinished);
			}
		break;
	case 2:
		{
		TInt opcode=0;
		switch(aSubTest)
			{
		case 0:
			opcode=EWsGcOpGdiBlt2;
			break;
		case 1:
			opcode=EWsGcOpGdiWsBlt2;
			break;
		case 2:
			return(EWsExitReasonFinished);
			}
		TWsGcCmdGdiBlt2 gdiBlit(point00,0xBADBAD);
		aWs.TestWrite(aGc->WsHandle(),opcode,&gdiBlit,sizeof(gdiBlit));
		if (aInRedraw)
			{ // Adding two bad bitmaps to redraw store fbs store causes leave as NULL handles of failed bitmaps clash
			gdiBlit.handle=0xBADBAD2;
			aWs.TestWrite(aGc->WsHandle(),opcode,&gdiBlit,sizeof(gdiBlit));
			}
		}
		break;
	case 3:
		{
		const TInt KNumBadBmpModes=3;
		const TInt KNumTestsPerOpcode=KNumBadBmpModes*2;
		enum {KPanicIndexMasked,KPanicIndexDraw,KPanicIndexAlphaBlend,KPanicIndexMax};
		TInt opcodeMode=aSubTest/KNumTestsPerOpcode;
		TInt bmpMode=aSubTest%KNumTestsPerOpcode;
		TInt bmp1=0xBADBAD;
		TInt bmp2=0xBADBAD;
		TInt goodBmp;
		TInt opcodeBlt;
		TInt opcodeDraw;
		if (bmpMode<KNumBadBmpModes)
			{	// These two use a CFbsBitmap
			CFbsBitmap* goodBitmap=new(ELeave) CFbsBitmap;
			goodBitmap->Create(TSize(10,10),EGray4);
			goodBmp=goodBitmap->Handle();
			opcodeBlt=EWsGcOpGdiBltMasked;
			opcodeDraw=EWsGcOpDrawBitmapMasked;
			}
		else
			{	// These two use a CWsBitmap
			CWsBitmap* goodBitmap=new(ELeave) CWsBitmap(aWs);
			goodBitmap->Create(TSize(10,10),EGray4);
			goodBmp=goodBitmap->WsHandle();
			opcodeBlt=EWsGcOpGdiWsBltMasked;
			opcodeDraw=EWsGcOpWsDrawBitmapMasked;
			}
		switch(bmpMode%KNumBadBmpModes)
			{
		case 0:
			bmp2=goodBmp;
			break;
		case 1:
			bmp1=goodBmp;
			break;
		case 2:	// Leave them both bad
			break;
			}
		switch(opcodeMode)
			{
		case KPanicIndexMasked:
			{
			TWsGcCmdBltMasked gdiBlitMasked(point00,bmp1,rect01,bmp2,EFalse);
			aWs.TestWrite(aGc->WsHandle(),opcodeBlt,&gdiBlitMasked,sizeof(gdiBlitMasked));
			}
			break;
		case KPanicIndexDraw:
			{
			TWsGcCmdDrawBitmapMasked maskedBitmap(rect01,bmp1,rect01,bmp2,EFalse);
			aWs.TestWrite(aGc->WsHandle(),opcodeDraw,&maskedBitmap,sizeof(maskedBitmap));
			}
			break;
		case KPanicIndexAlphaBlend:
			{
			TWsGcCmdAlphaBlendBitmaps alphaBlend(point00,bmp1,rect01,bmp2,point00);
			aWs.TestWrite(aGc->WsHandle(),EWsGcOpGdiAlphaBlendBitmaps,&alphaBlend,sizeof(alphaBlend));
			}
			break;
		case KPanicIndexMax:
			return(EWsExitReasonFinished);
			}
		}
		break;
	case 4:
		switch(aSubTest)
			{
		case 0:
			{
			TWsClCmdCreateBitmap createBitmap;
			createBitmap.handle=0xB0D;
			aWs.TestWrite(aWs.WsHandle(),EWsClOpCreateBitmap,&createBitmap,sizeof(createBitmap));
			}
			break;
		case 1:
			{
			TInt badBrush=0xBADB3054;
			aWs.TestWrite(aGc->WsHandle(),EWsGcOpUseBrushPattern,&badBrush,sizeof(badBrush));
			}
			break;
		case 2:
			{
			TWsGcCmdDrawBitmap drawBitmap(point00,0xBADBAD);
			aWs.TestWrite(aGc->WsHandle(),EWsGcOpDrawBitmap,&drawBitmap,sizeof(drawBitmap));
			}
			break;
		case 3:
			return(EWsExitReasonFinished);
			}
		break;
	case 5:
		// test bad opcodes
		{
		TInt opcode=0;
		switch(aSubTest)
			{
		case 0:
			opcode=9999;
			break;
		case 1:
			return(EWsExitReasonFinished);
			}
		aWs.TestWrite(aGc->WsHandle(),opcode,NULL,0);
		}
		break;
	case 6:
		{// Test EWsGcOpDrawPolygon with invalid parameters
		 // First two times has slightly more points specified than exist in the data
		 // Third time time has a massive number of points in the header
		const TInt KNumTestsPerPolyMode=3;
		enum TPanicPolyMode {EPanicPolyModePolygon,EPanicPolyModePolyLine,EPanicPolyModeEnd};
		TInt polyMode=aSubTest/KNumTestsPerPolyMode;
		if (polyMode==EPanicPolyModeEnd)
			return(EWsExitReasonFinished);
		TInt subMode=aSubTest%KNumTestsPerPolyMode;
		TInt bufPoints=0;
		TInt headerPoints=1;
		switch(subMode)
			{
		case 0:
			break;
		case 1:
			bufPoints=2;
			headerPoints=8;
			break;
		case 2:
			bufPoints=2;
			headerPoints=999999;
			break;
			}
		TInt bufDataLen=bufPoints*sizeof(TPoint);
		if (polyMode==EPanicPolyModePolyLine)
			bufDataLen+=sizeof(TWsGcCmdDrawPolyLine);
		else
			bufDataLen+=sizeof(TWsGcCmdDrawPolygon);
		TAny* bufData=User::AllocL(bufDataLen);
		TPoint* pointPtr;
		TInt opcode;
		if (polyMode==EPanicPolyModePolyLine)
			{
			TWsGcCmdDrawPolyLine* drawPolyline=static_cast<TWsGcCmdDrawPolyLine*>(bufData);
			drawPolyline->numPoints=headerPoints;
			drawPolyline->more=EFalse;
			drawPolyline->last=point00;
			pointPtr=reinterpret_cast<TPoint*>(drawPolyline+1);
			opcode=EWsGcOpDrawPolyLine;
			}
		else
			{
			TWsGcCmdDrawPolygon* drawPolygon=static_cast<TWsGcCmdDrawPolygon*>(bufData);
			drawPolygon->numPoints=headerPoints;
			drawPolygon->fillRule=CGraphicsContext::EAlternate;
			pointPtr=reinterpret_cast<TPoint*>(drawPolygon+1);
			opcode=EWsGcOpDrawPolygon;
			}
		const TPoint* endPtr=pointPtr+bufPoints;
		TInt pointPos=0;
		while(pointPtr<endPtr)
			*pointPtr++=TPoint(pointPos,pointPos);
		aWs.TestWrite(aGc->WsHandle(),opcode,bufData,bufDataLen);
		aWs.Flush();	// Needs flush to make sure EndRedraw() doesn't make buffer bigger and catch out buf len check
		}
		break;
	case 7:
		{
		// first sets the index to match the total count
		// second sets the index negative
		// fourth sends too much data
		TWsGcCmdStartSegmentedDrawPolygon startPoly;
		startPoly.totalNumPoints=8;
		aWs.TestWrite(aGc->WsHandle(),EWsGcOpStartSegmentedDrawPolygon,&startPoly,sizeof(startPoly));
		TInt bufDataLen=sizeof(TWsGcCmdSegmentedDrawPolygonData)+startPoly.totalNumPoints*sizeof(TPoint);
		TAny* bufData=User::AllocL(bufDataLen);
		TWsGcCmdSegmentedDrawPolygonData* polyData=static_cast<TWsGcCmdSegmentedDrawPolygonData*>(bufData);
		polyData->numPoints=1;
		polyData->index=0;
		switch(aSubTest)
			{
		case 0:
			polyData->index=startPoly.totalNumPoints;
			break;
		case 1:
			polyData->index=-123;
			break;
		case 2:
			polyData->numPoints=startPoly.totalNumPoints+1;
			break;
		case 3:
			return(EWsExitReasonFinished);
			}
		aWs.TestWrite(aGc->WsHandle(),EWsGcOpSegmentedDrawPolygonData,polyData,bufDataLen);
		TWsGcCmdDrawSegmentedPolygon drawit;
		drawit.fillRule=CGraphicsContext::EAlternate;
		aWs.TestWrite(aGc->WsHandle(),EWsGcOpDrawSegmentedPolygon,&drawit,sizeof(drawit));
		}
		break;
	case 8:
		{
		if (aSubTest==1)
			return(EWsExitReasonFinished);
		// This is a test designed to specificially test polylines still work after the previous
		// polyline/polygon tests. One potential defect is they leave the common redraw store gc
		// in a bad state still holding part of the poly data and causing a EWservPanicBadPolyData
		// panic.
		// This test is designed to make sure the drawpolyline call works ok and we reach the bad
		// opcode panic instead.
		TWsGcCmdStartSegmentedDrawPolygon startPoly;
		startPoly.totalNumPoints=2;
		aWs.TestWrite(aGc->WsHandle(),EWsGcOpStartSegmentedDrawPolygon,&startPoly,sizeof(startPoly));
		struct
			{
			TWsGcCmdSegmentedDrawPolygonData iPolyData;
			TPoint iPoints[2];
			} polyParams;
		polyParams.iPoints[0].iX=1;
		polyParams.iPoints[0].iY=1;
		polyParams.iPoints[1].iX=2;
		polyParams.iPoints[1].iY=2;
			
		polyParams.iPolyData.numPoints=2;
		polyParams.iPolyData.index=0;
		aWs.TestWrite(aGc->WsHandle(),EWsGcOpSegmentedDrawPolygonData,&polyParams.iPolyData,sizeof(polyParams));
		TWsGcCmdDrawSegmentedPolygon drawit;
		drawit.fillRule=CGraphicsContext::EAlternate;
		aWs.TestWrite(aGc->WsHandle(),EWsGcOpDrawSegmentedPolygon,&drawit,sizeof(drawit));
		aWs.TestWrite(aGc->WsHandle(),9999,NULL,0);
		}
		break;
	case 9:
		if (aSubTest==1)
			return(EWsExitReasonFinished);
		aGc->Activate(*aDrawWin);	// Double activate
		break;
	case 10:
		if (aSubTest==1)
			return(EWsExitReasonFinished);
		aGc->DrawText(_L("No font"),point00);
		break;
	case 11:
		if (aSubTest==1)
			return(EWsExitReasonFinished);
		aGc->SetBrushStyle(CGraphicsContext::EPatternedBrush);
		aGc->DrawRect(rect01);
		break;
	case 12:
		{
		if (aSubTest==1)
			return(EWsExitReasonFinished);
		aGc->UseFont(aFont);
		TPtrC bigAndbad(NULL,5000);	// Will go through remote descriptor fetching code
		aGc->DrawText(bigAndbad,point00);
		}
		break;
	case 13:
		{
		if (aSubTest==1)
			return(EWsExitReasonFinished);
		TInt badHandle=0xDEADBAD;
		aWs.TestWrite(aGc->WsHandle(),EWsGcOpUseFont,&badHandle,sizeof(badHandle));
		aGc->DrawText(_L("BOO!"),point00);
		}
		break;
		}
	if (aInRedraw)
		aRWin.EndRedraw();
	aWs.Finish();
	return(EWsExitReasonBad);
	}

LOCAL_C TInt GraphicsPanicTest(TInt aInt, TAny* aPanicParams)
	{
	CTPanic::TPanicParams* panicParams=static_cast<CTPanic::TPanicParams*>(aPanicParams);
	/*
	 * Drawing to a transparent window goes via the redraw store. In this
	 * situation parameters do not get checked during the original processing
	 * of the incoming graphics commands. They are only caught later when 
	 * playing back from the redraw store.
	 */
	const TBool useTransWin = panicParams->iRedrawMode==EPanicRedrawModeTransRedraw;
	/*
	 * We always do redraw drawing unless we are using a BackedUpWindow.
	 * Redraws can affect the way graphics commands are pre-processed, 
	 * as with transparent windows they can also cause commands to get
	 * buffered in the redraw store and played back later.
	 */
	const TBool inRedraw =
		panicParams->iRedrawMode==EPanicRedrawModeNormalRedraw ||
		panicParams->iRedrawMode==EPanicRedrawModeTransRedraw ||
		panicParams->iRedrawMode==EPanicRedrawModeInvisRedraw;
	/*
	 * Drawing to an invisible window skips some of the code where errors
	 * are caught. Particularly text drawing commands that skip the actual
	 * drawing, but still process the update of the justification, this
	 * has the potential of missing parameter checks made during the actual
	 * drawing, but being caught out when processing the justification update.
	 */
	const TBool invisWin = panicParams->iRedrawMode==EPanicRedrawModeInvisRedraw;
	
	RWsSession ws;
	User::LeaveIfError(ws.Connect());

	CWsScreenDevice* scrdev=new(ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(scrdev->Construct(panicParams->iScreen));
	
	RWindowGroup group(ws);
	User::LeaveIfError(group.Construct(888));
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
	RDrawableWindow* drawWin;
	RWindow rwin(ws);
	RBackedUpWindow bwin(ws);
	TBool needsValidating=EFalse;
	if (useTransWin || inRedraw || invisWin)
		{
		drawWin=&rwin;
		needsValidating=ETrue;
		User::LeaveIfError(rwin.Construct(group,1));
		if (useTransWin)
			{
			rwin.SetTransparencyAlphaChannel();
			}
		}
	else
		{
		// EPanicRedrawModeBackedUpWindow case
		drawWin=&bwin;
		User::LeaveIfError(bwin.Construct(group, EGray4, 1));
		}
	const TSize testWinSize(100,100);
	User::LeaveIfError(drawWin->SetSizeErr(testWinSize));
	if (invisWin)
		drawWin->SetPosition(TPoint(-testWinSize.iWidth,-testWinSize.iHeight));
	drawWin->Activate();
	CWindowGc* gc;
	scrdev->CreateContext(gc);
	gc->Activate(*drawWin);
	CFbsFont* font;
	User::LeaveIfError(scrdev->GetNearestFontToDesignHeightInTwips((CFont*&)font,TFontSpec()));
	TInt ret=DoGraphicsPanicTest(ws,rwin,drawWin,gc,font,aInt,panicParams->iSubTest,inRedraw,needsValidating);
	if (ret!=EWsExitReasonFinished && invisWin)
		{
		/*
		 * Some functions are totally skipped on invisible windows, parameter
		 * errors will be harmlessly ignored in these case. To make the test
		 * pass we re-do the tests with the window now visible. The purpose
		 * of the invisible draw tests was not to check the client is always
		 * panicked doing illegal draws to invisible windows, but to make sure
		 * they had no harmful side effects.
		 */
		drawWin->SetPosition(TPoint(0,0));
		gc->Reset();
		ret=DoGraphicsPanicTest(ws,rwin,drawWin,gc,font,aInt,panicParams->iSubTest,inRedraw,needsValidating);
		}
	return(ret);
	}
LOCAL_C TInt DoMiscPanicTest(TInt aSubTest, TAny* )
	{
	const TInt KNumPanicFuncsPerMode=EWsClOpLastEnumValue;
	const TInt KNumPanicFuncModes=6;
	const TInt KNumPanicSendTests=KNumPanicFuncsPerMode*KNumPanicFuncModes;
	const TInt KNumRandGarbageTests=500;
	if (aSubTest==(KNumPanicSendTests+KNumRandGarbageTests))
		return(EWsExitReasonFinished);
	RWsSessionHacker wshacker;
	User::LeaveIfError(wshacker.Connect());
	if (aSubTest<KNumPanicSendTests)
		{
		TInt messageMode=aSubTest/KNumPanicFuncsPerMode;
		TInt msgFunc=aSubTest%KNumPanicFuncsPerMode;
		const TInt EPanicWservMessAsynchronousService=0x010000; //copy of EWservMessAsynchronousService
		const TInt EPanicWservMessAnimDllAsyncCommand=0x100000; //copy of EWservMessAnimDllAsyncCommand
		switch(messageMode%3)
			{
		case 0:
			if(msgFunc == EWservMessFinish) //RWsSession::Finish() doesn't panic
				User::Panic(KWSERV, 0); //simulate a "successful" wserv panic to skip the sub test
			break;
		case 1:
			msgFunc|=EPanicWservMessAsynchronousService;
			break;
		case 2:
			msgFunc|=EPanicWservMessAnimDllAsyncCommand;
			break;
			}
		TInt sendItErr=KErrNone;
		if (messageMode<3)
			{
			if (msgFunc&EPanicWservMessAsynchronousService)
				{
				wshacker.PanicItSend(msgFunc);
				// Async request, probably won't panic, we just want to make sure nothing crashes 'orribly
				// So do it again without the async bit and let normal handling cause the panic
				msgFunc&=~EPanicWservMessAsynchronousService;
				}
			sendItErr=wshacker.PanicItSendReceive(msgFunc);
			}
		else
			{
			TPtrC8 badDesc(reinterpret_cast<const TUint8*>(0xDEAD),100);
			TIpcArgs ipcArgs;
			ipcArgs.Set(0,&badDesc);
			if (msgFunc&EPanicWservMessAsynchronousService)
				{
				sendItErr=wshacker.PanicItSend(msgFunc,ipcArgs);
				msgFunc&=~EPanicWservMessAsynchronousService;
				}
			sendItErr=wshacker.PanicItSendReceive(msgFunc,ipcArgs);
			}
		if (sendItErr==KErrNotSupported)
			wshacker.PanicItSendReceive(EWservMessCommandBuffer); // Should always panic
		}
	else
		{
	// Fill Wserv buffer with random garbage
		RWsBufferHacker* hacker=reinterpret_cast<RWsBufferHacker*>(wshacker.WsBuffer());
		TInt64 seed=aSubTest;
		TInt retries=0;
		const TInt KMaxRandPanicRetrys=1000;
		do
			{
			const TInt maxLen=hacker->iBuf.MaxLength()-1;
			TInt writeLen=1+Math::Rand(seed)%maxLen;
			while(writeLen--)
				{
				TUint8 randData=static_cast<TUint8>(Math::Rand(seed));
				hacker->iBuf.Append(randData);
				}
			wshacker.Flush();
			retries++;
			} while(retries<KMaxRandPanicRetrys);
		}
	return(EWsExitReasonBad);
	}

LOCAL_C TInt DoCMPanicTest(TInt aInt, TAny *aScreenNumber)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	// use correct screen
	//
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(screen->Construct((TInt)aScreenNumber));

	switch(aInt)
		{
		case 1:
			ws.ComputeMode((RWsSession::TComputeMode)543);
			break;
		}
	ws.Flush();
	return(EWsExitReasonBad);
	}

#if defined(_DEBUG) && defined(__WINS__)
LOCAL_C TInt DoCKPanicTest(TInt aInt, TAny *aScreenNumber)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	// use correct screen
	//
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(screen->Construct((TInt)aScreenNumber));

	RWindowGroup group(ws);
	group.Construct(888);
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
	switch(aInt)
		{
		case 1:
			group.CancelCaptureKey(345);
			break;
		}
	ws.Flush();
	return(EWsExitReasonBad);
	}
#endif

LOCAL_C TInt DoEventPanicTest(TInt aInt, TAny *aScreenNumber)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	// use correct screen
	//
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(screen->Construct((TInt)aScreenNumber));


	switch(aInt)
		{
		case 1:
			TRequestStatus stat;
			ws.EventReady(&stat);
			ws.EventReady(&stat);
			User::After(15000000);		//15secs
			break;
		}
	ws.Flush();
	return(EWsExitReasonBad);
	}

LOCAL_C TInt DoTBufPtrTests(TInt aInt, TAny *aScreenNumber)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	// use correct screen
	//
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(screen->Construct((TInt)aScreenNumber));
	
	switch(aInt)
		{
		case 1:
			{
			TWsClCmdLoadAnimDll dt;
			dt.length=600;
			ws.TestWrite(ws.WsHandle(),EWsClOpCreateAnimDll,&dt,sizeof(dt));
			}
			break;
		case 2:
			{
			TInt len=600;
			ws.TestWrite(ws.WsHandle(),EWsClOpLogMessage,&len,sizeof(len));
			}
			break;
		case 3:
			{
			RWindowGroup group(ws);
			group.Construct(888);
			group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
			TWsWinCmdSetName dt;
			dt.length=600;
			dt.ptr=NULL;
			ws.TestWrite(group.WsHandle(),EWsWinOpSetName,&dt,sizeof(dt));
			}
			break;
		case 4:
			{
			RWindowGroup group(ws);
			group.Construct(888);
			group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
			TWsWinCmdSetName dt;
			dt.length=600;
			dt.ptr=(TDesC *)0x1234;
			ws.TestWrite(group.WsHandle(),EWsWinOpSetName,&dt,sizeof(dt));
			}
			break;
		}
	ws.Flush();
	return(EWsExitReasonBad);
	}

#if defined(_DEBUG) && defined(__WINS__)		
LOCAL_C TInt DoMismatchedCancelCaptureTest(TInt aInt, TAny *aScreenNumber)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	// use correct screen		
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(screen->Construct((TInt)aScreenNumber));
	RWindowGroup group(ws);
	User::LeaveIfError(group.Construct(888));
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
	RWindow win(ws);
	User::LeaveIfError(win.Construct(group, 1));
	win.Activate();
	TInt capture;
	
	switch (aInt)
		{
		case CTPanic::ECancelCaptureKey:
			// Create a capture that is not matched to CancelCaptureKey()
			capture = group.CaptureLongKey(' ','a',0,0,2,ELongCaptureNormal);
			ws.Flush();
			group.CancelCaptureKey(capture);
			break;
		case CTPanic::ECancelCaptureKeyUpAndDowns:
			// Create a capture that is not matched to CancelCaptureKeyUpAndDowns()
			capture = group.CaptureKey('A',0,0);
			ws.Flush();
			group.CancelCaptureKeyUpAndDowns(capture);
			break;
		case CTPanic::ECancelCaptureLongKey:
			// Create a capture that is not matched to CancelCaptureLongKey()
			capture = group.CaptureKeyUpAndDowns(EStdKeySpace,0,0);
			ws.Flush();
			group.CancelCaptureLongKey(capture);
			break;
		}
	
	ws.Flush();
	return(EWsExitReasonBad);
	}
#endif // _DEBUG

class ROverrideProtectionInRSessionBase : public RWsSession
	{
public:
	inline TInt SendReceive(TInt aFunction,TAny *aPtr) const {return(RSessionBase::SendReceive(aFunction,TIpcArgs(aPtr)));};
	};

LOCAL_C TInt DoMultiInitPanicTest(TInt , TAny *aScreenNumber)
	{
	ROverrideProtectionInRSessionBase ws;
	User::LeaveIfError(ws.Connect());
	
	// use correct screen
	//
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(screen->Construct((TInt)aScreenNumber));
	
	RWindowGroup group(ws);
	group.Construct(888);
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
	ws.Flush();
	ws.SendReceive(EWservMessInit,NULL);
	
	return(EWsExitReasonBad);
	}
	
LOCAL_C TInt DoSpritePanicTestL(TInt aTest, TAny *aScreenNumber)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	// use correct screen
	//
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(screen->Construct((TInt)aScreenNumber));
	
	RWindowGroup group(ws);
	User::LeaveIfError(group.Construct(889));
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
	RBlankWindow win(ws);
	User::LeaveIfError(win.Construct(group,898));
	RWsPointerCursor* cursor=(RWsPointerCursor*)&win;
	switch (aTest)
		{
	case 1:
		win.SetCustomPointerCursor(*cursor);
		break;
	case 2:
		ws.SetSystemPointerCursor(*cursor,0);
		break;
	case 3:
		{
		RAnimDll animDll=RAnimDll(ws);
		User::LeaveIfError(animDll.Load(KAnimDLLName));
		RTestAnim anim=RTestAnim(animDll);
		RWsSprite* sprite=(RWsSprite*)&win;
		User::LeaveIfError(anim.Construct(*sprite,EAnimTypeSprite,TPtrC8()));
		}
		break;
	case 4:
		{
		CFbsBitmap* bitmap=new(ELeave) CFbsBitmap;
		CleanupStack::PushL(bitmap);
		User::LeaveIfError(bitmap->Load(TEST_BITMAP_NAME,0));
		win.SetExtent(TPoint(),TSize(150,250));
		win.SetVisible(ETrue);
		win.Activate();
		RWsSprite sprite(ws);
		User::LeaveIfError(sprite.Construct(win,TPoint(),0));
		TSpriteMember member;
		member.iMaskBitmap=NULL;
		member.iInvertMask=EFalse;
		member.iDrawMode=CGraphicsContext::EDrawModePEN;
		member.iOffset=TPoint();
		member.iInterval=TTimeIntervalMicroSeconds32(0);
		member.iBitmap=bitmap;
		User::LeaveIfError(sprite.AppendMember(member));
		User::LeaveIfError(sprite.Activate());
		User::After(1000000); //1 sec
		User::LeaveIfError(bitmap->Resize(bitmap->SizeInPixels() + TSize(200,200)));
		User::After(1000000); //1 sec
		CleanupStack::Pop(bitmap);
		break;
		}
		}
	ws.Flush();
	return(EWsExitReasonBad);
	}

#ifdef __WINS__
LOCAL_C TInt DoDoubleConstructionTestL(TInt aTest, TAny *aScreenNumber)
	{		
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	
	// use correct screen
	//
	CWsScreenDevice* screen = new(ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(screen->Construct((TInt)aScreenNumber));
	
	RWindowGroup group(ws);
	User::LeaveIfError(group.Construct(890, EFalse));
	RBlankWindow bwin(ws);
	User::LeaveIfError(bwin.Construct(group,900));
	
	switch (aTest)
		{
	case 1:
		{
		RWsSprite sprite = RWsSprite(ws);
		User::LeaveIfError(sprite.Construct(bwin,TPoint(),0));
		sprite.Construct(bwin,TPoint(),0); //should panic
		sprite.Close();
		}
		break;
	case 2:
		{
		RWsPointerCursor cursor(ws);
		User::LeaveIfError(cursor.Construct(0));
		cursor.Construct(0); //should panic
		cursor.Close();
		}
		break;
	case 3:
		{
		RSoundPlugIn click(ws);
		User::LeaveIfError(click.Construct());
		click.Construct(); //should panic
		click.Close();
		}
		break;
	case 4:
		{
		RWindowGroup windowgroup(ws);
		User::LeaveIfError(windowgroup.Construct(901));
		windowgroup.Construct(902); //should panic
		windowgroup.Close();
		}
		break;
	case 5:
		{
		RWindow win(ws);
		User::LeaveIfError(win.Construct(group,902));
		win.Construct(group,903); //should panic
		win.Close(); 
		}
		break;
	case 6:
		{
		RBlankWindow win(ws);
		User::LeaveIfError(win.Construct(group,902));
		win.Construct(group,903); //should panic
		win.Close(); 
		}
		break;
	case 7:
		{
		RBackedUpWindow win(ws);
		User::LeaveIfError(win.Construct(group,EGray4,902));
		win.Construct(group,EGray4,903); //should panic
		win.Close(); 
		}
		break;
	case 8:
		{
		RAnimDll animDll=RAnimDll(ws);
		User::LeaveIfError(animDll.Load(KAnimDLLName));
		animDll.Load(KAnimDLLName); //should panic
		animDll.Close();
		}
		break;
	case 9:
		{
		CWindowGc *gc = new(ELeave) CWindowGc(screen);
		User::LeaveIfError(gc->Construct());
		gc->Construct(); //should panic
		delete gc;
		}
		break;
	case 10:
		{
		CWsScreenDevice* screendevice = new (ELeave) CWsScreenDevice(ws);
		User::LeaveIfError(screendevice->Construct());
		screendevice->Construct(); //should panic
		delete screendevice;
		}
		break;
	default:
		break;
		}
	ws.Flush();
	return(EWsExitReasonBad);
	}
#endif

/** Checks that activating a sprite without members will panic.
*/
LOCAL_C TInt DoTestSpriteActivatePanicL(TInt aTest, TAny *aScreenNumber)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	CWsScreenDevice* screen = new(ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(screen->Construct((TInt)aScreenNumber));
	RWindowGroup group(ws);
	User::LeaveIfError(group.Construct(891, EFalse, screen));
	RBlankWindow bwin(ws);
	User::LeaveIfError(bwin.Construct(group,892));
	
	switch(aTest)
		{
	case 1:
		{
		RWsSprite sprite = RWsSprite(ws);
		User::LeaveIfError(sprite.Construct(group,TPoint(),0));
		sprite.Activate();	//should panic here
		sprite.Close();
		}
		break;
	case 2:
		{
		RWsSprite sprite = RWsSprite(ws);
		User::LeaveIfError(sprite.Construct(bwin,TPoint(),0));
		sprite.Activate();	//should panic here
		sprite.Close();
		}
		break;
		}
	return(EWsExitReasonBad);
	}

/** Checks that activating a RWindow before calling EnableAdvancedPointers will panic the client.
*/
LOCAL_C TInt DoTestAdvancePointerActivatePanicL(TInt /*aTest*/,TAny *aScreenNumber)
    {
    RWsSession ws;
    User::LeaveIfError(ws.Connect());

    CWsScreenDevice *scrdev=new(ELeave) CWsScreenDevice(ws);
    User::LeaveIfError(scrdev->Construct((TInt)aScreenNumber));
    
    RWindowGroup group(ws);
    User::LeaveIfError(group.Construct(888));
    RWindow win(ws);
    User::LeaveIfError(win.Construct(group, 1));
    win.Activate();
    win.EnableAdvancedPointers(); // This will panic
    ws.Flush();
    return(EWsExitReasonBad);
    }


LOCAL_C TInt DoMoveToGroupPanicTestL(TInt aTest, TAny *aScreenNumber)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	// use correct screen
	//
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(screen->Construct((TInt)aScreenNumber));
	
	RWindowGroup group(ws);
	User::LeaveIfError(group.Construct(887));
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
	TInt id=group.Identifier();
	switch (aTest)
		{
	case 1:
		{
		RWindowTreeNode* win=&group;
		((RWindowBase*)win)->MoveToGroup(id);
		}
		break;
	case 2:
		{
		RBlankWindow win1(ws);
		User::LeaveIfError(win1.Construct(group,878));
		RBlankWindow win2(ws);
		User::LeaveIfError(win2.Construct(win1,788));
		win2.MoveToGroup(id);
		}
		break;
		}
	ws.Flush();
	return(EWsExitReasonBad);
	}

LOCAL_C TInt DoGetEventPanicTestL(TInt aTest, TAny *aScreenNumber)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	switch(aTest)
		{
	case 1:
		{
		TPckgBuf<TWsEvent> event;
		ws.TestWrite(ws.WsHandle(),EWsClOpGetEvent,&event,0);
		}
		break;
	case 2:
		{
		CWsScreenDevice* screen=new(ELeave) CWsScreenDevice(ws);
		User::LeaveIfError(screen->Construct((TInt)aScreenNumber));
		RWindowGroup group(ws);
		User::LeaveIfError(group.Construct(555));	// trigger a focus changed event
		TRequestStatus stat;
		ws.EventReady(&stat);
		User::WaitForRequest(stat);
		TPtrC8 badDesc(reinterpret_cast<const TUint8*>(0xDEAD),100);
		ws.TestWrite(ws.WsHandle(),EWsClOpGetEvent,&badDesc,0);
		}
		break;
		}
	ws.Flush();
	return(EWsExitReasonBad);
	}

LOCAL_C TInt DoWinHandlePanicTestL(TInt aTest, TAny *)
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	RWindowGroup group1(ws);
	RWindowGroup group2(ws);
	RWindow win1(ws);
	RWindow win2(ws);
	switch(aTest)
		{
	case 1:
		User::LeaveIfError(group1.Construct(888));
		User::LeaveIfError(group2.Construct(888));
		break;
	case 2:
		User::LeaveIfError(group1.Construct(777));
		User::LeaveIfError(win1.Construct(group1,888));
		User::LeaveIfError(win2.Construct(group1,888));
		break;
	case 3:
		User::LeaveIfError(group1.Construct(777));
		User::LeaveIfError(win1.Construct(group1,777));
		break;
	case 4:
		User::LeaveIfError(group1.Construct(777));
		User::LeaveIfError(win1.Construct(group1,0));
		break;
		}
	ws.Flush();
	return(EWsExitReasonBad);
	}

#ifdef __WINS__
LOCAL_C TInt DoDeleteScreenPanicTestL(TInt aTest, TAny *aScreenNumber)
/**
 * Test examples of use of the 'screen device deleted' panic for group windows.
 * This is issued to panic the client, if they make API calls to RWindowGroup after having deleted
 * the CWsScreenDevice with which that window group is associated.
 */
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	// point to correct screen
	CWsScreenDevice* screen = new (ELeave) CWsScreenDevice(ws);
	User::LeaveIfError(screen->Construct((TInt)aScreenNumber));
	
	RWindowGroup group(ws);
	User::LeaveIfError(group.Construct(888));
	group.EnableReceiptOfFocus(EFalse);	// Stop auto group switching on close
	switch(aTest)
		{
		// Test 1: deleting screen then modifying screen change events is illegal
		case 1:
			{
			delete screen, screen=NULL;
			group.EnableScreenChangeEvents();
			break;
			}
		// Test 2: deleting screen then setting window group name is illegal
		case 2:
			{
			delete screen, screen=NULL;
			_LIT(KPanicTest, "PanicTest");
			group.SetName(KPanicTest);
			break;
			}
		}
	ws.Flush();
	return(EWsExitReasonBad);
	}
#endif

LOCAL_C TInt DoUnInitPanicTest(TInt , TAny *)
	{
	// Creating a client session outside the test harness for panicking
	//  before initialisation as the test harness initialises the one it creates.
	TVersion version(0,0,0);
	_LIT(KServerName, "!Windowserver");
	
	RUnInitalisedConnect myUnInit;

	User::LeaveIfError(myUnInit.Connect(KServerName(), version));
	myUnInit.Send(EWservMessCommandBuffer);
	
	return(EWsExitReasonBad);
	}

void CTPanic::TestScreenDevicePanicsL()
	{
	TEST(iTest->TestWsPanicL(&DoScreenDevicePanicTest,EWservPanicBitmap,1,(TAny*)iTest->iScreenNumber));
	
	TEST(iTest->TestWsPanicL(&DoScreenDevicePanicTest,EWservPanicBitmap,2,(TAny*)iTest->iScreenNumber));
	}

void CTPanic::TestMiscPanicsL()
	{
	TBool finished=EFalse;
	TInt index=0;
	while(!finished)
		{
		const TBool result = iTest->TestWsPanicL(&DoMiscPanicTest, EWservNoPanic, index, NULL, &finished);
		if(!result)
			{
			INFO_PRINTF2(_L("TestMiscPanicsL %d failed"), index);
			TEST(result);
			}
		index++;
		}
	}

void CTPanic::LogHeapInfo()
	{
	_LIT(KInfoHeapSummary," WsHeap - Count=%d,Total=%d,Free=%d,Max free=%d");
	TPckgBuf<TWsDebugHeapInfo> heapInfo;
	TheClient->iWs.DebugInfo(EWsDebugInfoHeap,heapInfo);
	TBuf<256> infoBuf;
	infoBuf.Format(KInfoHeapSummary,heapInfo().iCount,heapInfo().iTotal,heapInfo().iAvailable,heapInfo().iLargestAvailable);
	INFO_PRINTF1(infoBuf);
	}

void CTPanic::TestGraphicsPanicsL(TClientPanic aExitReason, TInt aIndex, CTPanic::TPanicParams* aPanicParams)
	{
	TBool finished=EFalse;
	aPanicParams->iSubTest=0;

	// uncomment to show which test is being run
	INFO_PRINTF3(_L("GraphicsPanicTest %d, mode=%d"),aIndex,aPanicParams->iRedrawMode);
    RDebug::Print(_L("GraphicsPanicTest %d, mode=%d"),aIndex,aPanicParams->iRedrawMode);
	do
		{
		// uncomment for detailed view of which sub-test failed
		LogHeapInfo();
		INFO_PRINTF4(_L("GraphicsPanicTest %d/%d, mode=%d"),aIndex,aPanicParams->iSubTest,aPanicParams->iRedrawMode);
		RDebug::Print(_L("GraphicsPanicTest %d/%d, mode=%d"),aIndex,aPanicParams->iSubTest,aPanicParams->iRedrawMode);
		TEST(iTest->TestWsPanicL(&GraphicsPanicTest,aExitReason,aIndex,aPanicParams,&finished));
		aPanicParams->iSubTest++;
		} while(!finished);
	iTest->CloseAllPanicWindows();
	}

void CTPanic::TestGraphicsPanicsL(TPanicRedrawMode aRedrawMode)
	{
	CTPanic::TPanicParams pp;
	pp.iScreen=iTest->iScreenNumber;
	pp.iRedrawMode=aRedrawMode;
	static TClientPanic expectedPanics[]=
		{
		EWservPanicBufferPtr,
		EWservPanicBitmap,
		EWservPanicBitmap,
		EWservPanicBitmap,
		EWservPanicOpcode,
		EWservPanicBadPolyData,
		EWservPanicBadPolyData,
		EWservPanicOpcode,
		EWservPanicGcActive,
		EWservPanicNoFont,
		EWservPanicNoBrush,
		EWservPanicDescriptor,
		EWservPanicFont,
		EWservNoPanic,	// Marks the end of the list
		};
	TInt panicIndex=0;
	TClientPanic expectedPanic;
	while((expectedPanic=expectedPanics[panicIndex++])!=EWservNoPanic)
		{
		TestGraphicsPanicsL(expectedPanic,panicIndex,&pp);
		}
	}

void CTPanic::TestGraphicsPanicsL()
	{
	TestGraphicsPanicsL(EPanicRedrawModeBackedUpWindow);
	TestGraphicsPanicsL(EPanicRedrawModeNormalRedraw);
	if (TransparencySupportedL()==KErrNone)
		{
		TestGraphicsPanicsL(EPanicRedrawModeTransRedraw);
		}
	TestGraphicsPanicsL(EPanicRedrawModeInvisRedraw);
	}

void CTPanic::TestDeletedParentPanicsL()
	{
	TEST(iTest->TestWsPanicL(&DoDeletedParentTest,EWservPanicParentDeleted,1,(TAny*)iTest->iScreenNumber));
	}

void CTPanic::TestHandleReUseL()
	{
	for (TInt ii=2;ii<6;++ii)
		{
		TEST(iTest->TestWsPanicL(&DoHandleReUse,EWservPanicHandle,ii,(TAny*)iTest->iScreenNumber));
		}
	}

void CTPanic::TestComputeModePanicsL()
	{
	TEST(iTest->TestWsPanicL(&DoCMPanicTest,EWservPanicSetComputeMode,1,(TAny*)iTest->iScreenNumber));
	}

void CTPanic::TestCaptureKeyPanicsL()
	{
// Run this test in debug on emulator only.
// On a debug ROM the release version of the wserv.exe is included so the test can't be run as no panic happens. 
#if defined(_DEBUG) && defined(__WINS__)
	TEST(iTest->TestWsPanicL(&DoCKPanicTest,EWservPanicDestroy,1,(TAny*)iTest->iScreenNumber));
#endif
	}

void CTPanic::TestEventPanicsL()
	{
	TEST(iTest->TestWsPanicL(&DoEventPanicTest,EWservPanicReadOutstanding,1,(TAny*)iTest->iScreenNumber));
	}

void CTPanic::TestTPtrPanicsL()
	{
	TEST(iTest->TestWsPanicL(&DoTBufPtrTests,EWservPanicBufferPtr,1,(TAny*)iTest->iScreenNumber));
	}

void CTPanic::TestOpcodePanicsL()
	{
	TInt param=1;
	TBool finishTest = EFalse;
	while(!finishTest)
		{
		TEST(iTest->TestWsPanicL(&DoOpcodeTests,EWservPanicOpcode,param,(TAny*)iTest->iScreenNumber,&finishTest));
		param++;
		}
	}

void CTPanic::TestMultiInitPanicL()
	{
	TEST(iTest->TestWsPanicL(&DoMultiInitPanicTest,EWservPanicReInitialise,0,(TAny*)iTest->iScreenNumber));
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0472

@SYMDEF             DEF118618

@SYMTestCaseDesc    Test defect fixes to system panics 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Check that various defect fixes to system panics are correct.
					Also verify that reconstructing a closed object will succeed.

@SYMTestExpectedResults Panics respond correctly
*/
void CTPanic::TestDoubleConstructionL()
	{
#ifdef __WINS__
	for(TInt test=1;test<11;test++)
		{
		TEST(iTest->TestW32PanicL(&DoDoubleConstructionTestL,EW32PanicGraphicDoubleConstruction,test,NULL));
		}
	TestDoubleConstructionNoPanic();
#endif
	}
	
/** Verifies the following scenario	is valid:
	1. Create some wserv client-side objects.
	2. Call Close on them.
	3. Reconstruct them and they shouldn't panic this time. */
void CTPanic::TestDoubleConstructionNoPanic()
	{
	RWsSession ws;
	User::LeaveIfError(ws.Connect());
	
	RWindowGroup group(ws);
	User::LeaveIfError(group.Construct(890, EFalse));
	RBlankWindow bwin(ws);
	User::LeaveIfError(bwin.Construct(group,900));
	
	//RWsSprite
	RWsSprite sprite(ws);
	TEST(KErrNone == sprite.Construct(bwin,TPoint(0,0),0));
	sprite.Close();
	TEST(KErrNone == sprite.Construct(bwin,TPoint(0,0),0));
	sprite.Close();
	
	//RWsPointerCursor
	RWsPointerCursor cursor(ws);
	TEST(KErrNone == cursor.Construct(0));
	cursor.Close();
	TEST(KErrNone == cursor.Construct(0));
	cursor.Close();
	
	//RSoundPlugIn
	RSoundPlugIn click(ws);
	TEST(KErrNone == click.Construct());
	click.Close();
	TEST(KErrNone == click.Construct());
	click.Close();
	
	//RWindowGroup
	RWindowGroup windowgroup(ws);
	TEST(KErrNone ==windowgroup.Construct(901));
	windowgroup.Close();
	TEST(KErrNone ==windowgroup.Construct(901));
	windowgroup.Close();
	
	//RWindow
	RWindow win1(ws);
	TEST(KErrNone == win1.Construct(group,902));
	win1.Close(); 
	TEST(KErrNone == win1.Construct(group,902));
	win1.Close(); 
	
	//RBlankWindow
	RBlankWindow win2(ws);
	TEST(KErrNone == win2.Construct(group,902));
	win2.Close(); 
	TEST(KErrNone == win2.Construct(group,902));
	win2.Close();
	
	//RBackedUpWindow
	RBackedUpWindow win3(ws);
	TEST(KErrNone == win3.Construct(group,EGray4,902));
	win3.Close();
	TEST(KErrNone == win3.Construct(group,EGray4,902));
	win3.Close(); 
	
	//RAnimDll
	RAnimDll animDll=RAnimDll(ws);
	TEST(KErrNone == animDll.Load(KAnimDLLName));
	animDll.Close();
	TEST(KErrNone == animDll.Load(KAnimDLLName));
	animDll.Close();
	
	group.Close();
	bwin.Close();
	ws.Close();
	}

void CTPanic::TestSpritePanicsL()
	{
	for(TInt test=1;test<4;test++)
		{
		TEST(iTest->TestWsPanicL(&DoSpritePanicTestL,EWservPanicSprite,test,(TAny*)iTest->iScreenNumber));
		}
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0475

@SYMDEF             DEF118616

@SYMTestCaseDesc    Test defect fixes to system panics 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Check that various defect fixes to system panics are correct.

@SYMTestExpectedResults Panics respond correctly
*/
void CTPanic::TestSpriteActivatePanicL()
	{
	for(TInt test=1;test<3;test++)
		{
		TEST(iTest->TestWsPanicL(&DoTestSpriteActivatePanicL,EWservPanicNoSpriteMember,test,NULL));
		}
	}

void CTPanic::TestMoveToGroupPanicsL()
	{
	TEST(iTest->TestWsPanicL(&DoMoveToGroupPanicTestL,EWservPanicOpcode,1,(TAny*)iTest->iScreenNumber));
	TEST(iTest->TestWsPanicL(&DoMoveToGroupPanicTestL,EWservPanicNotTopClient,2,(TAny*)iTest->iScreenNumber));
	}

void CTPanic::TestDeleteScreenPanicL()
	{
#ifdef __WINS__
	TEST(iTest->TestWsPanicL(&DoDeleteScreenPanicTestL,EWservPanicGroupWinScreenDeviceDeleted,1,(TAny*)iTest->iScreenNumber));
	TEST(iTest->TestWsPanicL(&DoDeleteScreenPanicTestL,EWservPanicGroupWinScreenDeviceDeleted,2,(TAny*)iTest->iScreenNumber));
#endif
	}

void CTPanic::TestWinHandleErrors()
	{
#ifdef __WINS__
	TEST(iTest->TestWsPanicL(&DoWinHandlePanicTestL,EWservPanicDuplicateHandle,1));
	TEST(iTest->TestWsPanicL(&DoWinHandlePanicTestL,EWservPanicDuplicateHandle,2));
	TEST(iTest->TestWsPanicL(&DoWinHandlePanicTestL,EWservPanicDuplicateHandle,3));
#endif
	TEST(iTest->TestWsPanicL(&DoWinHandlePanicTestL,EWservPanicNullHandle,4));
	}

void CTPanic::TestGetEventErrors()
	{
	TEST(iTest->TestWsPanicL(&DoGetEventPanicTestL,EWservPanicUnsignalledEventData,1,(TAny*)iTest->iScreenNumber));
	TEST(iTest->TestWsPanicL(&DoGetEventPanicTestL,EWservPanicDescriptor,2,(TAny*)iTest->iScreenNumber));
	}

void CTPanic::TestUnInitPanicL()
	{
	TEST(iTest->TestWsPanicL(&DoUnInitPanicTest,EWservPanicUninitialisedClient,0,(TAny*)iTest->iScreenNumber));
	}

void CTPanic::TestEnableAdvancePointerActivatePanicL()
    {
    TEST(iTest->TestWsPanicL(&DoTestAdvancePointerActivatePanicL,EWservPanicUnableToEnableAdvPointer, 0, (TAny*)iTest->iScreenNumber));
    }

void CTPanic::ConstructL()
	{
	TheGc->Activate(*BaseWin->Win());
	TheGc->Clear();
	TheGc->SetBrushColor(TRgb::Gray16(12));
	TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheGc->SetPenStyle(CGraphicsContext::ENullPen);
	TheGc->DrawRect(TRect(BaseWin->Win()->Size()));
	TheGc->Deactivate();
	TheGc->Activate(*TestWin->Win());
	TheGc->Clear();
	TheGc->SetBrushColor(TRgb::Gray16(4));
	TheGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	TheGc->SetPenStyle(CGraphicsContext::ENullPen);
	TheGc->DrawRect(TRect(TestWin->Win()->Size()));
	TheGc->Deactivate();
//
	_LIT(KReportFullRom,"Warning full ROM, EikSrv present, panic dialogs may interfere with tests");
	_LIT(KReportGoodRom,"ROM OK, No EikSrv present");
	if (iTest->IsFullRomL())
		{
		INFO_PRINTF1(KReportFullRom);
		}
	else
		{
		INFO_PRINTF1(KReportGoodRom);
		}
	}

void CTPanic::TestAlphaBlendingPanicL()
	{
	INFO_PRINTF1(_L("Masked transparency support has been removed."));
	}
	
void CTPanic::TestMismatchedCaptureCancelPanicL()
	{		
// Run this test in debug on emulator only.
// On a debug ROM the release version of the wserv.exe is included so the test can't be run as no panic happens. 
#if defined(_DEBUG) && defined(__WINS__)		
	TEST(iTest->TestWsPanicL(DoMismatchedCancelCaptureTest,EWservPanicDestroy,ECancelCaptureKey,(TAny*)iTest->iScreenNumber));
	TEST(iTest->TestWsPanicL(DoMismatchedCancelCaptureTest,EWservPanicDestroy,ECancelCaptureKeyUpAndDowns,(TAny*)iTest->iScreenNumber));
	TEST(iTest->TestWsPanicL(DoMismatchedCancelCaptureTest,EWservPanicDestroy,ECancelCaptureLongKey,(TAny*)iTest->iScreenNumber));
#endif
	}

void CTPanic::RunTestCaseL(TInt /*aCurTestCase*/)
	{
	((CTPanicStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(++iTest->iState)
		{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0259

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test various system panics 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Check that various system panics respond correctly 

@SYMTestExpectedResults Panics respond correctly
*/
		case 1:
			{
			((CTPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0259"));
			iTest->LogSubTest(_L("TestCaptureKeyPanicsL"));
			TestCaptureKeyPanicsL();
			}
			break;
		case 2:
		    {
			iTest->LogSubTest(_L("TestEventPanicsL"));
			TestEventPanicsL();
		    }
		    break;
		case 3:
		    {
			iTest->LogSubTest(_L("TestComputeModePanicsL"));
			TestComputeModePanicsL();
		    }
		    break;
		case 4:
		    {
#ifdef __WINS__
//  Only running this under WINS as the tests are a bit excessive, firing off all sorts of illegal
// opcode/flag combinations, as well as buffers of random data.
// Currently on ARM builds they're failing with KErrOutOfMemory, probably running out of handles
// somewhere in the OS.
			iTest->LogSubTest(_L("TestMiscPanicsL"));
			TestMiscPanicsL();
#endif
		    }
		    break;
		case 5:
		    {
			iTest->LogSubTest(_L("TestGraphicsPanicsL"));
			TestGraphicsPanicsL();
		    }
		    break;
		case 6:
		    {
			iTest->LogSubTest(_L("TestTPtrPanicsL"));
			TestTPtrPanicsL();
		    }
		    break;
		case 7:
		    {
			iTest->LogSubTest(_L("TestOpcodePanicsL"));
			TestOpcodePanicsL();
		    }
		    break;
		case 8:
		    {
			iTest->LogSubTest(_L("TestScreenDevicePanicsL"));
			TestScreenDevicePanicsL();
		    }
		    break;
		case 9:
		    {
			iTest->LogSubTest(_L("TestMultiInitPanicL"));
			TestMultiInitPanicL();
			}
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0260

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test various system panics 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Check that various system panics respond correctly 

@SYMTestExpectedResults Panics respond correctly
*/
		case 10:
			{
			((CTPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0260"));
			iTest->LogSubTest(_L("Panic 2"));
			TestSpritePanicsL();
			TestMoveToGroupPanicsL();
			}
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0261

@SYMDEF             DEF081259

@SYMTestCaseDesc    Test defect fixes to system panics 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Check that various defect fixes to system panics are correct 

@SYMTestExpectedResults Panics respond correctly
*/			
		case 11:
			{
			((CTPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0261"));
			_LIT(KPanicTest,"Defect Fixes (Pan.3)");
			iTest->LogSubTest(KPanicTest);
			TestDeletedParentPanicsL();
			TestHandleReUseL();
			TestDeleteScreenPanicL(); // DEF069809
			}
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-097969-0001

@SYMDEF             DEF097969

@SYMTestCaseDesc    Test defect fixes to system panics 

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Check that various defect fixes to system panics are correct 

@SYMTestExpectedResults Panics respond correctly
*/			
		case 12:
			{
			((CTPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-097969-0001"));
			_LIT(KPanicTest,"Server panic defect Fix (Pan.4)");
			iTest->LogSubTest(KPanicTest);
			TestUnInitPanicL();	// DEF097969
			}
			break;
		
		case 13:
			{
/**
@SYMTestCaseID		GRAPHICS-WSERV-0501
*/
			((CTPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0501"));
			_LIT(KPanicTest,"AlphaBlending Bitmap panic defect Fix (Pan.5)");
			iTest->LogSubTest(KPanicTest);
			TestAlphaBlendingPanicL();	// DEF112916
			}
			break;
		case 14:
			{		
            ((CTPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0472"));
			_LIT(KPanicTest,"Double construction panic test");
			iTest->LogSubTest(KPanicTest);
			TestDoubleConstructionL();	// DEF118618
			}
			break;
		case 15:
			{
			((CTPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0475"));
			_LIT(KPanicTest, "RWsSprite Activate() without members panic test");
			iTest->LogSubTest(KPanicTest);
			TestSpriteActivatePanicL(); //DEF118616				
			}
			break;
/**
@SYMTestCaseID		GRAPHICS-WSERV-0497

@SYMDEF             DEF133776

@SYMTestCaseDesc     Test that a debug only panic occurs when an attempt
 is made to cancel a key capture using the wrong cancel capture API.

@SYMTestPriority    High

@SYMTestStatus      Implemented

@SYMTestActions     Check that calling the each RWindowGroup::CancelCapture***() API
 using the handle returned from a mismatched RWindowGroup::Capture***() call causes
 a debug only panic. Tests each of the three RWindowGroup::CancelCapture***() APIs.

@SYMTestExpectedResults Panics respond correctly in debug only.
*/			
		case 16:
			{
			((CTPanicStep*)iStep)->SetTestStepID(_L("GRAPHICS-WSERV-0497"));
			_LIT(KPanicTest,"TestMismatchedCaptureCancelPanicL");
			iTest->LogSubTest(KPanicTest);
			TestMismatchedCaptureCancelPanicL();				
			}
			break;			
#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
		case 17:
			{
			_LIT(KPanicTest, "Non-Redraw Drawing inside Redrawer Panic Test");
			iTest->LogSubTest(KPanicTest);
			TestNonRedrawRedrawerL();
			}
			break;
	  case 18:
      {
      _LIT(KPanicTest, "Enable advanced pointer Panic Test");
      iTest->LogSubTest(KPanicTest);
      TestEnableAdvancePointerActivatePanicL();
      }   
			break;
						
#endif			
		default:
            		((CTPanicStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
			((CTPanicStep*)iStep)->CloseTMSGraphicsStep();
			TestComplete();
			break;
		}
	((CTPanicStep*)iStep)->RecordTestResultL();
	}

#ifdef TEST_GRAPHICS_WSERV_TAUTOSERVER_NGA
LOCAL_C TInt DoTestNonRedrawRedrawerL(TInt /* aInt */, TAny * /* aPtr */)
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);
	
	RWsSession session;
	User::LeaveIfError(session.Connect());
	
	CWsScreenDevice *screenDevice = new (ELeave)CWsScreenDevice(session);
	User::LeaveIfError(screenDevice->Construct ());
	CWindowGc *gc;
	User::LeaveIfError(screenDevice->CreateContext(gc));
	
	CNonRedrawWin *nonRedrawWin=CNonRedrawWin::NewL(session, gc);	
	nonRedrawWin->Invalidate(); 
	CActiveScheduler::Start();
	User::After(2000000); // so the Redrawer has a chance to run 
	CActiveScheduler::Stop();
	delete gc;
	delete screenDevice;
	session.Close();
	CleanupStack::PopAndDestroy(scheduler);
	return(EWsExitReasonBad);
	}

/**
@SYMTestCaseID		GRAPHICS-WSERV-0121808
@SYMDEF             DEF121808: No more Non-Redraw drawing for NGA (test added during DEF134308) 
@SYMTestCaseDesc    Test defect fixes to system panics 
@SYMTestPriority    High
@SYMTestStatus      Implemented
@SYMTestActions     Check that WServ panics a client which uses Non-Redraw drawing in the Redrawer.
@SYMTestExpectedResults Panics respond correctly
*/
void CTPanic::TestNonRedrawRedrawerL()
	{
	TEST(iTest->TestWsPanicL(&DoTestNonRedrawRedrawerL,EWservPanicWindowBeginRedrawNotCalled,NULL,NULL));
	}

CNonRedrawWin::CRedrawer::CRedrawer(CNonRedrawWin* aWd) :
	CActive(CActive::EPriorityStandard), iWd(aWd){
	CActiveScheduler::Add(this);
	HandleRedrawEvent();	
}

CNonRedrawWin::CRedrawer::~CRedrawer(){
	Cancel();
}

void CNonRedrawWin::CRedrawer::HandleRedrawEvent(){
	iWd->GetSession().RedrawReady(&iStatus);
	SetActive();
}

void CNonRedrawWin::CRedrawer::RunL(){
	TWsRedrawEvent redrawEvent;
	iWd->GetSession().GetRedraw(redrawEvent);	
	iWd->Redraw();
	HandleRedrawEvent();
}

void CNonRedrawWin::CRedrawer::DoCancel(){
	iWd->GetSession().RedrawReadyCancel();
}

CNonRedrawWin* CNonRedrawWin::NewL(RWsSession &aSession, CWindowGc *aGc){
	CNonRedrawWin* self=new(ELeave)CNonRedrawWin(aSession, aGc);
	CleanupStack::PushL(self);
	self->ConstrucL();
	CleanupStack::Pop(self);
	return self;
}

CNonRedrawWin::CNonRedrawWin(RWsSession &aSession, CWindowGc *aGc):
	iSession(aSession), iGc(aGc){}

CNonRedrawWin::~CNonRedrawWin(){
	delete iRedrawer;
	iWd.Close();
	iWdGrp.Close();	
}

void CNonRedrawWin::Redraw(){
	// This is a Non-Redraw Drawing Redrawer; BeginRedraw()/EndRedraw()
	// have been intentionally omitted.
	iGc->Activate(iWd);	
	iGc->SetBrushColor(TRgb(255,0,0));
	iGc->SetPenColor(KRgbBlue);
	iGc->SetPenSize(TSize(10,20));
	iGc->DrawRect(TRect(TPoint(10,10),TPoint(50,50)));	
	iGc->Deactivate();
	iSession.Finish();			
}

RWsSession &CNonRedrawWin::GetSession(){return iSession;}

void CNonRedrawWin::Invalidate(){iWd.Invalidate();}

void CNonRedrawWin::ConstrucL(){
	iWdGrp=RWindowGroup(iSession);	
	iWdGrp.Construct((TUint32)this,ETrue);
	_LIT(KWndGrpName,"NonRedrawWndGrp");
	iWdGrp.SetName(KWndGrpName);	
	iWd=RWindow(iSession);	
	iWd.Construct(iWdGrp, 0x101);	
	User::LeaveIfError(iWd.SetExtentErr(TPoint(0,0),TSize(150,150)));
	iWd.SetBackgroundColor(KRgbWhite);
	iWd.SetOrdinalPosition(0);	
	iWd.Activate();	
	iRedrawer=new(ELeave) CRedrawer(this);
}
#endif

TInt RUnInitalisedConnect::Connect(const TDesC &aName, const TVersion &aVersion)
	{
	return CreateSession(aName, aVersion, 255);
	}

TInt RUnInitalisedConnect::Send(const TInt aMsg)
	{
	return SendReceive(aMsg);
	}

__WS_CONSTRUCT_STEP__(Panic)
#pragma warning( disable : 4505 )
