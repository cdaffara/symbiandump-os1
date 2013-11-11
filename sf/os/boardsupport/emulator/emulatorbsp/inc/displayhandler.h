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

/**
 @file
 @publishedPartner
 @deprecated Emulator Deprecated
*/

#ifndef __DISPLAYHANDLER_H__
#define __DISPLAYHANDLER_H__

/**
Display request handler interface
*/
class MDisplayHandler
	{
public:
	/**
	Process a request on this logical channel.
	
	@param aReqNo	Request number:
			== KMaxTInt, a 'DoCancel' message
			>= 0, a 'DoControl' message with function number equal to aReqNo
			< 0, a 'DoRequest' message with function number equal to ~aReqNo
	@param a1		First argument. For DoRequest requests this is a pointer to
			the TRequestStatus.
	@param a2     Second argument. For DoRequest this is a pointer to the 2
			actual TAny* arguments.
	@return	Result. Ignored by device driver framework for DoRequest requests.
	*/
	virtual TInt HandleRequest(TInt aReqNo, TAny* a1, TAny* a2) = 0;
	};

#endif

