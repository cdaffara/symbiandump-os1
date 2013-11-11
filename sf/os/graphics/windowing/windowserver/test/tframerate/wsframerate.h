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
// Test GraphicTestFrameRate plugins
// 
//

/**
 @file
 @test - Test plug-in to test wsgraphic animation artwork framerate
 @internalComponent - Internal Symbian test code
*/

#ifndef __GRAPHICTESTFRAMERATE_H__
#define __GRAPHICTESTFRAMERATE_H__

#include "W32STD.H"

/**
Holds draw count values of two different animations
*/
NONSHARABLE_STRUCT(TAnimRate)
	{
	TInt iAnim1;
	TInt iAnim2;
	};
	
/** Client representation of a test anim that displays its frame-rate

@publishedPartner
@released
*/	
NONSHARABLE_CLASS(CGraphicTestFrameRate): public CWsGraphic

	{
public:
	IMPORT_C static CGraphicTestFrameRate* NewL(TUid aUid);
	IMPORT_C static CGraphicTestFrameRate* NewL(TInt aScreen);
	IMPORT_C ~CGraphicTestFrameRate();
	IMPORT_C TInt QueryPlugin(TAnimRate& aCount);
	IMPORT_C void SetCallBack(TCallBack aCallBack);
private:
	CGraphicTestFrameRate();
	void HandleMessage(const TDesC8& aData);
	void OnReplace();
private:
	TAnimRate* iCount;
	TCallBack iCallBack;
	};

#endif //__GRAPHICTESTFRAMERATE_H__
