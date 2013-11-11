// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 

#if !defined (__STARTUPAPPINFO_H__)
#define __STARTUPAPPINFO_H__


/** Required information for application starter
@internalComponent
@deprecated
*/
class MStartupAppInfo
	{
public:
	virtual ~MStartupAppInfo() {}

public:
	/** Path to the application
	*/
	virtual TPtrC GetPath() const = 0;
	
	/** Application arguments
	*/
	virtual TPtrC GetArgs() const = 0;
	
	/** Whether system starter should wait for application to signal completion
	or use 'fire and forget' mode.
	*/
	virtual TInt StartMethod() const = 0;
	
	/** Whether to kill a process that's taking too long.
	a value of zero means "do not time this process out".
	*/
	virtual TInt Timeout() const = 0;
	
	/** Whether startup should fail or continue if application doesn't succeed
	*/
	virtual TBool FailOnError() const = 0;
	
	/** How to start application.
	*/
	virtual TInt StartType() const = 0;
	
	/** Whether the application should be started viewless.
	*/
	virtual TBool Viewless() const = 0;
	
	/** Whether the application should be started in the background.
	*/
	virtual TBool StartInBackground() const = 0;
	
	/** No of retries if the server/app/process fails to start
	*/
	virtual TInt NoOfRetries() const = 0;
	};

#endif	// __STARTUPAPPINFO_H__
