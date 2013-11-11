/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef HWRMFMTXCONNOBSERVER_H
#define HWRMFMTXCONNOBSERVER_H

/**
* Connection types. Not a bitmask, but types can be assigned
* values as 0, 1, 2, 3, ...
*/  
enum TFmtxWatcherObserverTypes
    {
 // EFmtxWatcherObserverAccessory,
 // EFmtxWatcherObserverUsb,
    EFmtxWatcherObserverTest, // Test observer should be always the last
    EFmtxWatcherObserverCount
    };


/**
* A callback interface for handling notifification of changes 
* in wired connections, which ought to disable FM Tx. The client 
* derives a class from this interface and implements the 
* HandleConnectionChange-method.
*
*/
class MHWRMFmtxConnObserverCallback
    {
public:

    /**
    * This callback method is used to notify the client about
    * the change in connection status.
    *
    * @param aType Connection type.
    * @param aConnected Truth value indicating connection status.
    */
    virtual void HandleConnectionChange( TFmtxWatcherObserverTypes aType,
                                         TBool aConnected ) = 0;
    };

/**
*  Interface class to be implemented by connection observers.
*
*/
class MHWRMFmtxConnObserver
    {
public:

    /**
    * Destructor.
    *
    * Needed for destroying objects so that there's no need to know
    * the actual implementation class(es).
    */
    virtual ~MHWRMFmtxConnObserver() {};

    /**
    * This method gets connection status.
    *
    * @return TBool ETrue if connected, otherwise EFalse.
    *
	* @leave One of the Symbian OS error codes.
    */
    virtual TBool GetStatusL() = 0;

    /**
    * This method is used to start observing connections.
    *
	* @leave One of the Symbian OS error codes.
    */
    virtual void StartObservingL() = 0;

    /** 
    * This method is used to stop observing connections.
    *
    */
    virtual void StopObserving() = 0;
    };

#endif // HWRMFMTXCONNOBSERVER_H
