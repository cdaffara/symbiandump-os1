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
* Description:  Channel condition set object
*
*/


#ifndef SENSRVCHANNELCONDITIONSETIMPL_H
#define SENSRVCHANNELCONDITIONSETIMPL_H

#include <sensrvchannelconditionset.h>

// CLASS DECLARATION

/**
* CSensrvChannelConditionSetImpl is an implementation of interface 
* CSensrvChannelConditionSet.
*/
NONSHARABLE_CLASS(CSensrvChannelConditionSetImpl) : public CSensrvChannelConditionSet
    {
    friend class CSensrvChannelConditionSet;
    
    public:
        
        virtual ~CSensrvChannelConditionSetImpl();
        
    public:
          
        /**
        * Return condition set type.
        *
        * @since S60 5.0
        * @return TSensrvConditionSetType Type of the condition set
        */    
        virtual TSensrvConditionSetType ConditionSetType() const;
        
        /**
        * Add channel condition to the condition set. Ownership of the 
        * CSensrvChannelCondition is transferred to the CSensrvChannelConditionSet. 
        * The channel condition is deleted when this condition set is destroyed.
        *
        * @since S60 5.0
        * @param[in] aChannelCondition Channel condition to be added
        * @exception KErrArgument NULL condition or there is already a condition
        *            for the same index in this set and the new condition is not a
        *            complementary part of range condition for that existing condition.
        */  
        virtual void AddChannelConditionL
                ( CSensrvChannelCondition* aChannelCondition );

        /**
        * Get all channel conditions added to this set.
        *
        * @since S60 5.0
        * @return Channel condition pointers are returned in this list.
        */  
        virtual const RSensrvChannelConditionList& AllConditions() const;
    
        /**
        * Returns id of this condition set
        *
        * @since S60 5.0
        * @return TInt Condition set id
        */      
        inline TInt ConditionSetId() const { return iId; };

        /**
        * Sets id of this condition set
        *
        * @since S60 5.0
        * @param aId New condition set id
        */      
        inline void SetConditionSetId( TInt aId ) { iId = aId; };
        
        /**
        * Check if this set has just been added to listening.
        *
        * @since S60 5.0
        * @return ETrue if set has been just added.
        */ 
        inline TBool IsJustAdded() const { return iJustAdded; } ;
        
        /**
        * Clear iJustAdded flag
        *
        * @since S60 5.0
        */ 
        inline void ClearJustAdded() { iJustAdded = EFalse; } ;

        /**
        * Checks if condition set is complete, i.e. there are
        * no unpaired range conditions. 
        * Empty sets are not considered complete.
        *
        * @since S60 5.0
        * @return ETrue if set is complete.
        */
        virtual TBool IsComplete() const;
        
        /**
        * Serializes set data into single buffer that can be
        * transported from client to server.
        *
        * @since S60 5.0
        * @return New buffer containing serialized set
        */
        virtual HBufC8* SerializeLC() const;

        /**
        * Takes serialized condition set as input and makes this set
        * a clone of the original serialized set
        *
        * @since S60 5.0
        * @param[in] aSerializedSet Serialized set
        * @return Standard symbian error code
        */  
        virtual TInt Deserialize( const TDesC8& aSerializedSet );
        
        /**
        * Set met data.
        *
        * @since S60 5.0
        * @param[in] aMetData Data that caused set to be met.
        * @return Standard symbian error code
        */
        virtual TInt SetMetData( const TDesC8& aMetData );
        
        /**
        * Get pointer to met data.
        *
        * @since S60 5.0
        * @return Pointer to iMetData. Can be NULL.
        */
        inline HBufC8* MetData() const { return iMetData; };

    private:     
        CSensrvChannelConditionSetImpl(TSensrvConditionSetType aConditionSetType);
     
        void ConstructL();

    public:
        
        /**
        * Link item for queueing condition set pointers
        */
        TSglQueLink iLink;

    private:
    
        /**
        * Type of this set.
        */
        TSensrvConditionSetType iSetType;
        
        /**
        * List of conditions added to this set.
        * Conditions in the list are owned.
        */
        RSensrvChannelConditionList iConditionList;
        
        /**
        * Condition set id. Must be unique in the scope of client session.
        */
        TInt iId;
        
        /**
        * Flag to indicate that set has just been added and should be ignored
        * at next buffer fill handling.
        * Only used on server side, not serialized.
        */
        TBool iJustAdded;
        
        /**
        * Data that caused condition set to be met.
        * Only used on server side, not serialized.
        * Own.
        */
        HBufC8* iMetData;
    };


#endif //SENSRVCHANNELCONDITIONSETIMPL_H

// End of File


