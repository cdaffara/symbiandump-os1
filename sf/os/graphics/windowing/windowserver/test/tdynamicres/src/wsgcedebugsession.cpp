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

/**
 @file
*/

#include "wsgcedebugsession.h"

RWsDebugSession::RWsDebugSession(TInt aScreenNo /*= -1*/):
	iScreenNo(aScreenNo),
	iSurfaceUpdateSerial(-1)
	{
	iSurfaceListBuffer.CreateMax(48);
	}

RWsDebugSession::~RWsDebugSession()
	{
	iSurfaceListBuffer.Close();
	
	}

TBool RWsDebugSession::ResetUpdated()
	{
	TInt surfaceUpdateSerial=DebugInfo(EWsDebugSerialSurfacesUpdated,iScreenNo);
	TBool retVal=(iSurfaceUpdateSerial!=surfaceUpdateSerial);
	iSurfaceUpdateSerial=surfaceUpdateSerial;
	return retVal;
	}

TInt RWsDebugSession::ResetUpdatedAndGetSurfaceWindowList(const TWsDebugWindowId * & aWinList)
	{
	iSurfaceUpdateSerial=DebugInfo(EWsDebugSerialSurfacesUpdated,iScreenNo);
	TInt reqSize=DebugInfo(EWsDebugSurfaceWindowList,iSurfaceListBuffer,iScreenNo);
	while (reqSize>iSurfaceListBuffer.MaxLength())
		{
		iSurfaceListBuffer.Close();
		iSurfaceListBuffer.CreateMax(reqSize);
		iSurfaceUpdateSerial=DebugInfo(EWsDebugSerialSurfacesUpdated,iScreenNo);
		reqSize=DebugInfo(EWsDebugSurfaceWindowList,iSurfaceListBuffer,iScreenNo);
		}
	if (reqSize==KErrCancel)
		{
		aWinList=NULL;
		return 0;
		}
	else
		{
		aWinList=(TWsDebugWindowId*)iSurfaceListBuffer.Ptr();
		return reqSize/sizeof(TWsDebugWindowId);
		}
	}
/**
 * Stream data into the provided buffer and point the return object pointer to it.
 * The pointer is left null if the method returns an error code or the buffer is too small.
 * The ret5urn code reports 
 * 
 **/
TInt	RWsDebugSession::DebugInfo(TWsDebugInfoFunc aFunction, TInt aParam, TDes8& aHostBuffer,const void*&aReturnedObject,TInt aObjectSize)const
	{
	aHostBuffer.SetMax();
	TInt reqSize=DebugInfo(aFunction,aHostBuffer,aParam);
	aReturnedObject=NULL;
	if (reqSize<0)
		{
		return reqSize;	//Error code is transmitted unmolested
		}
	if (reqSize==0)		//Size 0 is transformed to max
		reqSize=aHostBuffer.MaxLength();
	if ((reqSize%aObjectSize)!=0)
		{				//Size not multiple of object --> error
		return KErrCorrupt;
		}
	if (reqSize<=aHostBuffer.MaxLength())
		{		//Pointer is only set if data fits buffer
		aReturnedObject=(const void*)aHostBuffer.Ptr();
		}
	reqSize/=aObjectSize;	//Return the exact number of objects filled
	return reqSize;
	}
/**
 * Stream the reply data via the buffer associated with the region.
 * Some protected accessor optimisation used to manage the reported size of the region after streaming.
 * 
 **/
TInt	RWsDebugSession::DebugInfo(TWsDebugInfoFunc aFunction, TInt aParam, TRegion& aPreAllocatedReturnedRegion)const
	{
	//Attempt to fit the received data in the preexisting region buffer...
	class XRegion:public TRegion
		{
		public:
			using TRegion::AppendRect;
		//	using TRegion::SetListSize;
			using TRegion::RectangleListW;
			TInt MaxSize()				
				{ return iAllocedRects;	}
			void  SetListSize(TInt aNewSize)	//DANGER no error checking!!!				
				{ iCount=aNewSize;	}
		}& preAllocatedReturnedRegion=(XRegion&)aPreAllocatedReturnedRegion;
	typedef TRect TElt;
	TInt reqSize=preAllocatedReturnedRegion.MaxSize();
	const TElt* elements=preAllocatedReturnedRegion.RectangleListW();
	TInt lenBytes=reqSize*sizeof(TElt);
	TPtr8 pBuff((TUint8*)elements,lenBytes,lenBytes);
	reqSize=DebugInfo(aFunction,aParam,pBuff,elements);
	
	if (elements)
		{
		if (reqSize==0)
			{
			reqSize=preAllocatedReturnedRegion.MaxSize();
			}
		preAllocatedReturnedRegion.SetListSize(reqSize);
		return reqSize;
		}
	//If data does not fit in preexisting buffer
	//Use a temp array instead
	//Still try to block copy into the existing capacity
	//I think this use of region copy is more efficient than appending,
	//and definitely better than unioning the elements.
	if (reqSize>=0)
		{
		TInt breakLoop=10;
		do	{
			TElt* tempbuff=new TElt[reqSize];
			if (tempbuff==NULL)
				{
				reqSize=KErrNoMemory;
				break;
				}
			elements=tempbuff;
			TInt lenBytes=reqSize*sizeof(TElt);
			TPtr8 pBuff((TUint8*)elements,lenBytes,lenBytes);
			TInt reqSize2=DebugInfo(aFunction,aParam,pBuff,elements);
			if (reqSize2!=0)
				reqSize=reqSize2;
			if (elements)
				{
				RRegion r(reqSize,tempbuff);	//note this region does not own its memory so should not be closed!
				aPreAllocatedReturnedRegion.Copy(r);
				if (aPreAllocatedReturnedRegion.CheckError())
					reqSize=KErrNoMemory;
				}
			delete[] tempbuff;
			}while (reqSize>0 && elements==NULL && --breakLoop);
		}
	if (reqSize>=0 && elements==NULL)
		{
		preAllocatedReturnedRegion.ForceError();
		reqSize=KErrTimedOut;
		}
	if (reqSize<0)
		{
		if (reqSize==KErrCancel)
			{
			preAllocatedReturnedRegion.Clear();
			}
		else
			{
			preAllocatedReturnedRegion.ForceError();
			}
	
		}
	return reqSize;
	}
