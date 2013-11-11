/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class to watch key value changes in Central Repository.
*
*/



#ifndef C_ACCSRVCENTRALREPOSITORYWATCH_H
#define C_ACCSRVCENTRALREPOSITORYWATCH_H


#include    <e32base.h>
#include    <centralrepository.h>

#include "AccSrvCentralRepositoryObserver.h"


/**
 *  Class to watch key value changes in Central Repository.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib AccServer.exe
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CAccSrvCentralRepositoryWatch ): public CActive
    {


public:


    /**
    * Two-phased constructor.
    * @param aObserver Observer's callback interface.
    * @param aRepositoryId Identifier of a repositor.
    * @param aKeyId Identifier of a key.
    */
    static CAccSrvCentralRepositoryWatch* NewL(
            MAccSrvCentralRepositoryObserver*   aObserver,
            TUid    aRepositoryId,
            TUint32 aKeyId
            );
    
    /**
    * Two-phased constructor.
    * @param aObserver Observer's callback interface.
    * @param aRepositoryId Identifier of a repositor.
    * @param aKeyId Identifier of a key.
    */
    static CAccSrvCentralRepositoryWatch* NewLC(
            MAccSrvCentralRepositoryObserver*   aObserver,
            TUid    aRepositoryId,
            TUint32 aKeyId
            );

    /**
    * Destructor.
    */
    virtual ~CAccSrvCentralRepositoryWatch();

    /**
    * Watch value changes of a specified key in a specified CenRep.
    * @param None
    * @return void
    */
    void Watch();

    /**
    * Get current value of a specified key in a specified CenRep.
    * @param aValue Reference to a variable to which current value is to be assigned.
    * @return TInt Error code. 
    *       KErrNone if successful, 
    *       KErrAbort if in a transaction that has previously failed 
    *       KErrPermissionDenied if caller fails capability check, 
    *       KErrNotFound if the setting does not exist, 
    *       KErrArgument if the setting exists but is not an integer, plus other system-wide error codes.
    */
    TInt GetCurrentValue( TInt& aValue );


protected:

    /**
    * Active object cancellation
    * @param None
    * @return void
    */
    void DoCancel();

private:


    /**
    * C++ default constructor.
    */
    CAccSrvCentralRepositoryWatch();

    /**
    * By default Symbian OS constructor is private.
    * @param aObserver Observer's callback interface.
    * @param aRepositoryId Identifier of a repositor.
    * @param aKeyId Identifier of a key.
    * @return void
    */
    void ConstructL(
            MAccSrvCentralRepositoryObserver*   aObserver,
            TUid    aRepositoryId,
            TUint32 aKeyId );

    /**
    * Completion
    * @param None
    * @return void
    */
    void RunL();
   
    /**
    * Error handling.
    * @param aError Error code.
    * @return void
    */
    TInt RunError( TInt aError );
   
    /**
    * By default, prohibit copy constructor
    */
   CAccSrvCentralRepositoryWatch( const CAccSrvCentralRepositoryWatch& );

   /**
   * By default, prohibit assignment operator
   */
    CAccSrvCentralRepositoryWatch& operator= ( const CAccSrvCentralRepositoryWatch& );


private: // data

    /**
     * Observer objects callback interface.
     * Not own.
     */
    MAccSrvCentralRepositoryObserver*   iObserver;

    /**
     * Repository pointer.
     * Own.
     */
    CRepository*               iRepository;

    /**
     * Identifier of the repositor.
     */
    TUid                       iRepositoryUid;

    /**
     * Identifier of a key.
     */
    TInt32                     iKey;

 
    };


#endif // C_ACCSRVCENTRALREPOSITORYWATCH_H
