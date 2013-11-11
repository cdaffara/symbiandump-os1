// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef REMCONSIDEKEYTARGETOBSERVER_H
#define REMCONSIDEKEYTARGETOBSERVER_H

#include <e32base.h>
#include <remcon/remconsidekeyapi.h>

/**
Client-implemented mixin- notifies the client of incoming SideKey commands.
*/
class MRemConSideKeyTargetObserver
	{
public:
	/** 
	A 'side key up' command has been received.
	*/
	virtual void MrcsktoSideKeyUp(TRemConSideKeyApiButtonAction aAction) = 0;

	/** 
	A 'side key down' command has been received.
	*/
	virtual void MrcsktoSideKeyDown(TRemConSideKeyApiButtonAction aAction) = 0;
	};

#endif // REMCONSIDEKEYTARGETOBSERVER_H
