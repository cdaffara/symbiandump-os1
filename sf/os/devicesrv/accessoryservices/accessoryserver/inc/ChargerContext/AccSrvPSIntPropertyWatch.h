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
* Description:  Watcher class for PS' integer property value changes.
*
*/



#ifndef C_ACCSRVPSINTPROPERTYWATCH_H
#define C_ACCSRVPSINTPROPERTYWATCH_H


#include <e32property.h>


#include "AccSrvPSIntPropertyObserver.h"


/**
 *  Watcher class for PS' integer property value changes.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib AccServer.exe
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CAccSrvPSIntPropertyWatch ): public CActive
    {

    enum TPSIntPropertyWatchPriotity {EPriority=0};
    
public:


    /**
    * Two-phased constructor.
    * @param aObserver Observer's callback interface.
    * @param aCategory The UID that identifies the property category.
    * @param aKey The property sub-key, i.e. the key that identifies the specific property within the category. 
    * @param aType The ownership of this property handle. By default, ownership is vested in the current process, 
    * but can be vested in the current thread by specifying EOwnerThread.
    */
    static CAccSrvPSIntPropertyWatch* NewL(
            MAccSrvPSIntPropertyObserver*   aObserver,
            TUid    aCategory,
            TInt    aKey,
            TOwnerType aType = EOwnerProcess );
    
    
    /**
    * Two-phased constructor.
    * @param aObserver Observer's callback interface.
    * @param aCategory The UID that identifies the property category.
    * @param aKey The property sub-key, i.e. the key that identifies the specific property within the category. 
    * @param aType The ownership of this property handle. By default, ownership is vested in the current process, 
    * but can be vested in the current thread by specifying EOwnerThread.
    */
    static CAccSrvPSIntPropertyWatch* NewLC(
            MAccSrvPSIntPropertyObserver*   aObserver,
            TUid    aCategory,
            TInt    aKey,
            TOwnerType aType = EOwnerProcess );
    

    /**
    * Destructor.
    */
    virtual ~CAccSrvPSIntPropertyWatch();

    /**
    * Start waching.
    *
    * @since S60 5.1
    * @param None
    * @return void
    */
    void Watch();

    
    /**
    * Get current value of integer property.
    * @param aValue Reference to a variable to which current value is to be assigned.
    * @return TInt Error code. 
    *           KErrNone, if successful;
    *           KErrPermissionDenied, if the caller process doesn't pass the Read Policy; 
    *           KErrNotFound, if the property has not been defined; 
    *           KErrArgument, if the property is not of integral type.
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
    CAccSrvPSIntPropertyWatch();

    /**
    * By default Symbian OS constructor is private.
    * @param aObserver Observer's callback interface.
    * @param aCategory The UID that identifies the property category.
    * @param aKey The property sub-key, i.e. the key that identifies the specific property within the category. 
    * @param aType The ownership of this property handle. By default, ownership is vested in the current process, 
    * but can be vested in the current thread by specifying EOwnerThread.
    */
    void ConstructL(
            MAccSrvPSIntPropertyObserver*   aObserver,
            TUid    aCategory,
            TInt    aKey,
            TOwnerType aType = EOwnerProcess );

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
    CAccSrvPSIntPropertyWatch( const CAccSrvPSIntPropertyWatch& );

   /**
   * By default, prohibit assignment operator
   */
    CAccSrvPSIntPropertyWatch& operator= ( const CAccSrvPSIntPropertyWatch& );


private: // data

    /**
    * Observer objects callback interface.
    * Not own.
    */
    MAccSrvPSIntPropertyObserver*   iObserver;
    

    /**
    * Handle of the integer property.
    */
    RProperty                   iProperty;

    /**
    * The UID that identifies the property category.
    */
    TUid                       iCategory;

    /**
    * The property sub-key, i.e. the key that identifies the specific property within the category.
    */
    TInt                        iKey;


    };


#endif // C_ACCSRVPSINTPROPERTYWATCH_H
