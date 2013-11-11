/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Save as expressly licensed to you by Symbian Software Ltd, all rights reserved.
* 
*
*/



#ifndef MBUFFERCONTROL_H
#define MBUFFERCONTROL_H

#include <a3f/a3fbase.h>

/*
A CBufferFlushHandler's client must implement this interface 
to be notified when when the flush process has finished
*/ 
class MFlushHandlerObserver
	{
public:
	virtual void FlushComplete(TInt aError) = 0;
	};

/**
 This class allow interaction with the physical layer allowing flush physical buffers
 */
class MStreamBufferControl
	{
public:
	/**
	@return KErrNone error if flush operation is successful
	*/
	virtual TInt FlushBuffers() = 0;
	};

#endif // MBUFFERCONTROL_H
