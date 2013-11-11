/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//
// PropWatcher.h
//
/** 
 @file
 @internalTechnology
 @test
*/

#ifndef __LBS_GETAFIX_PROPERTY_WATCHER_H__
#define __LBS_GETAFIX_PROPERTY_WATCHER_H__

#include <e32base.h>
#include <e32property.h>

class MPropWatcherObserver
	{
public:
	virtual void OnChange(RProperty& aProperty) = 0;
	};
	
class CPropWatcher : public CActive
	{
public:
    static CPropWatcher* NewL(TUint aCategory, TUint32 aKey, MPropWatcherObserver& aObserver);
    ~CPropWatcher();
    
private:
    CPropWatcher  (MPropWatcherObserver& aObserver);
    void ConstructL(TUint aCategory, TUint32 aKey);
    
    void RunL();
    void DoCancel();

private:
	RProperty		      iProperty;
	MPropWatcherObserver& iObserver;
    };

#endif // __LBS_GETAFIX_PROPERTY_WATCH_H__
