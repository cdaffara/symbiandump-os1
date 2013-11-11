/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef _CMMTSYBASE_H
#define _CMMTSYBASE_H

//  INCLUDES
#include "CMmSubTsyBase.h"
#include "MmTsy_timeoutdefs.h"

// CLASS DECLARATION

/**
 *  CMmTsyBase contains functionality common to all TSY objects.
 *  It also provides default implementations to some of the virtual 
 *  functions.
 */
NONSHARABLE_CLASS( CMmTsyBase ) : public CSubSessionExtBase
    {
    public: // Constructors and destructor

        /**
         * Constructor
         */
        CMmTsyBase();
    
        /**
         * Destructor
         */
        virtual ~CMmTsyBase();
        
        // New functions

        /**
         * Initialisation method
         */
        virtual void Init();

        /**
         * New instances are created by given name. By default: not supported  
         * and should never be called. Leaves.
         *          
         * 
         * @param  aName a name
         * @return CTelObject 
         */
        virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName); 
    
        /**
         * New instances are created by given name. By default: not supported  
         * and should never be called. Leaves.
         *          
         * 
         * @param aNewName a name
         * @return CTelObject
         */
        virtual CTelObject* OpenNewObjectL(TDes& aNewName); 
    
        /**
         * Returns request mode for given IPC number.
         *          
         * 
         * @param anIpc an interprocess communication command identifier
         * @return TInt the ReqMode corresponding to given IPC
         */
        virtual CTelObject::TReqMode ReqModeL( const TInt anIpc );
    
        /**
         * Returns number of slots to be used for given IPC.
         *          
         * 
         * @param  anIpc an interprocess communication command identifier
         * @return TInt the number of slots reserved for a given IPC
         */
        virtual TInt NumberOfSlotsL( const TInt anIpc );

        /**
         * Cancels request of which IPC number and request handle are given in 
         * parameters.
         *          
         * 
         * @param anIpc an interprocess communication command identifier
         * @param aTsyReqHandle Request handle
         * @return TInt KErrGeneral if unknown IPC
         */
        virtual TInt CancelService(const TInt anIpc,
                                   const TTsyReqHandle aTsyReqHandle );
    
        /**
         * Register given notification.
         *          
         * 
         * @param anIpc: an interprocess communication command identifier
         * @return TInt KErrNotSupported
         */
        virtual TInt RegisterNotification( const TInt anIpc );
    
        /**
         * Deregister given notification.
         *          
         * 
         * @param anIpc: an interprocess communication command identifier
         * @return TInt KErrNotSupported
         */
        virtual TInt DeregisterNotification( const TInt anIpc );
    
        /**
         * Gives pointer to the array of TSY's subsystems.
         *          
         * 
         * @return CMmSubTsyBase** Array of pointers to CMmSubTsyBase classes
         */
        virtual CMmSubTsyBase** GetSubsystemArrayPtr() = 0;
    
        /**
         * Gives the maximun number of TSY's subsystems.
         * 
         * @return TInt number of subsystems
         */
        virtual TInt GetMaxNumberOfSubsystems() = 0;

#ifdef REQHANDLE_TIMER

        /**
         * Calls the needed complete method due timer expiration
         *          
         * 
         * @param aReqHandleType req handle type.
         * @param aError error value
         * @param aIPC IPC value
         */
        virtual void Complete( TInt aReqHandleType, TInt aError, TInt aIPC);

#endif //REQHANDLE_TIMER 

        /**
         * Handles extended client requests. Uses TRAP to ensure functioning  
         * on memory allocation failure.
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aPackage reference to a data package
         * @return KErrNone if request accepted, else several possibilities
         *         depending on IPC
         */
        virtual TInt DoBaseExtFuncL(const TTsyReqHandle aTsyReqHandle,
                                    const TInt anIpc,
                                    const TDataPackage& aPackage);

        /**
         * Resets an entry in a handle request store for a given request handle
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aIpc IPC value
         * @return EFalse if req handle not found 
         */        
        virtual TBool ResetReqHandle(const TTsyReqHandle aTsyReqHandle, const TInt aIpc);
        
    };

#endif // CMMTSYBASE_H

//  End of file
