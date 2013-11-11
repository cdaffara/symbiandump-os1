// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Functions to act as bench marks for various window server features
// 
//

#include "TTIME.H"

void SetUp(RWindowTreeNode &aParent, RBlankWindow &aBlank, const TPoint &aPos, const TSize &aSize, TInt aColor)
	{
	aBlank.Construct(aParent,ENullWsHandle);
	aBlank.SetColor(TRgb::Gray256(aColor));
	aBlank.SetExtent(aPos, aSize);
	aBlank.SetShadowHeight(1);
	aBlank.Activate();
	}

void doMovingWindowTest(TInt aMode, TInt aOwningGroup)
	{
	RWsSession ws;
	ws.Connect();
	CWsScreenDevice *device=new(ELeave) CWsScreenDevice(ws);
	device->Construct();
	TPoint pos(10,10);
	TSize size(100,100);
	TSize scrSize(device->SizeInPixels());
	RBlankWindow blank(ws);
	RWindowGroup group(ws);
	group.Construct(ENullWsHandle);
	group.SetOwningWindowGroup(aOwningGroup);
//
	RBlankWindow back(ws);
	SetUp(group, back, TPoint(0,0),TSize(640,240), 255);
//
	RBlankWindow blank2(ws);
	RBlankWindow blank3(ws);
	RBlankWindow blank4(ws);
	RBlankWindow blank5(ws);
	if (aMode==1)
		{
		SetUp(group, blank2, TPoint(10,10),TSize(240,150), 128);
		SetUp(blank2, blank3, TPoint(10,10),TSize(220,130), 255);
		SetUp(group, blank4, TPoint(340,-10),TSize(200,70), 64);
		SetUp(group, blank5, TPoint(390,-10),TSize(30,250), 128);
		}
//
	SetUp(group, blank,pos,size, 128);
//
	for(pos.iX=10;pos.iX<(scrSize.iWidth-10-size.iWidth);pos.iX+=1)
		{
		blank.SetPosition(pos);
//		ws.Flush();
		}
//
	if (aMode==1)
		{
		blank2.Close();
		blank3.Close();
		blank4.Close();
		blank5.Close();
		}
	blank.Close();
	back.Close();
	group.Close();
	delete device;
	ws.Close();
	}

TInt MovingWindowTest1Func(TInt aOwningGroup)
	{
	doMovingWindowTest(0,aOwningGroup);
	return(KErrNone);
	}

GLDEF_D TTimeTestHeader MovingWindowTest1={_S("Moving window test 1"),MovingWindowTest1Func};

TInt MovingWindowTest2Func(TInt aOwningGroup)
	{
	doMovingWindowTest(1,aOwningGroup);
	return(KErrNone);
	}

GLDEF_D TTimeTestHeader MovingWindowTest2={_S("Moving window test 2"),MovingWindowTest2Func};

TInt CreateAndDestroy(TInt aOwningGroup)
	{
	RWsSession ws;
	ws.Connect();
	CWsScreenDevice *device=new(ELeave) CWsScreenDevice(ws);
	device->Construct();
	RWindowGroup group(ws);
	group.Construct(ENullWsHandle);
	group.SetOwningWindowGroup(aOwningGroup);
//
	for (TInt count=0;count<100;count++)
		{
		RBlankWindow blank(ws);
		blank.Construct(group,ENullWsHandle);
		blank.SetExtent(TPoint(10,10), TSize(20,20));
		blank.Activate();
		blank.Close();
		}
	group.Close();
	delete device;
	ws.Close();
	return(KErrNone);
	}

GLDEF_D TTimeTestHeader WindowCreateDestroy={_S("Window create destroy"),CreateAndDestroy};

//

TInt WindowCreate2(TInt aOwningGroup)
	{
enum {KNumWindows=20};
	RWsSession ws;
	ws.Connect();
	CWsScreenDevice *device=new(ELeave) CWsScreenDevice(ws);
	device->Construct();
	RWindowGroup group(ws);
	group.Construct(ENullWsHandle);
	group.SetOwningWindowGroup(aOwningGroup);
	RBlankWindow shield(ws);
	shield.Construct(group,ENullWsHandle);
	shield.Activate();
	CWindowGc *gc=new(ELeave) CWindowGc(device);
	User::LeaveIfError(gc->Construct());
//
	for(TInt times=0;times<2;times++)
		{
#if 1
		TSize size(200,100);
		RBlankWindow win[KNumWindows];
		TInt count;
		for (count=0;count<KNumWindows;count++)
			{
			win[count]=RBlankWindow(ws);
			win[count].Construct(group,ENullWsHandle);
			win[count].SetExtent(TPoint(count<<2,count<<3), size);
			win[count].SetShadowHeight(1);
			win[count].SetColor(TRgb::Gray4(count&3));
			win[count].Activate();
			}
#else
		TSize size(400,200);
		RBackedUpWindow win[KNumWindows];
		TInt count;
		for (count=0;count<KNumWindows;count++)
			{
			win[count]=RBackedUpWindow(ws);
			win[count].Construct(group,EGray2,ENullWsHandle);
			win[count].SetExtent(TPoint(count<<2,count<<3), size);
			win[count].SetShadowHeight(1);
			gc->Activate(win[count]);
			gc->SetBrushColor(TRgb::Gray4(count&3));
			gc->Clear();
			gc->Deactivate();
			win[count].Activate();
			}
#endif
		ws.Flush();
		for (count=0;count<KNumWindows;count++)
			{
			win[count].SetOrdinalPosition(0);
			ws.Flush();
			}
		for (count=0;count<KNumWindows;count++)
			{
			win[count].Close();
			ws.Flush();
			}
		}
	shield.Close();
	group.Close();
	delete device;
	ws.Close();
	return(KErrNone);
	}

GLDEF_D TTimeTestHeader StackedWindowCreate={_S("Stacked window creating"),WindowCreate2};

TInt BackedUpWindowCreateTest(TInt aOwningGroup)
	{
	RWsSession ws;
	ws.Connect();
	CWsScreenDevice *device=new(ELeave) CWsScreenDevice(ws);
	device->Construct();
	RWindowGroup group(ws);
	group.Construct(ENullWsHandle);
	group.SetOwningWindowGroup(aOwningGroup);
	RBlankWindow shield(ws);
	shield.Construct(group,ENullWsHandle);
	shield.Activate();
	CWindowGc *gc=new(ELeave) CWindowGc(device);
	User::LeaveIfError(gc->Construct());
//
	RWindow win;
	TInt count;
	for (count=0;count<100;count++)
		{
		win=RWindow(ws);
		win.Construct(group,ENullWsHandle);
		win.EnableBackup();
		win.SetExtent(TPoint(10,10), TSize(200,100));
		win.Activate();
		ws.Flush();
		win.Close();
		}
	shield.Close();
	group.Close();
	delete device;
	ws.Close();
	return(KErrNone);
	}

GLDEF_D TTimeTestHeader BackedUpWindowCreate={_S("Backed up window creating"),BackedUpWindowCreateTest};

TInt BackedUpWindowMenuTest(TInt aOwningGroup)
//
// Test designed to emulate menus
//
	{
	RWsSession ws;
	ws.Connect();
	CWsScreenDevice *device=new(ELeave) CWsScreenDevice(ws);
	device->Construct();
	RWindowGroup group(ws);
	group.Construct(ENullWsHandle);
	group.SetOwningWindowGroup(aOwningGroup);
	RBlankWindow shield(ws);
	shield.Construct(group,ENullWsHandle);
	shield.Activate();
	CWindowGc *gc=new(ELeave) CWindowGc(device);
	User::LeaveIfError(gc->Construct());
//
	RWindow win;
	win=RWindow(ws);
	win.Construct(group,ENullWsHandle);
	win.EnableBackup();
	win.Activate();
	TInt count;
	TInt state=0;
	for (count=0;count<200;count++)
		{
		win.SetVisible(EFalse);
		if (state==0)
			{
			state=1;
			win.SetExtent(TPoint(10,10), TSize(200,160));
			}
		else
			{
			state=0;
			win.SetExtent(TPoint(100,10), TSize(150,200));
			}
		win.SetVisible(ETrue);
		win.BeginRedraw();
		gc->Activate(win);
		gc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		gc->SetBrushColor(TRgb::Gray4((state+1)));
		gc->Clear();
		gc->Deactivate();
		win.EndRedraw();
		ws.Flush();
		}
	win.Close();
	shield.Close();
	group.Close();
	delete device;
	ws.Close();
	return(KErrNone);
	}

GLDEF_D TTimeTestHeader MenuEmulationCreate={_S("Menu emulation"),BackedUpWindowMenuTest};

TInt BackedUpWindowMenuTest2(TInt aOwningGroup)
	{
	CFbsBitmap *bitmaps[1000];
	TInt index;
	for(index=0;index<1000;index++)
		{
		bitmaps[index]=new(ELeave) CFbsBitmap();
		bitmaps[index]->Create(TSize(10,10),EGray4);
		}
	BackedUpWindowMenuTest(aOwningGroup);
	for(index=0;index<1000;index++)
		delete bitmaps[index];
	return(KErrNone);
	}

GLDEF_D TTimeTestHeader MenuEmulationCreate2={_S("Menu emulation 2"),BackedUpWindowMenuTest2};
