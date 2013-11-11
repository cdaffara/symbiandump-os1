// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: Class definition of CLbtSystemStateListener
//


#ifndef C_LBSSYSTEMSTATELISTENER_H
#define C_LBSSYSTEMSTATELISTENER_H

#include <e32base.h>

#ifndef SYMBIAN_SYSTEM_STATE_MANAGEMENT
    #include <e32property.h>       
#else
    #include <ssm/ssmstateawaresession.h>
#endif

/**
 * Observer class
 */
class MLbsStartupObserver
    {
    public:
        /**
         * Notifies the observer if the system state
         * has changed so that non critical operation
         * like LBT logic server loading can be done
         *
         */
        virtual void HandleStartupCompletionL() = 0;
    };

/**
 * Class definition for LBS System state listner
 * @since S60 v4.0
 */
class CLbsSystemStateListener : public CActive
	{
	
	public:	
		/**
	     * Instantiates a new object of 
	     * CLbtSystemStateListener
	     *
	     * @return a pointer to an instance of
	     * CLbtSystemStateListener
	     */
	    static CLbsSystemStateListener* NewL(MLbsStartupObserver& aObserver);
	    
	    /**
	     * Starts listening to system state
	     * events
	     *
	     */
	    void StartToListenL();
	    
	    /**
	     * Destructor
	     *
	     */
	    ~CLbsSystemStateListener();
	    	    
	private:
		
		/**
		 * Enum to determine which event's 
		 * notification is outstanding
		 */
		enum 
			{
			ENoneOutstanding, // no notification outstanding
			ESSEventOutstanding, // system state event outstanding
			ETimerEventOutstanding // timer event outstanding
			};
	
		/**
	     * 2nd phase constuctor for instantiating member variables
	     *
	     */
		void ConstructL();
		
		/**
	     * Default C++ Constructor
	     *
	     */
	    CLbsSystemStateListener(MLbsStartupObserver& aObserver);
	    
	private: // From CActive
	
		/**
	     * Request completion event handler
	     *
	     */	
	    void RunL();
	    
	    /**
	     * Invoked by the active object FW when 
	     * any outstanding request is cancelled
	     *
	     */	
	    void DoCancel();

	    /**
	     * Handles errors if RunL leaves
	     * This override is necessary because the base implementation
	     * panics the thread with E32User-CBase 47.
	     *
	     */	
	    TInt RunError(TInt aError);
	    
	private: // Data Members
	
		
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	    /**
	     * 
	     */
	    RSsmStateAwareSession iStateAwareSession;
		/**
		* Used to indicate if the System State Manager is available
		*/
		TBool iSystemStateManagerAvailable;
#else
	    /**
         * Handle to the startup property object
         *
         */ 
        RProperty       iStartupObserver;
        
#endif
	    
	    /**
	     * Pointer to the system state observer
	     *
	     */
	    MLbsStartupObserver&	iObserver;
	};

#endif // C_LBSSYSTEMSTATELISETER_H

