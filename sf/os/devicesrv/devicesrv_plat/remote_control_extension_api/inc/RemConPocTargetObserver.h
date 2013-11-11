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
* Description:  Informs client that PoC button has been pressed in accessory.
*
*/


#ifndef MREMCONPOCTARGETOBSERVER_H
#define MREMCONPOCTARGETOBSERVER_H

#include <e32base.h>

class MRemConPocTargetObserver
	{
public:
	
	/**
	* POC button has been pressed down.
	*/
	virtual void APOCButtonDown() = 0;
	
	/**
	* POC button has been released up.
	*/
	virtual void APOCButtonUp() = 0;
		
	};

#endif // MREMCONPOCTARGETOBSERVER_H
