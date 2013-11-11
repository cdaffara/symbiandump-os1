/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Informs client that synchronization button has been pressed in 
*                accessory.
*
*/


#ifndef MREMCONSYNCHRONIZATIONTARGETOBSERVER_H
#define MREMCONSYNCHRONIZATIONTARGETOBSERVER_H

#include <e32base.h>

class MRemConSynchronizationTargetObserver
	{
public:
	
	/**
	* Synchronize button has been pressed.
	*/
	virtual void Synchronize() = 0;
			
	};

#endif // MREMCONPOCTARGETOBSERVER_H
