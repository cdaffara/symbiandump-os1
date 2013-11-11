// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __WSTYPES_H__
#define __WSTYPES_H__

#include <e32std.h>
#include "panics.h"

/**
Enums
*/
enum TRedrawDepth
	{
	ERedrawAll,
	ERedrawTopOnly
	};

/**
<h1>Regions used as stack local variables.</h1>

We want to make regions as fast as possible but not allow memory leaks
to enter the code, as a result of missing Close() calls.

Speed is achieved by having regions as stack local arrays.  This works
for simple regions.  For complex regions, heap allocation is subsequently needed.
This then requires appropriate use of the Close() call to clean up before exit.

Memory leak checking is enabled by always allocating regions on the heap.  This
allows us to pick up on missing Close() statements.  However, this gives poor
performance.

To obtain the best of both worlds, use the STACK_REGION macro whenever a
function requires a local region buffer.  This uses local arrays for release
builds (for speed), and heap allocated memory for debug builds (for leak checking).
*/
#if defined(_DEBUG)
/** Optimise to check for memory leaks */
#define STACK_REGION RWsRegion
#else
/** Optimise for performance */
#define STACK_REGION RRegionBuf<10>
#endif

#if defined(_DEBUG)
class RWsRegion : public RRegion
	{
public:
	enum { EDefGran = 5 };
public:
	RWsRegion();
	RWsRegion(TInt aGran);
	RWsRegion(const RRegion& aRegion);
	RWsRegion(const TRect& aRect,TInt aGran=EDefGran);
	RWsRegion(TInt aCount,TRect* aRectangleList,TInt aGran=EDefGran);
	TBool Contains(const RWsRegion& aRegion) const;
	~RWsRegion() { WS_ASSERT_DEBUG(iCount == 0, EWsPanicInvalidRegion); WS_ASSERT_DEBUG(iAllocedRects == 0, EWsPanicInvalidRegion);}
	};
#else
typedef RRegion RWsRegion;
#endif

GLREF_D RWsRegion nullRegion;

/*
This class provides the functionality of an RRegionBuf, but in addition, once it
has become an RRegion it can be reverted back to using the pre-allocated buffer
by calling Reset(). 
*/
template <TInt S>
class RWsRegionBuf : public RRegionBuf<S>
	{
public:
	inline RWsRegionBuf()  {}
	inline ~RWsRegionBuf() { WS_ASSERT_DEBUG(TRegion::iCount == 0, EWsPanicInvalidRegion); }
	inline void Close()    { ASSERT(0); } //Do not use, use Reset instead.
	inline void Clear()    { ASSERT(0); } //Do not use, use Reset instead.
	inline void Reset()
		{
		//If iAllocedRects >= 0, then this instance is no longer an RRegionBuf, but has been 
		//converted into an RRegion as explained in the class comment for the RRegionBuf class.
		
		//Clear will set iCount to 0 and, if iAllocedRects>0, free heap memory and set iAllocedRects to 0.
		RRegionBuf<S>::Clear();
		
		if(TRegion::iAllocedRects >= 0)
			{
			//The magic in the next statement reverts this instance back into an RRegionBuf.
			//(The code is the same as can be found in the initialiser list of RRegionBuf's default constructor.)
			TRegion::iAllocedRects = -S&(~TRegion::ERRegionBuf);
			}
		}
	};

// Leave specific log message
#if defined(_DEBUG)
#define LEAVE_LOG_INSTALL_C \
	do { \
		class TLeaveMsg \
		{ \
		public: \
		inline static void LeaveLog(TAny *) \
			{ \
			RDebug::Printf("Leave occured at File: %s, Line: %d\n", __FILE__, __LINE__); \
			} \
		}; \
	CleanupStack::PushL(TCleanupItem(TLeaveMsg::LeaveLog)); \
	} while(0)
	
#define LEAVE_LOG_UNINSTALL_C \
	do { \
	CleanupStack::Pop(); \
	} while(0)
#else
#define LEAVE_LOG_INSTALL_C
#define LEAVE_LOG_UNINSTALL_C
#endif //_DEBUG

#endif
