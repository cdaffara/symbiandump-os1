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
//

/**
 @file
 @publishedAll
 @released
*/

#ifndef REMCONABSVOLTARGETOBSERVER_H
#define REMCONABSVOLTARGETOBSERVER_H

#include <e32base.h>

/**
Client-implemented mixin- notifies the client of incoming AbsVol commands.
*/
class MRemConAbsVolTargetObserver
	{
public:
	/** 
	A 'get absolute volume' command has been received.
	*/
	virtual void MrcavtoGetAbsoluteVolume();

	/**
	A 'set absolute volume' command has been received.
	@param aAbsVol The volume on a scale of 0 to aMaxVol.
	@param aMaxVol The volume aAbsVol is relative to.
	*/
	virtual void MrcavtoSetAbsoluteVolume(TUint aAbsVol, TUint aMaxVol);
	};

#endif // REMCONABSVOLTARGETOBSERVER_H
