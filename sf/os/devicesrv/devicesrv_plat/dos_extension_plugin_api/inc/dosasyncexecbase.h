/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*    This is the definition for CDosAsyncExecBase class.
*
*/



#ifndef __DOSASYNCEXECBASE_H__
#define __DOSASYNCEXECBASE_H__

#include <e32base.h>

/**
* Helper class to help implementing the DSY for asynchronous operations.
*/
class CDosAsyncExecBase : public CActive
{
public:
	/**
	* Constructor.
	* @param aMessage The client-server message that has to be completed.
	*/
	IMPORT_C CDosAsyncExecBase(const RMessagePtr2 aMessage);

	/**
	* Destructor
	*/
	IMPORT_C ~CDosAsyncExecBase();

private:
	/**
	* It's meant to be overriden and it's called just before the message has
	* been completed.
	* @param aComplete If put to ETrue the message will be completed otherwise
	* it won't.
	*/
	IMPORT_C virtual void BeforeComplete(TBool& aComplete);

	/**
	* It's meant to be overriden and it's called just before the message has 
	* been completed.
	* @param aAutoDestroy If it's set to ETrue the object will destroy itself
	* otherwise it won't.
	*/
	IMPORT_C virtual void AfterComplete(TBool& aAutoDestroy);

private:
	/**
	* Inherited from CActive. It calls BeforeComplete, completes the request if 
	* instructed to do so and then calls AfterComplete.
	*/
	IMPORT_C void RunL();

	/**
	* Inherited from CActive. It cancels the outstanding request.
	*/
	IMPORT_C virtual void DoCancel();

private:
	const RMessagePtr2 iMessage;
};


#endif //__DOSASYNCEXECBASE_H__
