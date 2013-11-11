/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Container for accessory policy name/value pair objects
 *
 */

#ifndef ACCPOLOBJECTCON_H
#define ACCPOLOBJECTCON_H

#include <accpolobjectbase.h>

/**
 *  Base class for container objects. Objects which are added to container 
 *  must be inherited from "CAccPolObjectBase" class.
 *
 *  @AccPolicy.lib  
 */
class CAccPolObjectCon : public CAccPolObjectBase
    {
    
public:

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CAccPolObjectCon();                              
    
    /**
     * Resets the object container.
     * It frees all memory allocated to the array and resets the internal 
     * state so that it is ready to be reused.
     *     
     * @return void.     
     */     
    IMPORT_C void Reset();
    
    /**
     * Duplicates the object and adds that to the object container.     
     *          
     * @param aObject, Object to be added to container.                
     * @leave, Leaves with one of the system wide error codes, 
     *         if the operation fails.
     * @return void.     
     */          
    IMPORT_C void AddL( const CAccPolObjectBase& aObject ); 
    
    /**
     * Gets a reference to the object pointer located at 
     * the specified position within the array.
     *          
     * @param aIndex, Position of object in object container.      
     * @leave, Leaves with one of the system wide error codes, 
     *         if the operation fails.
     * @return CAccPolObjectBase, A const reference to 
     *         the object pointer at position anIndex within the array.     
     */
    IMPORT_C CAccPolObjectBase *const &AtL( TInt aIndex ) const;  
    
    /**
     * Returns the object count.
     *     
     * @return TInt, Count of objects in the object container.
     */
    IMPORT_C TInt Count() const;
            
protected:    
                  
    /**
     * C++ default constructor.
     */
    IMPORT_C CAccPolObjectCon();        
    
    /**
     * C++ default constructor.
     *
     * @param aObjectType, Object type.      
     * @param aObject, Pointer to object container where the object is stored.
     */
    IMPORT_C CAccPolObjectCon( TUint8 aObjectType, CAccPolObjectCon* aObject = NULL );
                          
private:  
     
    /**
     * Array for objects which are inherited from CAccObjectBase.
     */
    RPointerArray<CAccPolObjectBase> iObjects;
        
  };

#endif // ACCPOLOBJECTCONBASE_H
