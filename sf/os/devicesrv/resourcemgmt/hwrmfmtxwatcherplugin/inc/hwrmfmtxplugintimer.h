/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Plugin timer definition.
*
*/


#ifndef HWRMFMTXPLUGINTIMER_H
#define HWRMFMTXPLUGINTIMER_H

#include <hwrmpluginservice.h>

/**
* An interface for handling timer expiration events.
*
* @since S60 3.2
* @lib fmtxwatcherplugin.lib
*/
class MHWRMFmtxPluginTimerCallback
    {
public:

    /** 
    * Called when the timer fires
    */
    virtual void GenericTimerFired( TInt aCommandId,
                                    TUint8 aTransId,
                                    TInt aRetVal) = 0;
    };


/**
* Timer implementation class.
*
* @since S60 3.2
* @lib fmtxwatcherplugin.lib
*/
class CHWRMFmtxPluginTimer : public CTimer
    {
public:

    /**
    * This is a two-phase constructor method that is used to create a new
    * instance plugin timer.
    *
    * @param aCommandId Command ID
    * @param aTransId   Transaction ID
    * @param aRetVal    Return value for response
    * @param aCallback  Reference to plugin callback handler
    *
	* @leave One of the Symbian OS error codes.
    */
    static CHWRMFmtxPluginTimer* NewL( TInt aCommandId, TUint8 aTransId, TInt aRetVal,
                               MHWRMFmtxPluginTimerCallback& aCallback );
                     
    /**
    * Destructor.
    */
    ~CHWRMFmtxPluginTimer();

    /**
    * Returns transaction ID.
    */
    TUint8 TransId() const;

protected:
    
    // from CActive  
    void RunL();
    
private:

    /**
    * C++ default constructor.
    * Declaration only, no implementation.
    */
    CHWRMFmtxPluginTimer();
    
    /**
    * Constructor.
    *
    *
    * @param aCommandId Command ID
    * @param aTransId   Transaction ID
    * @param aRetVal    Return value for response
    * @param aCallback  Reference to plugin callback handler
    */
    CHWRMFmtxPluginTimer( TInt aCommandId, TUint8 aTransId, 
                  TInt aRetVal, MHWRMFmtxPluginTimerCallback& aCallback );
    
    /**
    * By default Symbian OS constructor is private.
    */
    void ConstructL();
    
private:

    /**
    * Command ID.
    */
    TInt iCommandId;

    /**
    * Transaction ID.
    */
    TUint8 iTransId;

    /**
    * Return value for response.
    */
    TInt iRetVal;
   
    /**
    * Callback reference to be used with responses to commands.
    */
    MHWRMFmtxPluginTimerCallback& iCallback;
    };


#endif // __PLUGINTIMER_H_