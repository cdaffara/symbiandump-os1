// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TCRX_H__
#define __TCRX_H__

#include <e32base.h>
#include <w32std.h>

class CRedrawHandler;
class CWindow;
class CWsListen;
class CWsRedir;
class CTwoWindow;

class CClient: public CActive
	{
public:
	static CClient* NewL();
	~CClient();
	void RunL();
	void DoCancel();
	inline RWsSession& Ws();
	inline RWindowGroup& Group();
	inline CWindowGc& Gc();
	inline CWindow* Win();
	inline CWsScreenDevice* Screen();
	inline CWsRedir* WsRedir();
	inline CWsListen* WsListen();
	void HandleCommand(TInt aCmd);
private:
	CClient();
	void ConstructL();
	void MakeRequest();
	void DrawTwoWindow();
	void ClearTwoWindow();

	RWsSession iWs;
	RWindowGroup iGroup;
	CWsScreenDevice* iScreen;
	CWindowGc* iGc;
	TWsEvent iEvent;
	CWindow* iWin;
	CRedrawHandler* iRedraw;
	CWsListen* iListen;
	CWsRedir* iRedir;
	RBlankWindow iBlank;
	CTwoWindow* iTwo;
	};

class CRedrawHandler: public CActive
	{
public:
	static CRedrawHandler* NewL(CClient* aClient);
	~CRedrawHandler();
	void RunL();
	void DoCancel();
private:
	CRedrawHandler(CClient* aClient);
	void ConstructL();
	void MakeRequest();

	CClient* iClient;
	};

class CWindow: public CBase
	{
public:
	CWindow(CClient* aClient);
	void ConstructL(CWindow* aParent, TBool aTransparentFlag);
	~CWindow();
	void Draw(const TRect& aRect) const;
	void DrawMenu() const;
	inline RWindow& Window();
	inline CWindow* Parent();
	inline TSize Size();
	inline CClient* Client();
private:
	CClient* iClient;
	RWindow iWin;
	CWindow* iParent;
	};

// CClient
inline RWsSession& CClient::Ws() 
	{return iWs;}
inline RWindowGroup& CClient::Group() 
	{return iGroup;}
inline CWindowGc& CClient::Gc() 
	{return *iGc;}
inline CWindow* CClient::Win() 
	{return iWin;}
inline CWsScreenDevice* CClient::Screen() 
	{return iScreen;}
inline CWsListen* CClient::WsListen()
	{return iListen;}
inline CWsRedir* CClient::WsRedir()
	{return iRedir;}
// CWindow
inline RWindow& CWindow::Window() 
	{return iWin;}
inline CWindow* CWindow::Parent() 
	{return iParent;}
inline TSize CWindow::Size() 
	{return iWin.Size();}
inline CClient* CWindow::Client() 
	{return iClient;}

#endif

