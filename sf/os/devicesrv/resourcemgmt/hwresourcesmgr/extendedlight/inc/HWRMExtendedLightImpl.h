/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
 @internalTechnology 
*/

#ifndef HWRMEXTENDEDLIGHTIMPL_H
#define HWRMEXTENDEDLIGHTIMPL_H

#include "HWRMClient.h"
#include "HWRMExtendedLight.h"
#include "hwrmuipluginservice.h"

class CHWRMExtendedLightStatusObserver;
class CHwrmUiPluginHandler;

/**
*  Class of the extended light client side session.
*
*/
NONSHARABLE_CLASS(CHWRMExtendedLightImpl) : public CHWRMExtendedLight,					                        
					                        public MHwrmForegroundObserver
    {
    public:  // Constructors and destructor
        
        // Implementations are always constructed
        // by CHWRMExtendedLight::NewL or CHWRMExtendedLight::NewLC.

        /**
        * Destructor.
        */
        virtual ~CHWRMExtendedLightImpl();

    public: // New functions

    public: // Functions from base classes
       
        // From CHWRMExtendedLight
    	void ReserveLightL(TInt aTarget);
        void ReserveLightL(TInt aTarget, 
                           TBool aRestoreState, 
                           TBool aForceNoCCoeEnv);
    	void ReleaseLight(TInt aTarget);
    	
    	void LightOnL(TInt aTarget);
		void LightOnL(TInt aTarget, 
                      TInt aDuration);
		void LightOnL(TInt aTarget, 
                      TInt aDuration, 
                      TInt aIntensity,
                      TBool aFadeIn);

		void LightBlinkL(TInt aTarget);
		void LightBlinkL(TInt aTarget, 
		                 TInt aDuration);
		void LightBlinkL(TInt aTarget, 
		                 TInt aDuration, 
		                 TInt aOnDuration, 
		                 TInt aOffDuration, 
		                 TInt aIntensity);
		void LightOffL(TInt aTarget);
		void LightOffL(TInt aTarget, 
                       TInt aDuration);
		void LightOffL(TInt aTarget, 
                       TInt aDuration, 
                       TBool aFadeOut);

        CHWRMExtendedLight::TLightStatus LightStatus(TInt aTarget) const;
        TInt SupportedTargets() const;                
		
		// From MHWRMForegroundObserver
		void HandleGainingForeground();
		void HandleLosingForeground();		


    protected:  // New functions
        
    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CHWRMExtendedLightImpl();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(MHWRMExtendedLightObserver* aCallback, TInt aPermittedTargets = -1);

    	/**
    	* Opens the Light session.
    	* This method must be called before any other methods can be called.
    	*/
    	void OpenL(TInt aPermittedTargets = -1);

    	/**
    	* Closes the session. 
    	*/
    	void Close();
    	
    	/**
    	* Checks the validity of common API call parameters.
    	* Also checks the session handle.
    	*
    	* @param aTarget    aTarget parameter of API call
    	* @param aDuration  aDuration parameter of API call
    	* @param aIntensity aIntensity parameter of API call
    	* @leave KErrArgument   If parameters not valid
    	* @leave KErrBadHandle  If session handle is invalid. 
    	*/
    	void CheckCommonParametersL(TInt aTarget, TInt aDuration, TInt aIntensity) const;
        
    	/**
    	* Checks the validity of duration type API call parameters
    	*
    	* @param aDuration  aDuration parameter of API call
    	*/
    	void CheckDurationParameterL(TInt aDuration) const;
    	
    	/**
    	* Checks the validity of target type API call parameters.
    	*
    	* @param aTarget  aTarget parameter of API call
    	*/
    	void CheckTargetParameterL(TInt aTarget) const;
    	
        /**
        * Do actual reservation. For parameters, see ReserveLightL
        */        
        void DoReserveLightL(TInt aTarget, TBool aRestoreState, TBool aForceNoCCoeEnv);
        
        /**
        * Do actual release
        */
    	void DoReleaseLight(TInt aTarget);

        /**
        * Send set color request to HWRM Server
        * aSetColor - specifies whether it is called for set color or default color.
        */

        void DoSetLightColorL(TBool aSetColor, TInt aTarget, TUint8 aRed=0, TUint8 aGreen=0, TUint8 aBlue=0);

        void DoLightOnWithColorL(TInt aTarget, TInt aDuration, TInt aIntensity, TBool aFadeIn,
                                 const THWRMLightColor& aRGBParam);

        void DoLightBlinkWithColorL(TInt aTarget, TInt aDuration, TInt aOnDuration, 
                                TInt aOffDuration, TInt aIntensity, const THWRMLightColor& aRGBParam);



    public:     // Data
    
    protected:  // Data

    private:    // Data

        RHWRMClient                        iClient;           // HWRM server client
        MHWRMExtendedLightObserver*        iCallback;         // Client listening events. Not owned
        CHWRMExtendedLightStatusObserver*  iStatusObserver;   // Light status observer pointer. Owned.
        CHwrmUiPluginHandler*            iUiPluginHandler; //

        TInt                       iReserved;         // Bitmask to indicate what targets are reserved currently.
        TInt                       iAutoReserving;    // Bitmask to indicate what targets are being autoreserved.
                                                      // and autoreleased on background/foreground transitions.
        
        TInt                       iSupportedTargets; // Supported targets mask.
        
    public:     // Friend classes

        friend class CHWRMExtendedLight;
        friend class CHWRMEnhancedLight;


    protected:  // Friend classes

    private:    // Friend classes

    };          
    
#endif      // HWRMEXTENDEDLIGHTIMPL_H   
            
// End of File
