// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef FETCHFRAMECUSTOMINTERFACE_H
#define FETCHFRAMECUSTOMINTERFACE_H

/** 
FetchFrame Custom Interface UID.

@publishedPartner
@prototype
*/
const TUid KUidMMFVideoFetchFrame = { 0x20024343 };

/**
This interface can be used to fetch the frame data using the buffer id.
This interface can be used by a H/W device which needs to send the frame 
data back to application.

@publishedPartner
@prototype
*/
class MMmfVideoFetchFrame
	{
	public:

	/**
	Set the handle for the surface

	@param  aBufId Buffer id whose frame data needs to be returned. 
	
	returns pointer to a frame data. memory is allocated by the Custom interface
	implementor to hold the frame data on the host side. ownership of the memory
	would be transfered to the caller of this method.
	*/
	virtual TUint8* MmvffGetFrame(TInt aBufId) = 0;
	};

#endif // FETCHFRAMECUSTOMINTERFACE_H
