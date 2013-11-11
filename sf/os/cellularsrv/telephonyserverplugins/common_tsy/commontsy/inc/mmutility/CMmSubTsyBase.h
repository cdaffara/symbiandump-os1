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



#ifndef _CMMSUBTSYBASE_H
#define _CMMSUBTSYBASE_H

//  INCLUDES
#include <e32base.h>
#include <et_tsy.h>
#include <et_phone.h>
#include "MmTsy_timeoutdefs.h"
 
/** CLASS DECLARATION */

/**
 *  CMmSubTsyBase is a base class for TSY's subsystems. 
 */
class CMmSubTsyBase : public CBase
    {
    public: // Constructors and destructor

        /**
         * Destructor.
         */
        virtual ~CMmSubTsyBase() 
            { 
            return; 
            };
            
        /**
         * Initialisation method
         */
        virtual void Init() = 0;
    
        /**
         * Tells whether the object supports given IPC.
         *         
         * 
         * @param anIpc an interprocess communication command identifier 
         * return TBool:  ETrue if object support given IPC
         */
        virtual TBool SupportingIPC( const TInt anIpc ) = 0;

        /**
         * Handles extended client requests. Uses TRAP to ensure functioning on 
         * memory allocation failure.
         *         
         * 
         * @param aTsyReqHandle Request handle
         * @param anIpc an interprocess communication command identifier 
         * @param aPackage reference to a data package
         * @return KErrNone if request accepted, else several possibilities
         *         depending on IPC
         */
        virtual TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle,
                                 const TInt anIpc,
                                 const TDataPackage& aPackage) = 0;

        /**
         * Returns request mode for given IPC number.
         *         
         * 
         * @param anIpc an interprocess communication command identifier
         * @return The ReqMode corresponding to given IPC
         */
        virtual CTelObject::TReqMode ReqModeL( const TInt anIpc ) = 0;
    
        /**
         * Returns number of slots to be used for given IPC.
         *         
         * 
         * @param anIpc an interprocess communication command identifier
         * @return the number of slots reserved for a given IPC
         */
        virtual TInt NumberOfSlotsL( const TInt anIpc ) = 0;
    
        /**
         * Cancels request of which IPC number and request handle are given in 
         * parameters.
         *         
         * 
         * @param anIpc an interprocess communication command identifier
         * @param aTsyReqHandle Request handle
         * @return KErrNone, KErrGeneral if unknown IPC
         */
        virtual TInt CancelService(   const TInt anIpc,
                                      const TTsyReqHandle aTsyReqHandle ) = 0;

        /**
         * Register given notification.
         *         
         * 
         * @param anIpc an interprocess communication command identifier
         * @return KErrNotSupported
         */
        virtual TInt RegisterNotification( const TInt anIpc ) = 0;
    
        /**
         * Deregister given notification.
         *         
         * 
         * @param anIpc an interprocess communication command identifier
         * @return KErrNotSupported
         */
        virtual TInt DeregisterNotification( const TInt anIpc ) = 0;

#ifdef REQHANDLE_TIMER

        /**
         * Calls the needed complete method due timer expiration
         *         
         * 
         * @param aReqHandleType req handle type.
         * @param TInt aError error value
         */
        virtual void Complete( TInt, //aReqHandleType
                               TInt ) //aError
            { 
            return;
            }
        
        /**
         * Resets an entry in the req handle store 
         * 
         * 
         * @param aTsyReqHandle req handle 
         * @param aIpc IPC value
         * return EFalse if req handle not found
         */        
        virtual TBool ResetReqHandle(const TTsyReqHandle /*aTsyReqHandle*/, const TInt /*aIpc*/)
            {
            return EFalse;
            }
        
#endif //REHANDLE_TIMER

    protected:
    
        /**
         * C++ default constructor.
         */    
        CMmSubTsyBase() 
        { 
            return; 
        };

    };

#endif // CMMSUBTSYBASE_H

//  End of File
