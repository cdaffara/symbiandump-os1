// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __COMMSBUFPONDINTF_H__
#define __COMMSBUFPONDINTF_H__

#include <e32base.h>

class RCommsBuf;
class CCommsBufAsyncRequest;
class MCommsBufPondDbg;

/**
This class defines a interface to communicate with the legacy mbufmanager or
with the system wide shared buffer mechanism
@internalTechnology
*/
class MCommsBufPondIntf 
	{
	public:
	/**
	Allocate a new RCommsBuf
	
	@param aSize - Requested total size
	@param aMinBufSize - The size that the allocated buffer must atleast have
	@param aMaxBufSize - The size that the allocated buffer "can" have.

	@return RCommsBuf if allocation is successful otherwise NULL.
	*/

	virtual RCommsBuf* Alloc(TInt aSize, TInt aMinBufSize, TInt aMaxBufSize) =0;
	
	virtual RCommsBuf* FromHandle(TInt aHandle) =0;

	virtual TInt Store(TDes8& aStore) const = 0;

	/**
	Frees the RCommsBuf

	@param aBuf - RCommsBuf for freeing
	*/
	virtual void Free(RCommsBuf* aBuf) =0;
	/**
	Total bytes available in the mbufmgr or shared buffer system
	*/
	virtual TInt BytesAvailable() const =0;
	/**
	Total bytes available in a pool with a given size.
	*/
	virtual TInt BytesAvailable(TInt aSize) const =0;
	/**
	Returns the buffer size that is greater than the given size.
	*/
	virtual TInt NextBufSize(TInt aSize) const =0;

	/**
	Larget buffer size available in the mbufmgr or shared buffer system
	*/
	virtual TInt LargestBufSize() const =0;
	/**
	Request to allocate a RCommsBuf asynchronously
	*/
	virtual void StartRequest(CCommsBufAsyncRequest& aRequest) =0;

	/**
	Cance the request to allocate a RCommsBuf asynchronously
	*/
	virtual void CancelRequest(CCommsBufAsyncRequest& aRequest) =0;

	/**
	Set the conext. In the case of system shared buffer manager this function is not having any implmentation
	as the context will be set by the RCommsBufPond. For mbufmgr which is another DLL needs
	to set the context for the usage of global timers etc...
	*/
	virtual void SetContext() =0;
	
	virtual void Release(RLibrary& aLib) =0;

	virtual MCommsBufPondDbg& CommsBufPondDbg() =0;
	
	};

#endif // __COMMSBUFPONDINTF_H__
