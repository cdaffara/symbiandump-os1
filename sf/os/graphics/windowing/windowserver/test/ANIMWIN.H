// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Animated Bitmap Window classes
// 
//

#ifndef __ANIMWIN_H__
#define __ANIMWIN_H__

#include <e32std.h>
#include <e32base.h>
#include "W32STD.H"
#include "../test/tlib/testbase.h"

_LIT(VIDEO_MBM_NAME,"Z:\\WSTEST\\MAILANIM.MBM");

class RMbmAnim : public RAnim
	{
public:
	inline RMbmAnim(RAnimDll& aDll) :RAnim(aDll) {}
	void ConstructL(const RWindowBase &aDevice);
	void SetMbmFile(const TBuf<32>& aFileName);
	void Activate();
	void Deactivate();
	void SpeedUp();
	void SlowDown();
	};

class TestWindow11 : public CTWin
	{
public:
	TestWindow11();
	~TestWindow11();
	void Draw();
	void InitWinL();
	void WinKeyL(const TKeyEvent &aKey,const TTime &);
private:
	void LoadDllL();
	void CheckWindowSize();
private:
	RAnimDll* iAnimDll;
	RMbmAnim* iAnim;
	TSize iVideoSize;
	};

#endif
