// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef WSGCEDEBUGSESSION_H_
#define WSGCEDEBUGSESSION_H_

#include <w32std.h>
#include <w32debug.h>
struct TWsDebugWindowId;

class RWsDebugSession:public RWsSession
	{
public:
	RWsDebugSession(TInt aScreenNo = -1);
	~ RWsDebugSession();
	using RWsSession::DebugInfo;		//exposes the following two functions:
//	TInt	DebugInfo(TInt aFunction, TInt aParam=0) const;
//	TInt	DebugInfo(TInt aFunction, TDes8& aReturnBuf, TInt aParam=0) const;
	
	/**	Returns an array of debug data about the specified object. 
	 *  The data is placed in the aHostBuffer 
	 * 	If data is returned, aReturnedObject points to the start of the returned data, and the return value is the number of DataType objects.
	 * 	If no data is returned, aReturnedObject is NULL.
	 *  If so the return code indicates an error code, or the required number of objects.
	 * 	The object index is specified using the masks and shifts in TWsDebugClassArgMask
	 *  Various error codes may be returned from the server, some of which may simply explain why no data is available.
	 * 	The client may return 
	 * 		KErrCancel to indicate that there is zero length of data
	 * 		KErrCorrupt if the size of the returned data is not an exact multiple of DataType.
	 * 	No other type checking is performed.  
	 **/
	template <class DataType>	// DataType may be TSurfaceId, TInt,TRect, TSurfaceConfiguration (not TRegion)
	TInt	DebugInfo(TWsDebugInfoFunc aFunction, TInt aObjectIndex, TDes8& aHostBuffer,const DataType*&aReturnedObject)const
	{
	return DebugInfo(aFunction,aObjectIndex,aHostBuffer,(const void*&)aReturnedObject,sizeof(DataType));
	}
	/**	Fills the region object with the specified debug information.
	 * 	If required the region is resized to fit the data, which may fail with KErrNoMemory
	 * 	The region will be forced to error state if an error code is returned.
	 * 	Other details: see overloads
	 **/
	TInt	DebugInfo(TWsDebugInfoFunc aFunction, TInt aObjectIndex, TRegion& aPreAllocatedReturnedRegion)const;
	
	/** Indicates the start of a "consistancy-critical-read" section and fetches the window list.
	 * 	Any number of debug commands can be fetched in the section, then client can check if the were interrupted by a server recalculation.
	 **/
	TInt	ResetUpdatedAndGetSurfaceWindowList (const TWsDebugWindowId*&);
	/** Indicates the start or end of a "consistancy-critical-read" section
	 * 	Any number of debug commands can be fetched in the section, then client can check if the were interrupted by a server recalculation.
	 * 	If this method returns ETrue then the data fetched since the start may be inconsistant, and should be refetched.
	 * 	This method may also be used as a dirty way of waiting for a deferred update to occur.
	 **/
	TBool	ResetUpdated();
	/**	Returns the screen number assigned when the stub was created. 
	 **/
	TInt DebugScreenNo()	{	return iScreenNo;	}
	/** Helper method to correctly format the aObjectIndex parameter.
	 **/
	inline TInt ObjInd(TInt aWin=0,TInt aLayer=0,TInt aExtra=0)
		{	return ObjIndScr(iScreenNo,aWin,aLayer,aExtra);	}
	/** Helper method to correctly format the aObjectIndex parameter.
	 **/
	inline TInt ObjIndScr(TInt aScreen,TInt aWin=0,TInt aLayer=0,TInt aExtra=0);
	
	inline	TInt Finish(TBool aAndFlush=ETrue)
		{
		if (aAndFlush)
			Flush();
		return RWsSession::Finish();
		}
protected:
	// Resolves the template parameter to a size. See overload docs for more info.
	TInt	DebugInfo(TWsDebugInfoFunc aFunction, TInt aObjectIndex, TDes8& aHostBuffer,const void*&aReturnedObject,TInt aSize)const;
	TInt	iScreenNo;
	TInt	iSurfaceUpdateSerial;
	RBuf8	iSurfaceListBuffer;
	};
inline TInt RWsDebugSession::ObjIndScr(TInt aScreen,TInt aWin,TInt aLayer,TInt aExtra)
	{
	return 	(aScreen<<EWsDebugArgScreenShift)
		|	(aWin<<EWsDebugArgWindowShift)
		|	(aLayer<<EWsDebugArgLayerShift)
		|	(aExtra<<EWsDebugArgExtraShift);
	}

#endif /*WSGCEDEBUGSESSION_H_*/
