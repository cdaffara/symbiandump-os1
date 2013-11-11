/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#if (!defined __DRAWUTILS_H__)
#define __DRAW_UTILS_H__

//	EPOC Includes
#include <hal.h>

//	User Includes
#include "DataWrapperBase.h"



class CDrawUtils
/**
*	Test Utilities class
@test
@publishedPartner
*/ 
	{
public:
	static	void 	ColourFillUtility(TUint8 aColorVal,const TPoint& aPos);
	static	void    DrawLineUtility(const TPoint& aStartPos, const TPoint& aEndPos,const TUint8 aColorVal);
	static	void 	DrawSquareUtility(const TPoint& aTopLeft,const TInt aBoxHeight,const TInt aBoxWidth,TUint8 aColorVal);
	static	void    DrawSquareOutLineUtility(const TPoint& aTopLeft,const TInt aBoxHeight,const TInt aBoxWidth,TUint8 aColorVal);
	};


 #endif /* __DRAW_UTILS_H__ */
 
 



