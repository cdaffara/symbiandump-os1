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
// Template for writing Anim DLL's
// 
//

#if !defined(__MINANIM_H__)
#define __MINANIM_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif
#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif
/*#if !defined(__FBS_H__)
#include <fbs.h>
#endif*/
#if !defined(__W32ADLL_H__)
#include <w32adll.h>
#endif


class CMinAnimDll : public CAnimDll 
	{
public:
	CAnim *CreateInstanceL(TInt aType);
	};

class CMinWindowAnim : public CWindowAnim
	{
public:
	~CMinMbm();
	//pure virtual functions from MEventHandler
	TBool OfferRawEvent(const TRawEvent &aRawEvent);
	//pure virtual functions from CAnim
	void ConstructL(TAny *aArgs, TBool aHasFocus);
	void Animate(TDateTime *aDateTime);
	void Redraw();
	void Command(TInt aOpcode, TAny *aArgs);
	TInt CommandReplyL(TInt aOpcode, TAny *aArgs);
	void FocusChanged(TBool aState);
	};

#endif
