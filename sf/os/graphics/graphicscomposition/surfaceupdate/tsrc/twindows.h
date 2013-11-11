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

#ifndef __TWINDOWS_H__
#define __TWINDOWS_H__

#include <e32base.h>

class TWsEvent;
class TWsRedrawEvent;
class RWindowTreeNode;
class RWindow;
class RWindowGroup;
class RBlankWindow;
class RBackedUpWindow;
class RWsSession;
class CWindowGc;
class CWsScreenDevice;

/**
 * The class handles redrawing of the window tree node standard class 
 */
class CTWindowTreeNode : public CBase
	{
public:
	virtual void Event(TWsEvent & aEvent) = 0;
	virtual void Redraw(TWsRedrawEvent & aEvent) = 0;
	virtual RWindowTreeNode* WindowTreeNode() 	{ return iWin; }
protected:
	RWindowTreeNode* iWin;
	};

/**
 * The class handles redrawing of the window window group
 */
class CTWindowGroup : public CTWindowTreeNode
	{
public:
	static CTWindowGroup* NewL(RWsSession& aWs, CWsScreenDevice* aScreenDevice);
	~CTWindowGroup();
	void Event(TWsEvent & aEvent);
	void Redraw(TWsRedrawEvent & aEvent);
	RWindowGroup* WindowGroup() { return reinterpret_cast<RWindowGroup*>(iWin); }
private:
	CTWindowGroup();
	void ConstructL(RWsSession & aWs, CWsScreenDevice* aScreenDevice);
	};

/**
 * Base class for windows.
 */
class CTWindow : public CTWindowTreeNode
	{
public:
	static CTWindow* NewL(RWsSession& aWs, CTWindowTreeNode& aGroup, CWindowGc& aGc);
	~CTWindow();
	void Event(TWsEvent & aEvent);
	void Redraw(TWsRedrawEvent & aEvent);
	RWindow* Window() { return reinterpret_cast<RWindow*>(iWin); }
	virtual void Draw();
private:
	CWindowGc& iGc;
private:
	CTWindow(CWindowGc& aGc);
	void ConstructL(RWsSession& aWs, CTWindowTreeNode& aGroup);
	};

#endif //__TWINDOWS_H__
