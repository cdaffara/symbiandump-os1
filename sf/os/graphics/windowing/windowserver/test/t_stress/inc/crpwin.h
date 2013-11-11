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

#ifndef CRPWIN_H
#define CRPWIN_H


#include <w32std.h>
#include "compwin.h"

/*******************************************************************************
Client side of the crp used by CCrpWin
*******************************************************************************/
class CCrpClient : public CWsGraphic
	{
public:
	static CCrpClient * NewL();
	~CCrpClient();
	void DrawCrp(CWindowGc& aGc,const TRect& aRect);

private:
	CCrpClient();
	void ConstructL();

private: // from CWsGraphic
	virtual void HandleMessage(const TDesC8& aData);
	virtual void OnReplace();
	};

/*******************************************************************************
These windows have a crp that draws over the full extent of the window.
Currently only draws a simple square with an elipse within (NO ANIMATION).
*******************************************************************************/
_LIT8(KCrpWindowType, "CrpDrawer");
class CCrpWin : public CCompWin
	{ 
public:
	static CCrpWin* NewLC(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc);
	static void LoadConfiguration(const MTestStepConfigurationContext* aContext);
	static TBool IsEnabled() { return iEnabled; }
	~CCrpWin();
	
	void SetSize(const TSize & aSize);
	void Redraw(const TRect& aRect);
	void DrawBitmap(CFbsBitGc* aGc, TRect& aClip, TPoint& aOrigin);
	virtual const TDesC8& TypeName() { return KCrpWindowType; }
	void DumpDetails(RFile & aFile, TInt aDepth);
	
protected:
	CCrpWin(RWsSession& aWs, RWindowGroup* aGroup, CCompWin* aParent, CWindowGc& aGc);
	void ConstructL();
	
private:
	static TBool iEnabled;
	static TBool iTransparent;
	CCrpClient* iCrp;
	};




#endif // CRPWIN_H
