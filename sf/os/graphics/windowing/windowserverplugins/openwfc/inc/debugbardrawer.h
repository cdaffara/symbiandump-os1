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

#ifndef __DEBUGBARDRAWER_H__
#define __DEBUGBARDRAWER_H__

#include <graphics/wsgraphicscontext.h>
#include <graphics/wsrenderstage.h>

/** MWsDebugBar simple implementation 
            
@publishedPartner
@prototype
*/
class CDebugBarDrawer : public MWsDebugBar
	{	
public:		
	static CDebugBarDrawer* NewL(CWsRenderStage * aRenderStage, TInt aScreenWidth);
	~CDebugBarDrawer();			
	void DrawDebugBar(const TArray<TPtrC>& aDebugText);			
	
private:
	CDebugBarDrawer(CWsRenderStage * aRenderStage, TInt aScreenWidth);
	void ConstructL();
	
private:
	enum {KFontHeightInPixel = 10};
	
	CWsRenderStage * iRenderStage;
	MWsGraphicsContext * iGc;

	TInt iScreenWidth;
	TRect iDebugBarRect;
	CFbsTypefaceStore* iFontStore;
	CFont* iFont;
	TInt iBaseline;	
	};
#endif


