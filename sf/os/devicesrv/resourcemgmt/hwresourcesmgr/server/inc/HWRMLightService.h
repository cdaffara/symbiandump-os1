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

#ifndef HWRMLIGHTSERVICE_H
#define HWRMLIGHTSERVICE_H

//  INCLUDES
#include <e32base.h>
#include "HWRMGenericTimer.h"
#include "HWRMLightCommands.h"
#include "HWRMLight.h"
#include "HWRMService.h"
#include "HWRMLightCommonData.h"
#include "HWRMLightClientServer.h" // for KHWRMLightMaxTargets

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES

/**
* Set of datas that store single target state 
*/
struct THWRHLightTargetStateData
    {
    CHWRMLight::TLightStatus iState;  // Light state
    TInt iIntensity;                  // Light intensity 
    TInt iOnCycleDuration;            // On cycle duration (if base state is blink), ms
    TInt iOffCycleDuration;           // Off cycle duration (if base state is blink), ms.
    TBool iFade;                      // Fade in or out. (not used for blink state).
    TUint iColor;                     // R G B values.
    };

/**
* Data struct for light target specific data
*/
struct THWRMLightTargetData 
    {
    TInt               iTarget;    // The identifier of target this data refers to.
    CHWRMGenericTimer* iTimer;     // Timer pointer for duration based calls. Not owned.
    TBool              iSuspended; // Flag to indicate if this target is suspended.
    TBool              iReserved;  // Flag to indicate if this target is reserved.
    TInt               iFrozenTime; // Remaining light timer duration at last release.    T

    THWRHLightTargetStateData iBaseState;   // Stores light base state
    THWRHLightTargetStateData iLastState;   // Stores last desired light state
    THWRHLightTargetStateData iFrozenState; // Stores freeze state for light
    };


// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CHWRMLightCommonData;
class CHWRMPluginHandler;
class CHWRMReservationHandler;

// CLASS DECLARATION

/**
* Data storage class for light plugin requests.
*/
class THWRMLightPluginRequestData : public THWRMPluginRequestData
    {
    public:
        TInt iTarget; // Target of the request

        /**
	    * Convenience constructor
	    */
	    THWRMLightPluginRequestData(TInt aTarget,
	                                const RMessage2& aRequestMessage,
	                                TUint8 aTransId, 
	                                TInt   aCommandId,
	                                TBool  aSplitCommand,
	                                TInt   aSplitCommandId,
	                                TAny*  aSplitCommandData)
	        : THWRMPluginRequestData(aRequestMessage, aTransId, aCommandId, aSplitCommand, aSplitCommandId, aSplitCommandData),
	          iTarget(aTarget)
	            {
	            };
	    /**
	    * Virtual destructor.
	    */
	    virtual ~THWRMLightPluginRequestData()
	        {
	        };
    };

/**
*  Hardware Resource Manager server side service object for Light sessions.
*
*/
class CHWRMLightService : public CHWRMService,
                          public MHWRMGenericTimerCallback
    {
    private:  // Enumerations
    
        /**
        * Private enumeration to identify state change type
        */
        enum TStateChangeType
            {
            EStateChangeFrozenState = 0,
            EStateChangeBaseState,
            EStateChangeLastState,
            EStateChangeDefaultState
            };

    public: // Constructors and Destructor
    
        /**
        * Two-phased constructor.
        *
        * @param aPluginHandler The plugin handler to use with this service.
        */
        static CHWRMLightService* NewL(CHWRMPluginHandler* aPluginHandler,
                                       CHWRMReservationHandler* aReservationHandler,
                                       CHWRMLightCommonData& aLightCommonData);
        
        /**
        * Destructor.
        */
        virtual ~CHWRMLightService();
        
    public: // Base class methods
    	
    	virtual TBool ExecuteMessageL( const RMessage2& aMessage );
    	virtual void ProcessResponseL( TInt aCommandId, TUint8 aTransId, TDesC8& aData, TBool aTimeout );
    	virtual void SuspendSubResource(TInt aSubResource);
    	virtual void ResumeSubResource(TInt aSubResource);
        virtual void ActivateSubResource(TInt aSubResource, TBool aActivate);    	    	    
        
    	// From MHWRMGenericTimerCallback    	
    	virtual void GenericTimerFired(TInt aTimerId, TBool aCutOff);
    	
    public: // Other methods
    
        /**
        * Updates default sensor sensitivity to plugin
        *
        * @param aSensitivity New default sensitivity value.
        */
        void UpdateDefaultSensitivityL(TInt aSensitivity);

        /**
        * Updates default intensity to plugin
        *
    	* @param aTarget The light target mask
        * @param aIntensity New default intensity value for specified targets.
        */
        void UpdateDefaultIntensityL(TInt aTarget, TInt aIntensity);
        
        /**
        * Turns all unreserved system lights on.
        */
        void SystemLightsOn();

    private: 
    
        /**
        * C++ default constructor.
        */
        CHWRMLightService(CHWRMLightCommonData& aLightCommonData);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(CHWRMPluginHandler* aPluginHandler,
                        CHWRMReservationHandler* aReservationHandler);
        
    	/**
    	* Turns lights on.
    	*
    	* @param aTarget The light target
    	* @param aDuration  Duration of timer, ms
    	* @param aIntensity Intensity
    	* @param aFadeIn fade-in
    	* @param aMessage Related message
    	* @param aTimerInitiated If ETrue, call is initiated by a timer.
    	* @param aActivationInitiated If ETrue, call is initiated by activation of target.
    	*/
	    void LightsOnL(TInt aTarget,
	                   TInt aDuration, 
	                   TInt aIntensity,
	                   TBool aFadeIn,
	                   const RMessage2& aMessage,
	                   TBool aTimerInitiated = EFalse,
	                   TBool aActivationInitiated = EFalse);
  	
    	/**
    	* Turns lights off.
    	*
    	* @param aTarget The light target
    	* @param aDuration  Duration of timer, ms
    	* @param aFadeOut fade-out
    	* @param aMessage Related message
    	* @param aTimerInitiated If ETrue, call is initiated by a timer.
    	* @param aActivationInitiated If ETrue, call is initiated by activation of target.
    	*/
	    void LightsOffL(TInt aTarget,
	                    TInt aDuration, 
	                    TBool aFadeOut,
	                    const RMessage2& aMessage,
	                    TBool aTimerInitiated = EFalse,
	                    TBool aActivationInitiated = EFalse);
  	
    	/**
    	* Starts lights blinking
    	*
    	* @param aTarget The light target
    	* @param aDuration  Duration of timer, ms
    	* @param aIntensity Intensity
    	* @param aOnCycleTime On cycle time, ms
    	* @param aOffCycleTime Off cycle time, ms 
    	* @param aMessage Related message
    	* @param aTimerInitiated If ETrue, call is initiated by a timer.
    	* @param aActivationInitiated If ETrue, call is initiated by activation of target.
    	*/
	    void LightsBlinkL(TInt aTarget,
	                      TInt aDuration, 
	                      TInt aIntensity,
	                      TInt aOnCycleTime,
	                      TInt aOffCycleTime,
	                      const RMessage2& aMessage,
	                      TBool aTimerInitiated = EFalse,
	                      TBool aActivationInitiated = EFalse);
    	
    	/**
    	* Restores base state of the target light.
    	*
    	* @param aTargetData Reference to target data. 
    	*         If restoring default state, this is ignored,
    	*         but still cannot be NULL reference.
    	* @param aStateType Determines which type of state is restored
    	* @param aTimerInitiated If ETrue, call is initiated by a timer.
    	* @param aDefaultTargetMask Target mask used in restoring 
    	*         default state. Ignored in other restores.
    	* @param aMessage A pointer to message to use in transaction. If NULL,
    	*         a dummy message is used.
    	* @param aActivationInitiated If ETrue, call is initiated by activation of target.
    	*/
    	void RestoreState(const THWRMLightTargetData& aTargetData, 
    	                  TStateChangeType aStateType,
    	                  TBool aTimerInitiated = EFalse,
    	                  TInt aDefaultTargetMask = 0,
    	                  const RMessage2* aMessage = NULL,
	                      TBool aActivationInitiated = EFalse );
    	

    	/**
    	* Stores base states of target data objects as necessary.
    	* Also sets or cancels relevant timers.
    	*
    	* @param aTimerInitiated If ETrue, original call was timer initiated.
    	* @param aDuration Duration of the call.
    	* @param aTarget A light target. If target is mask, multiple 
    	*        timers can be started.
    	* @param aState The light state to be set
    	* @param aIntensity Intensity of initiating call
    	* @param aFade fade-in or fade-out of initiating call
    	* @param aOnCycleTime On cycle time of the initiating call
    	* @param aOffCycleTime Off cycle time of the initiating call
    	*/
    	void UpdateTimersAndStoreStates(TBool aTimerInitiated,
    	                TInt aDuration,
    	                TInt aTarget,
                        CHWRMLight::TLightStatus aState,
	                    TInt aIntensity,
	                    TBool aFade,
                        TInt aOnCycleTime,
                        TInt aOffCycleTime);

    	/**
    	* Checks the validity of common API call parameters.
    	* Also checks the session handle.
    	*
    	* @param aTarget    aTarget parameter of API call
    	* @param aDuration  aDuration parameter of API call
    	* @param aIntensity aIntensity parameter of API call
    	* @leave KErrArgument   If parameters not valid
    	* @leave KErrBadHandle  If plugin handler reference is invalid. 
    	*/
    	void CheckCommonParametersL(TInt aTarget, 
    	                            TInt aDuration, 
    	                            TInt aIntensity) const;
        
    	/**
    	* Checks the validity of duration type API call parameters
    	*
    	* @param aDuration  aDuration parameter of API call
    	* @leave KErrArgument   If parameters not valid
    	*/
    	void CheckDurationParameterL(TInt aDuration) const;
    	
    	/**
    	* Checks the validity of target type API call parameters.
    	*
    	* @param aTarget  aTarget parameter of API call
    	*/
    	void CheckTargetParameterL(TInt aTarget) const;
    	
    	/**
    	* Gets mask of unsuspended targets and checks their
    	* reservation status.
    	*
    	* @param aTarget Targets to be checked.
    	* @return Mask of unsuspended targets.
    	* @leave KErrInUse if any of unsuspended targets is 
    	*             reserved to some other service
    	*/
    	TInt GetAndCheckUnsuspendedTargetsL(TInt aTarget) const;
    	
    	/**
    	* Executes a plugin command
        *
        * @param aTarget Targets of the plugin command
        * @param aMessage Message related to this command.
        * @param aCommandId Command ID of the command.	
        * @param aPckg Command package
        * @param aSplitCommand Indicates if this execution is part of split command.
    	*/
        void ExecutePluginCommandL(TInt aTarget,
                                  const RMessage2& aMessage,
                                  TInt aCommandId,
                                  TDesC8& aPckg,
                                  TBool aSplitCommand,
                                  TInt aSplitCommandId = 0, 
                                  TAny* aSplitCommandData = NULL  );
        
        /**
        * Returns suitable modified target mask for general case
        *
        * @param aTarget Original target
        * @param aLightsOff ETrue if mask is needed for lights off command
        * @return Modified target mask
        * @leave KErrNotSupported if target is not supported at this time.
        */
        TInt ModifyTargetMaskL(TInt aTarget, TBool aLightsOff);
        
        /**
        * Cleans up light service.
        * Attempts to restore lights to proper states.
        */
        void CleanupLights();

        /**
        * Releases lights.
        * Called by ExecuteMessageL.
        */
        void ReleaseLights(TInt aTargetMask);
        
        /**
        * Reserves lights.
        * Called by ExecuteMessageL.
        *
        * @param aTargetMask
        * @param aMessage Message pointer for identifying the reserving client
        * @param aForceNoCCoeEnv Indicates if reservation is done in force CCoeEnv mode.
        */
        void ReserveLightsL(const RMessage2& aMessage);

        /**
        * Turns lights on.
        *
        * @param aTarget The light target
        * @param aColor  color data
        *
        */
        TBool SetColorL(TInt aTarget, TUint aColor, const RMessage2& aMessage, TBool aActivationInitiated = EFalse,
                       TBool aSplitCommand = EFalse, TInt aSplitCommandId = 0, TAny* aSplitCommandData = NULL);


    private:  // data

        CHWRMLightCommonData&     iLightCommonData; // Pointer to light data object common to all sessions. Not owned.                 
        THWRMLightTargetData      iTargets[KHWRMLightMaxTargets]; // Target specific datas. Owned. Each data refers to target
                                                             // value (KHWRMLightFirstTarget << <index>) in CHWRMLight::TLightTarget. 
        THWRHLightTargetStateData iDefaultState;             // Default state of lights when no reservations are active                                                            
        TInt                      iSupportedTargets;         // Supported targets mask
        TInt                      iFullSystemTarget;         // Full system target
        TBool                     iCleaningUp;               // Indicates that cleanup has started. No new commands are handled.
        
    public: // friends
    
        friend class CHWRMLightTargetHandler;
    };

#endif  // HWRMLIGHTSERVICE_H
            
// End of File
