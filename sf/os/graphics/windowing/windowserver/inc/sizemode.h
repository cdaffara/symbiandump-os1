// Copyright (c) 1994-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Screen Mode Size Data
// 
//

#ifndef SIZEMODE_H
#define SIZEMODE_H

/** Screen Mode Size Data

@publishedAll
@released
*/
class TSizeMode
	{
public:
	inline TSizeMode(){}
	inline TSizeMode(TSize& aSize);
	static inline TInt ScaledCord(TInt aOrigin,TInt aScale);
	inline TPoint ScaledOrigin() const;
public:
	CFbsBitGc::TGraphicsOrientation iRotation;
	TPoint iOrigin;
	TSize iScreenSize;
	TSize iScreenTwipsSize;
	TUint iAlternativeRotations;
	TRect iPointerCursorArea;
	TSize iScreenScale;
	TDisplayMode iDefaultDisplayMode;
	};

inline TSizeMode::TSizeMode(TSize& aSize) :
	iOrigin(0, 0), iScreenSize(aSize)
	{}

inline TInt TSizeMode::ScaledCord(TInt aOrigin,TInt aScale)
	{return (aOrigin+aScale-1)/aScale;}

inline TPoint TSizeMode::ScaledOrigin() const
	{return TPoint(ScaledCord(iOrigin.iX,iScreenScale.iWidth),ScaledCord(iOrigin.iY,iScreenScale.iHeight));}

#endif // SIZEMODE_H
