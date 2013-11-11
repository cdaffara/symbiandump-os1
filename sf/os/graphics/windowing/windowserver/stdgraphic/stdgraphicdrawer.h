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
//

#ifndef __STDGRAPHICDRAWER_H__
#define __STDGRAPHICDRAWER_H__

#include "graphics/WSGRAPHICDRAWER.H"
#include <graphics/WSGRAPHICDRAWERINTERFACE.H>
#include <icl/imagedata.h>
#include "stdgraphictestdrawer.h"

class CFbsBitmap;
class MWsGraphicDrawerContext;

NONSHARABLE_CLASS(CWsGraphicDrawerBitmap): public CWsGraphicDrawer
	{
public:
	enum 
		{
		EImplUid = 0x10281921
		};
	static CWsGraphicDrawerBitmap* CreateL();
	~CWsGraphicDrawerBitmap();
	void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,MWsClient& aOwner,const TDesC8& aData);
	void HandleMessage(const TDesC8& aData);
private:
	CWsGraphicDrawerBitmap();
	void DoDraw(MWsGc& aGc,const TRect& aRect,const TDesC8& aData) const;
private:
	MWsGraphicDrawerContext* iContext;
	CFbsBitmap* iBitmap;
	CFbsBitmap* iMask;
	};

NONSHARABLE_CLASS(CWsGraphicDrawerBitmapAnimation): public CWsGraphicDrawer
	{
public:
	enum 
		{
		EImplUid = 0x10281AAE
		};
	static CWsGraphicDrawerBitmapAnimation* CreateL();
	~CWsGraphicDrawerBitmapAnimation();
	void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TGraphicDrawerId& aId,MWsClient& aOwner,const TDesC8& aData);
	void HandleMessage(const TDesC8& aData);
private:
	CWsGraphicDrawerBitmapAnimation();
	void DoDraw(MWsGc& aGc,const TRect& aRect,const TDesC8& aData) const;
private:
	class CFrame;
	MWsGraphicDrawerContext* iContext;
	RPointerArray<CFrame> iFrames;
	TInt64 iAnimationLength;
	CFrameRate iFps;
	};

#endif //__STDGRAPHICDRAWER_H__
