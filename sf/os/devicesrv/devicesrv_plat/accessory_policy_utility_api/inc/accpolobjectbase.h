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
 * Description:  Base class for accessory policy name/value pair objects
 *
 */

#ifndef ACCPOLOBJECTBASE_H
#define ACCPOLOBJECTBASE_H

#include <e32base.h>
#include <s32mem.h>

class CAccPolObjectCon;
    
/**
 * Base class for accessory policy name/value pair object(s).
 * Objects which are inherited from the CAccPolObjectBase can be delivered 
 * inside an object container to accessory server's clients.
 *
 *  @Accpolicy.lib 
 */
class CAccPolObjectBase : public CBase
    {
    
public:         

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CAccPolObjectBase();        
        
    /**
     * Sets the object type.
     *     
     * @param aObjectType, Object type to be stored.
     * @return void.
     */
    IMPORT_C void SetObjectType( TUint8 aObjectType );
    
    /**
     * Returns the object type.
     *     
     * @return TUint8, Value of object type.
     */
    IMPORT_C TUint8 ObjectType() const;
    
    /**
     * Internalize an object data. Object which is inherited from
     * CAccPolObjectBase must implement this.
     *     
     * @param aStream, Read stream.
     * @return void.
     */
    virtual void InternalizeL( RReadStream& aStream ) = 0;

    /**
     * Externalize object data. Object which is inherited from
     * CAccPolObjectBase must implement this.
     *     
     * @param aStream, Write stream.
     * @return void.
     */
    virtual void ExternalizeL( RWriteStream& aStream ) const = 0;  
        
    /**
     * Duplicates a object data. Object which is inherited from
     * CAccPolObjectBase must implement this.
     *
     * @param aObject, Pointer to object container where the object is stored.
     * @return CAccPolObjectBase, Pointer to duplicated object.
     */
     virtual CAccPolObjectBase* DuplicateLC( CAccPolObjectCon* aObject = NULL ) = 0;
         
protected:    
    
    /**
     * Copies a base class data.
     *     
     * @param CAccPolObjectBase, a Const reference to object pointer.
     * @return CAccPolObjectBase, Reference to object pointer.
     */    
    IMPORT_C CAccPolObjectBase& operator= ( const CAccPolObjectBase& );
                
    /**
     * C++ default constructor.
     */
    IMPORT_C CAccPolObjectBase();
    
    /**
     * C++ default constructor.
     *
     * @param aObjectType, Object type.
     * @param aObject, Pointer to object container where the object is stored.
     */
    IMPORT_C CAccPolObjectBase( TUint8 aObjectType, CAccPolObjectCon* aObject = NULL );

protected:                  
    
    /**
     * Pointer to object container.      
     */
    CAccPolObjectCon* iObjectContainer;
             
private:

    /**
     * Object type
     */
    TUint8 iObjectType;
    };
    
#endif // ACCPOLOBJECTBASE_H
