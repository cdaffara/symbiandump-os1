/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef MOUTPUTPORT_H
#define MOUTPUTPORT_H

#include <a3f/a3fbase.h>

class CMMFBuffer;
class MInputPort;
class MFlushHandlerObserver;
/**
 */
class MOutputPort
	{
public:

	virtual TInt GetOutputPort(MOutputPort*& aInputPort) = 0;
	/**
	* @param
	*/
	virtual TInt FillBuffer(CMMFBuffer* aBuffer, MInputPort* aConsumer)=0;
	
	/**
	* @param 
	*/
	virtual TInt BufferEmptied(CMMFBuffer* aBuffer)=0;

	/**
	*/
	virtual TInt SetInput(MInputPort* aInput)=0;

	/**
	* @param 
	*/
	virtual TInt RemoveInput(MInputPort* aInput)=0;

	/**
	* @param 
	*/
	virtual TInt FlushBuffer(MFlushHandlerObserver* aFlushObserver)=0;
	};

#endif // MOUTPUTPORT_H
