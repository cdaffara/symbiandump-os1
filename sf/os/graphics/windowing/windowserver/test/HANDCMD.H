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
// Command Code for handwriting AnimDLL
// 
//

#if !defined(__HANDCMD_H__)
#define __HANDCMD_H__

enum THandWritingOpcodes
	{
	EHwOpActivate,
	EHwOpDeactivate,
	EHwOpSpriteMask,
	EHwOpSetDrawData,
	EHwOpGetLastChar,
	};

struct THandwritingDrawData
	{
	TRgb iInitialBitmapColor;
	TRgb iLineColor;
	TInt iLineWidth;
	TInt iMaskLineWidth;
	TInt iEndPontWidth;
	};

#endif
