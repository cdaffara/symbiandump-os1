/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Plug-in implementation
*
*/


#ifndef SENSRVCONDEVAL_H
#define SENSRVCONDEVAL_H

#include <e32std.h>
#include <e32base.h>
#include <sensrvconditionevaluator.h>
#include <sensrvchannelinfo.h>
#include <sensrvchanneldatatypes.h>



/**
*  Condition evaluator 
*
*  @lib sensrvcondeval.lib
*  @since S60 4.0
*/
class CSensrvCondEval : public CSensrvConditionEvaluator
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
		* @return The created object.
        */
        static CSensrvCondEval* NewL(TSensrvChannelInfo* aChannelInfo);
        
        /**
        * Destructor.
        */
        virtual ~CSensrvCondEval();

    public: // From CSensrvConditionEvaluator
        
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
        * @param aCondition2 The second condition to check data item against.
        *        Only used with two-part conditions, otherwise NULL.
        * @param aData The data item to be evaluated.
        * @return ETrue if match is found, EFalse otherwise
        */
        virtual TBool EvaluateConditionL(const CSensrvChannelCondition* aCondition1, 
                                         const CSensrvChannelCondition* aCondition2,  
                                         const TDesC8& aData);

        /**
        * Checks that condition values and indexes are valid for a 
        * single or two-part condition.
        *
        * @since S60 4.0
        * @param aCondition1 The first condition to check.
        *        If the condition is a two-part condition, this contains 
        *        the first part and the other part is given in aCondition2.
        * @param aCondition2 The second condition to check.
        *        Only used with two-part conditions, otherwise NULL.
        * @return ETrue if given condition is valid.
        */  
        virtual TBool CheckConditionValidityL( 
                                const CSensrvChannelCondition* aCondition1, 
                                const CSensrvChannelCondition* aCondition2 );
		
	private:
	    CSensrvCondEval(TSensrvChannelInfo* aChannelInfo);
	    void ConstructL();
	    
	    /**
	    * Get acceleration value for specified index from accelerometer axis data.
	    *
	    * @since 4.0
	    * @param aIndex Index of the axis for which the value is required
	    * @param aData The accelerometer axis data object reference
	    * @return Acceleration value
	    */
	    TInt AccelerationValue(TInt aIndex, const TSensrvAccelerometerAxisData& aData);
	    
	    /**
	    * Compare two values using the specified operator
	    *
	    * @param aValue1 First value to compare
	    * @param aValue2 Second value to compare
	    * @param aOperator Comparison operator
	    * @return ETrue if comparison result is true.
	    */
	    TBool Compare(TInt aValue1, TInt aValue2, TInt aOperator);

    private:
    	    
	    /**
	    * Channel information if the channel this plug-in is instantiated for.
	    */
	    TSensrvChannelInfo& iChannelInfo;
    };

#endif // SENSRVCONDEVAL_H

