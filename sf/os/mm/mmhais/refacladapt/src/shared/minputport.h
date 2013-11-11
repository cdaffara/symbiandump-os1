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



#ifndef MINPUTPORT_H
#define MINPUTPORT_H

#include <a3f/a3fbase.h>

class CMMFBuffer;
class MOutputPort;

/**
 */
class MInputPort
	{
	public:
	/**
	* @param
	*/
	virtual TInt GetInputPort(MInputPort*& aInputPort) = 0;
	/**
	* @param
	*/
	virtual TInt EmptyBuffer(CMMFBuffer* aBuffer, MOutputPort* aSupplier)=0;

	/**
	* @param 
	*/
	virtual TInt BufferFilled(CMMFBuffer* aBuffer)=0;

	/**
	*/
	virtual TInt SetOutput(MOutputPort* aOutput)=0;

	/**
	* @param 
	*/
	virtual TInt RemoveOutput(MOutputPort* aOutput)=0;

	};

#endif // MINPUTPORT_H
