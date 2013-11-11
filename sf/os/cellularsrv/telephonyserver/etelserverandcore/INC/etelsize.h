// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalAll
*/

#if !defined(__ETELSIZE_H__)
#define __ETELSIZE_H__

//
// Stack and Heap Sizes
// Can be varied by licensee if required
//
/**
@internalComponent
*/
const TUint KETelDefaultHeapSize=0x2000;	
const TUint KETelMaxHeapSize=0xC0000;
const TUint KETelDefaultStackSize=0x3000;	// 15.2.02 default stack size changed to 12K 
											// for practical TSY requirements
#endif
