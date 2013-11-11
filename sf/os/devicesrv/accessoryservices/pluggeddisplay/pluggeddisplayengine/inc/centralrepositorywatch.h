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
* Description: Class to watch key value changes in Central Repository.
*
*/



#ifndef C_CENTRALREPOSITORYWATCH_H
#define C_CENTRALREPOSITORYWATCH_H


#include    <e32base.h>
#include    <centralrepository.h>
#include "centralrepositoryobserver.h"


/**
 *  Class to watch key value changes in Central Repository.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib none.
 *  @since TB 9.2
 */
NONSHARABLE_CLASS( CCentralRepositoryWatch ): public CActive
    {


public:


    /**
    * Two-phased constructor.
    * @param aObserver Observer's callback interface.
    * @param aRepositoryUid Identifier of a repositor.
    * @param aKeyId Identifier of a key.
    */
    static CCentralRepositoryWatch* NewL(
            MCentralRepositoryObserver&   aObserver,
            TUid aRepositoryUid,
            TUint32 aKeyId
            );
    
    /**
    * Two-phased constructor.
    * @param aObserver Observer's callback interface.
    * @param aRepositoryUid Identifier of a repositor.
    * @param aKeyId Identifier of a key.
    */
    static CCentralRepositoryWatch* NewLC(
            MCentralRepositoryObserver&   aObserver,
            TUid aRepositoryUid,
            TUint32 aKeyId
            );

    /**
    * Destructor.
    */
    virtual ~CCentralRepositoryWatch();

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
    * @param aObserver Observer's callback interface.
    * @param aRepositoryId Identifier of a repositor.
    * @param aKeyId Identifier of a key.
    */
    CCentralRepositoryWatch(
            MCentralRepositoryObserver& aObserver,
            TUid aRepositoryId,
            TUint32 aKeyId );

    /**
    * By default Symbian OS constructor is private.
    * @return void
    */
    void ConstructL(  );

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
   CCentralRepositoryWatch( const CCentralRepositoryWatch& );

   /**
   * By default, prohibit assignment operator
   */
    CCentralRepositoryWatch& operator= ( const CCentralRepositoryWatch& );


private: // data

    /**
     * Observer objects callback interface.
     * Not own.
     */
    MCentralRepositoryObserver&   iObserver;

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


#endif // C_CENTRALREPOSITORYWATCH_H
