/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Channel condition object implementation
*
*/


#ifndef SENSRVCHANNELCONDITIONIMPL_H
#define SENSRVCHANNELCONDITIONIMPL_H

#include <e32base.h>
#include "sensrvchannelcondition.h"

class CSensrvChannelConditionSetImpl;

/**
* CSensrvChannelConditionImpl is an implementation of CSensrvChannelCondition
* interface. 
*/
NONSHARABLE_CLASS(CSensrvChannelConditionImpl) : public CSensrvChannelCondition
    {

    friend class CSensrvChannelCondition;
    friend class CSensrvChannelConditionSetImpl;
    
    public:
    
        virtual ~CSensrvChannelConditionImpl();
        
    public:  
    
        /**
        * Return condition type
        *
        * @since S60 5.0
        * @return TSensrvConditionType Type of the condition
        */    
        virtual TInt ConditionType() const;

        /**
        * Return condition operator
        *
        * @since S60 5.0
        * @return TSensrvConditionOperators Condition operator
        */      
        virtual TInt ConditionOperator() const;
      
        /**
        * Return condition item index
        *
        * @since S60 5.0
        * @return TInt Condition item index
        */      
        virtual TInt ConditionItemIndex() const;

        /**
        * Return condition value
        *
        * @since S60 5.0
        * @param[out] aValue Reference to descriptor where condition limit value
        *              should be copied to. 
        *              Typically this is a package buffer defined for data type of 
        *              this channel.
        * @return TInt KErrOverflow if aData is wrong size for data item.
        */      
        virtual TInt GetConditionValue( TDes8& aValue ) const;       

        /**
        * Get condition value as descriptor.
        *
        * @since S60 5.0
        * @return TDescC8& Reference to condition value descriptor.
        */      
        virtual const TDesC8& ConditionValue() const;   
        
     private:
     
        CSensrvChannelConditionImpl
                ( TInt aConditionType, 
                  TInt aConditionOperator,
                  TInt aItemIndex );
     
        void ConstructL(TDesC8& aValue);

    private:     
    
        /**
        * Type of this condition.
        */
        TInt iType;
        
        /**
        * Operator type of this condition.
        */
        TInt iOperator;
        
        /**
        * Index of the data item this condition is for.
        */
        TInt iIndex;
        
        /**
        * Data item specifying the limit value.
        * Own.
        */
        HBufC8* iValue;
    };


#endif //SENSRVCHANNELCONDITIONIMPL_H

// End of File


