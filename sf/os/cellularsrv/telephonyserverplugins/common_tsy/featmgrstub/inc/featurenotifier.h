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
* This file contains Feature Manager related definitions for use by the test Feature Manager stub.
*
*/




#ifndef FEATURENOTIFIER_H
#define FEATURENOTIFIER_H

//  INCLUDES
#include <e32svr.h>
#include <babitflags.h>
#include <featmgr/featurecmn.h>

// FORWARD DECLARATIONS

// DEFINES

// CLASS DECLARATION

// CONSTANTS

/**
 Class provides a callback interface for handling notifification of
 changes in features. The client derives a class from this interface
 and implements the HandleNotifyChange-methods that interest it.

 Feature Notifier API consists of the classes CFeatureNotifier and 
 MFeatureObserver. TFeatureEntry and TFeatureChangeType are defined
 in featurecmn.h.
*/
class MFeatureObserver
    {
    public:
        /** 
         This callback method is used to notify the client about
         the change in a feature.
        
         @param aType Type of the change.
         @param aFeature The changed feature.
        
         @see TFeatureChangeType
         @see TFeatureEntry
        */
        virtual void HandleNotifyChange( TFeatureChangeType aType, TFeatureEntry aFeature ) = 0;
        
        /** 
         This callback method is used to notify the client about errors
         in the CFeatureNotifier. Any error in the notifier causes the notifier 
         to stop handling of notifications. Handling can be restarted with
         a call to aNotifier->NotifyRequest(), if the error is non-fatal.
        
         @param aError One of the Symbian OS error codes.
        */
        virtual void HandleNotifyError( TInt aError ) = 0; 
    };

/**
 Active object for obtaining notification of changes in features.
 Feature Notifier automatically resubscribes to the notification and 
 fetches the status and data of the changed feature.

 Feature Notifier API consists of the classes CFeatureNotifier and 
 MFeatureObserver. The array RFeatureUidArray is defined in featurecmn.h.
 The user of this class needs to implement MFeatureObserver interface 
 methods to receive notifications. 

*/
NONSHARABLE_CLASS(CFeatureNotifier) : public CActive
	{
	public:
	    /**
         This is a two-phase constructor method that is used to create a new
         instance for listening to the changes in features. 
        
         @param aObserver A reference to an observer instance.
         @return A pointer to a new instance of the CFeatureNotifier class.
        
		 @leave One of the Symbian OS error codes
        */
        IMPORT_C static CFeatureNotifier* NewL( MFeatureObserver& aObserver );
        
        /**
         Destructor.
        */
        IMPORT_C ~CFeatureNotifier();
  
        /** 
         This method is used to request notification for one feature.
        
         @param aFeature Feature UID.
		 @return KErrAlreadyExists if notification has been already requested
		         Otherwise one of the Symbian OS error codes.
        */
		IMPORT_C TInt NotifyRequest( TUid aFeature );

        /** 
         This method is used to request notification for a subset of features.
        
         @param aFeatures A reference to a client owned UID-array
                          of requested features.
		 @return KErrAlreadyExists if notification has been already requested
		         Otherwise one of the Symbian OS error codes.
		
		 @see RFeatureUidArray
        */
		IMPORT_C TInt NotifyRequest( RFeatureUidArray& aFeatures );

        /** 
         Cancels notification request for one feature.
        
         @param aFeatures Feature UID.
		 @return KErrNotFound if the feature does not exist
		         in the list of previously requested features.
	             Otherwise one of the Symbian error codes. 
        */
        IMPORT_C TInt NotifyCancel( TUid aFeature );
        
        /** 
         Cancels notification requests for all features.
        
		 @return One of the Symbian OS error codes.
        */
		IMPORT_C TInt NotifyCancelAll();

    private:
	    /**
	     C++ default constructor.
	    */
	    CFeatureNotifier();
	    
	    /**
 	     @param aObserver A reference to an observer instance.
	    */
        CFeatureNotifier( MFeatureObserver& aObserver );
        
        /**
	     By default Symbian OS constructor is private.
	    */
	    void ConstructL();

    protected:
        /**
         Implements CActive.
        */    
        void RunL();
        
        /**
         Implements CActive.
         @param aError The error returned.
         @return One of the Symbian OS error codes.
        */
        TInt RunError( TInt aError );
        
        /**
         Implements CActive.
        */  
        void DoCancel();
 
    private:
        /**
         A reference to the callback/observer instance.
        */
        MFeatureObserver& iObserver;
        
        /** Holds UID-array of features being requested. */
        RFeatureUidArray iFeatures; 
        
        /** Server sets changed feature before signaling. */
        TUid iFeatureChanged;
	};


#endif      // FEATURENOTIFIER_H
            
// End of File
