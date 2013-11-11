// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Converts logical coordinate value to physical coordinate value.
// Then adjusts the value, assuming that it must be within the drawing rectangle.
// The first, "<0" check is arguable - should it be there or not!?
// Because it HIDES negative values of "x" coordinate, so possible programmer's errors!
// But with used logical to physical transformation it is possible that some logical
// coordinate value might be transformed to negative physical coordinate value and 
// then it has to be checked and set to 0.
// 
//

inline TInt Log2Phys(TInt aCoordVal, TInt aScalingOrigin, TInt aScalingFactor, TInt aSize) 
	{
	aCoordVal = aScalingOrigin + aCoordVal * aScalingFactor;
	return aCoordVal < 0 ? 0 : aCoordVal >= aSize ? (aSize - 1) : aCoordVal;
	}

//Converts logical coordinate value to physical coordinate value.
//Then adjusts the value, assuming that it represents right-bottom coordinate of the logical 
//rectangle. The first, "<0" check is arguable - should it be there or not!?
//Because it HIDES negative values of "x" coordinate, so possible programmer's errors!
//But with used logical to physical transformation it is possible that some logical
//coordinate value is transformed to negative physical coordinate value and then it has to
//be checked and set to 0.
inline TInt RBtmLog2Phys(TInt aCoordVal, TInt aScalingOrigin, TInt aScalingFactor, TInt aSize) 
	{
	aCoordVal = aScalingOrigin + aCoordVal * aScalingFactor;
	return aCoordVal < 0 ? 0 : aCoordVal > aSize ? aSize : aCoordVal;
	}

