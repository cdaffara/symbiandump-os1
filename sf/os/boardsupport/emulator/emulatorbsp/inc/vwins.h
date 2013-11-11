// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// wins\inc\vwins.h
// Wins Variant 'public' Header
// 
//

#ifndef __VWINS_H__
#define __VWINS_H__

#include <winsdef.h>
#include "nk_priv.h"

const TUint KDefaultColorDepth = 0xffffffffu;
const TUint KDefaultColorDepthGCE = 24;

class DDisplayChannel;

class DWinsUiBase : public DBase
	{
public:
	virtual TUint ColorDepth(TInt aScreenNumber) =0;
	virtual TInt SetFlip(TEmulatorFlip aFlip, TInt aScreenNumber) =0;
	virtual void Info(TVariantInfoV01& aInfo) =0;
	virtual HWND HWnd() =0;
	virtual TInt NumberOfScreens() =0;
	virtual TInt SetDisplayChannel(TInt aScreenNumber, DDisplayChannel* aDisplay) = 0;
protected:
	IMPORT_C DWinsUiBase();
	};

#endif
