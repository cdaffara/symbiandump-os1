/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Control interface for Condition Evaluator plug-ins.
*
*/


#ifndef SENSRVCONDITIONEVALUATOR_H
#define SENSRVCONDITIONEVALUATOR_H

/*****************************************************
*       Series 60 Customer
*       Needs customer implementation
*****************************************************/

#include <e32base.h>
#include <sensrvchannelcondition.h>
#include <sensrvchannelinfo.h>
#include <sensrvresource.hrh>

/**
* Condition Evaluator class for Sensor Server. Provides 
* mandatory ECOM interface specific things in addition to 
* condition evaluation services.
*
* Note that Condition Evaluator plug-ins run in Sensor Server main
* thread, so all method calls should return promptly.
*
* @since S60 5.0
*/
class CSensrvConditionEvaluator : public CBase
    {
public:
    /**
    * Two-phase constructor
    *
    * @since S60 5.0
    * @param[in] aChannelInfo Determines the channel information of channel that the loaded plug-in
    *            is needed for.
    *            Correct Condition Evaluator plug-in for each channel is found
    *            by matching for channel type UID (full 0x00000000 format) in default
    *            data field of the implementation information of the plug-in.
    * @return CSensrvConditionEvaluator* Pointer to created CSensrvConditionEvaluator object
    */  
    inline static CSensrvConditionEvaluator* NewL( TSensrvChannelInfo& aChannelInfo );

    /**
    * Virtual destructor
    *
    * @since S60 5.0
    */  
    inline virtual ~CSensrvConditionEvaluator();

public:

    /**
    * Check given condition against given data item. 
    * This will be called possibly many times for each data ítem, so
    * any processing done in this method should be kept at minimum.
    *
    * Note: Any conditions given in parameters aCondition1
    * and aCondition2 will be checked with CheckConditionValidity-method
    * before this method is called for the first time for those conditions,
    * so no duplicate validity checking is necessary. 
    *
    * @since 4.0
    * @param aCondition1 The first condition to check data item against.
    *        If the condition is a two-part condition, this contains 
    *        the first part and the other part is given in aCondition2.
    *        This parameter is never NULL.
    * @param aCondition2 The second condition to check data item against.
    *        Only used with two-part conditions, otherwise NULL.
    * @param aData The data item to be evaluated.
    * @return ETrue if match is found, EFalse otherwise
    * @leave One of Symbian error codes
    */
    virtual TBool EvaluateConditionL(const CSensrvChannelCondition* aCondition1, 
                                    const CSensrvChannelCondition* aCondition2,  
                                    const TDesC8& aData) = 0;

    /**
    * Checks that condition values and indexes are valid for a 
    * single or two-part condition. Both conditions have same index.
    *
    * @since S60 5.0
    * @param aCondition1 The first condition to check.
    *        If the condition is a two-part condition, this contains 
    *        the first part and the other part is given in aCondition2.
    *        This parameter is never NULL.
    * @param aCondition2 The second condition to check.
    *        Only used with two-part conditions, otherwise NULL.
    * @return ETrue if given condition is valid.
    * @leave One of Symbian error codes
    */  
    virtual TBool CheckConditionValidityL( 
                            const CSensrvChannelCondition* aCondition1, 
                            const CSensrvChannelCondition* aCondition2 ) = 0;

private:
    
    /**
    * Destructor key for a ECOM component
    */
    TUid iDestructorIdKey;
    };

#include <sensrvconditionevaluator.inl>

#endif //SENSRVCONDITIONEVALUATOR_H

// End of File


