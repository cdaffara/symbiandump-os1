// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// LOADSTXT.CPP
// used for timing text printing
// 
//

#include "TTIME.H"

TInt LoadsOfText(TInt aOwningGroup)
	{
	RWsSession ws;
	ws.Connect();
	CWsScreenDevice *device=new(ELeave) CWsScreenDevice(ws);
	device->Construct();
	RWindowGroup group(ws);
	group.Construct(ENullWsHandle);
	group.SetOwningWindowGroup(aOwningGroup);
	TSize scrSize(device->SizeInPixels());
//
	RWindow window(ws);
	window.Construct(group,ENullWsHandle);
	window.SetExtent(TPoint(), scrSize);
	window.Activate();
//
	CWindowGc *gc;
	device->CreateContext(gc);
	window.BeginRedraw();
	gc->Activate(window);
	gc->Clear();
	window.EndRedraw();
	TFontSpec fspec(KTestFontTypefaceName,200);
	CFbsFont *font;
	User::LeaveIfError(device->GetNearestFontToDesignHeightInTwips((CFont *&)font, fspec));
	gc->UseFont(font);
	TBuf<100> loadsatext(_L("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890zyxwvutsrqponmlkjihgfedcba"));
	TInt ascent=font->AscentInPixels();
	TInt fheight=font->HeightInPixels();
	for(TInt nTimes=0;nTimes<10;nTimes++)
		{
		TPoint pos;
//		for(pos.iY=ascent;pos.iY<scrSize.iHeight;pos.iY+=font->HeightInPixels())
//			gc->DrawText(loadsatext,pos);
		for(pos.iY=0;pos.iY<scrSize.iHeight;pos.iY+=fheight)
			gc->DrawText(loadsatext,TRect(pos,TPoint(scrSize.iWidth,pos.iY+fheight)),ascent);
		gc->Clear();
		}
	gc->Deactivate();
//
	ws.Flush();
	delete gc;
	device->ReleaseFont(font);
	window.Close();
	group.Close();
	delete device;
	ws.Close();
	return(KErrNone);
	}

GLDEF_D TTimeTestHeader LoadsaText={_S("Loads of text"),LoadsOfText};
