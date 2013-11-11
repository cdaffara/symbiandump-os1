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
// Some common definitions for this component
// 
//

/**
 @file
*/

#ifndef WIN_DRIVE_COMMON_H
#define WIN_DRIVE_COMMON_H

#include <e32debug.h>
#include <f32fsys.h>

//-----------------------------------------------------------------------------

//-- define this macro in order to enable debug print.
#define THIS_EXT_DEBUG_PRINT

#if (defined(_DEBUG) || defined(_DEBUG_RELEASE)) && defined(THIS_EXT_DEBUG_PRINT)
#define __PRINT(t)          {RDebug::Print(t);}
#define __PRINT1(t,a)       {RDebug::Print(t,a);}
#define __PRINT2(t,a,b)     {RDebug::Print(t,a,b);}
#define __PRINT3(t,a,b,c)   {RDebug::Print(t,a,b,c);}
#define __PRINT4(t,a,b,c,d) {RDebug::Print(t,a,b,c,d);}
#define __PRINTF(t)         {RDebug::Printf(t);}

#else
#define __PRINT(t)
#define __PRINT1(t,a)
#define __PRINT2(t,a,b)
#define __PRINT3(t,a,b,c)
#define __PRINTF(t)
#define __PRINT4(t,a,b,c,d)
#define __PRINTF(t)
#endif

//-- this logging is always enabled
#define __LOG(t)          {RDebug::Print(t);}
#define __LOG1(t,a)       {RDebug::Print(t,a);}
#define __LOG2(t,a,b)     {RDebug::Print(t,a,b);}
#define __LOG3(t,a,b,c)   {RDebug::Print(t,a,b,c);}
#define __LOG4(t,a,b,c,d) {RDebug::Print(t,a,b,c,d);}
#define __LOGF(t)         {RDebug::Printf(t);}


//-----------------------------------------------------------------------------
/**
    Zero-fill structure.
    @param apStruct     pointer to the data structure
*/
template<class T>
inline void ZeroFillStruct(T* apStruct)
{
    Mem::FillZ(apStruct,sizeof(T));
}

/**
    Zero-fill structure.
    @param apStruct     a reference to the data structure
*/
template<class T>
inline void ZeroFillStruct(T& aStruct)
{
    Mem::FillZ(&aStruct,sizeof(T));
}

/**
    Zero-fill memory. Just for consistency
    @param apStruct     pointer to the memory to be filled with zeroes
    @param aBytes       buffer size
*/
inline void ZeroFillStruct(void* aStruct, TUint aBytes)
{
    Mem::FillZ(aStruct,aBytes);
}

//-----------------------------------------------------------------------------

inline TBool IsPowerOf2(TUint32 aVal)
{
	if (aVal==0)
		return EFalse;

    return !(aVal & (aVal-1));
}



#endif //WIN_DRIVE_COMMON_H




































