/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#ifndef CONDEVALSTUBSECOND_H
#define CONDEVALSTUBSECOND_H

#include <e32std.h>
#include <e32base.h>
#include <sensrvconditionevaluator.h>
#include "sensrvtestchanneldatatypes.h"

/**
*  Condition evaluator stub
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class CCondEvalStubSecond : public CSensrvConditionEvaluator
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
		* @return The created object.
        */
        static CCondEvalStubSecond* NewL(TSensrvChannelInfo* aChannelInfo);
        
        /**
        * Destructor.
        */
        virtual ~CCondEvalStubSecond();

    public: // From CSensrvConditionEvaluator
        
    /**
    * Check given condition against given data item. 
    * This will be called possibly many times for each data ítem, so
    * any processing done in this method should be kept at minimum.
    *
    * Note: Any conditions given in parameters aCondition1
    * and aCondition2 will be checked with CheckConditionValidityL-method
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
    * @since S60 5.0
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
	    CCondEvalStubSecond(TSensrvChannelInfo* aChannelInfo);
	    void ConstructL();
	    
	    void TimestampValue(TInt aIndex, const TSensrvTestEventData& aData, TTime& aReturnValue);
	    
	    TBool Compare(const TTime& aValue1, const TTime& aValue2, TInt aOperator);

	    TSensrvChannelInfo iChannelInfo;
    };

#endif // CONDEVALSTUBSECOND_H

