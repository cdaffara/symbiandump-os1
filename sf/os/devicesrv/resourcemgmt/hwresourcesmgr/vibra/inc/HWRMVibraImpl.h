/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef HWRMVIBRAIMPL_H
#define HWRMVIBRAIMPL_H

// INCLUDES
#include "HWRMClient.h"
#include "HWRMVibra.h"
#include "HWRMVibraProfileObserver.h"
#include "hwrmuipluginservice.h"


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CHWRMVibraStatusObserver;
class CHwrmUiPluginHandler;
// CLASS DECLARATION

/**
*  Class of the vibra client side session.
*
*/
NONSHARABLE_CLASS(CHWRMVibraImpl) : public CHWRMVibra,
                                    public MHWRMVibraProfileObserver,
					                public MHwrmForegroundObserver
    {
    public:  // Constructors and destructor
        
        // Implementations are always constructed
        // by CHWRMVibra::NewL or CHWRMVibra::NewLC.

        /**
        * Destructor.
        */
        virtual ~CHWRMVibraImpl();

    public: // New functions

    public: // Functions from base classes
       
        // From CHWRMVibra
		void SetFeedbackObserver(MHWRMVibraFeedbackObserver* aCallback);
        void ReserveVibraL();
        void ReserveVibraL(TBool aRestoreState, TBool aForceNoCCoeEnv);
    	void ReleaseVibra();
    	void StartVibraL(TInt aDuration);
        void StartVibraL(TInt aDuration, TInt aIntensity);
	    void StopVibraL(void);
	   	void PulseVibraL();
	    void PulseVibraL(TInt aDuration);
	    void PulseVibraL(TInt aDuration, TInt aIntensity);
        CHWRMVibra::TVibraModeState VibraSettings(void) const;
        TVibraStatus VibraStatus() const;
        CHWRMVibra::TVibraFeedbackModeState VibraFeedbackSettings() const;
        
        // From MHWRMVibraProfileObserver
        void VibraModeChange(CHWRMVibra::TVibraModeState aNewState);
        void VibraFeedbackModeChange(CHWRMVibra::TVibraFeedbackModeState newState);
        
        // From MHWRMForegroundObserver
		void HandleGainingForeground();
		void HandleLosingForeground();


    protected:  // New functions
        
    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CHWRMVibraImpl();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(MHWRMVibraObserver* aCallback);

    	/**
    	* Opens the vibra session.
    	* This method must be called before any other methods can be called.
    	*/
    	void OpenL();

    	/**
    	* Closes the session. 
    	*/
    	void Close();

        /**
        * Do actual reservation. For parameters, see ReserveVibraL
        */        
        void DoReserveVibraL(TBool aRestoreState, TBool aForceNoCCoeEnv);
        
        /**
        * Do actual release
        */
    	void DoReleaseVibra();

    public:     // Data
    
    protected:  // Data

    private:    // Data

        RHWRMClient                	iClient;              	// HWRM server client
        MHWRMVibraObserver*        	iCallback;            	// Client listening events. Not owned
        MHWRMVibraFeedbackObserver* iFeedbackCallback;   	// Client listening feedback events. Not owned
        mutable TVibraModeState		iVibraSetting;        	// Current Profile Vibra Setting, cached for speed
        mutable TVibraFeedbackModeState    	iVibraFeedbackSetting;	// Current Profile feedback vibration setting, cached for speed
        CHWRMVibraStatusObserver*  	iStatusObserver;      	// Vibra status observer pointer. Owned.
        CHWRMVibraProfileObserver* 	iProfileObserver;     	// Vibra profile observer pointer. Owned.
        CHwrmUiPluginHandler*    	iUiPluginHandler; 		//
        
        TBool                      	iReserved;            	// Flag to indicate if vibra is reserved currently.
        TBool                      	iAutoReserving;       	// Flag to indicate if vibra is being autoreserved
                                                         	// and autoreleased on background/foreground transitions.
                                                         
        TBool                      	iPrivilegedClient;   	// Flag to indicate if this client can ignore profile settings                                                      

    public:     // Friend classes

        friend class CHWRMVibra;

    protected:  // Friend classes

    private:    // Friend classes

    };
    
#endif      // HWRMVIBRAIMPL_H   
            
// End of File
