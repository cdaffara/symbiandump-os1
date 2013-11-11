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
//

#ifndef SWDIRECTGDIELLIPSE_H_
#define SWDIRECTGDIELLIPSE_H_

/**
@file
@internalComponent
*/

#include <e32std.h>
#include <gdi.h>

/**
A utility class to efficiently draw and fill ellipses
*/
class TSwDirectGdiEllipse
	{
public:
	/**	
	Enumeration for the ellipse production life cycle
	*/
	enum TEllipseStatus
		{
		/**
		Ellipse has been initialised but has no area
		*/
		EInitialised,
		/**
		The ellipse is drawn as two parabolas. These are in two sectors, the top and bottom
		half of the rectangle. The first sector means the top parabola has been produced.
		*/
		EFirstSector,
		/**
		The parabola that occupies the bottom half of the rectangle has been produced. 
		*/
		ESecondSector,
		/**
		The ellipse is complete
		*/
		EComplete,
		/**
		The ellipse has either a height or width of two units or less
		and so takes the appearance of a line
		*/
		ELine
		};
public:
	void Construct(const TRect& aRect);
	TBool SingleStep(TPoint& aTopLeft,TPoint& aTopRight,TPoint& aBottomLeft,TPoint& aBottomRight);
	TBool NextStep(TPoint& aTopLeft,TPoint& aTopRight,TPoint& aBottomLeft,TPoint& aBottomRight);
	TPoint Intersection(const TRect& aRect,const TPoint& aPoint);
protected:
	TBool Output(TPoint& aTopLeft,TPoint& aTopRight,TPoint& aBottomLeft,TPoint& aBottomRight);
protected:
	TEllipseStatus iStatus;
	TInt iA;
	TInt iB;
	TInt64 iASquared;
	TInt64 iBSquared;
	TInt64 iASquBSqu;
	TInt iX;
	TInt iY;
	TInt iXAdj;
	TInt iYAdj;
	TPoint iOffset;
	TInt64 iD1;
	TInt64 iD2;
	};

#endif /*SWDIRECTGDIELLIPSE_H_*/
