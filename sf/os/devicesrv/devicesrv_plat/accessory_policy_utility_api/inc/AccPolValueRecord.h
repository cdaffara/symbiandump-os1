/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Value record definition part of the Name/Value record concept.
*
*/



#ifndef TACCPOLVALUERECORD_H
#define TACCPOLVALUERECORD_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// Type enumeration for value
enum TAccPolValueType
  {
  EAPVNone = 0,
  EAPVBool,
  EAPVInt,
  EAPVUInt,
  EAPVDes8,
  EAPVObject
  };

// Location enumeration for value
enum TAccPolValueLocation
  {
  EAPVNotDefined = 0,
  EAPVAccessory,
  EAPVPolicy, 
  };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAccPolObjectCon;

// CLASS DECLARATION

class TAccPolValueRecord
  {
    public:  // Constructors and destructor
        
    /**
     * C++ default constructor.
     */
    IMPORT_C TAccPolValueRecord();
     
    /**
     * C++ constructor.
     *
     * @since S60 3.1
     * @param aValue    Capability value.
     * @param aType     Capability type.   
     * @param aLocation Capability location.
     */
    IMPORT_C TAccPolValueRecord( TInt aValue, 
                                 TAccPolValueType aType, 
                                 TAccPolValueLocation aLocation );
                                                                                                                                        
    /**
     * C++ constructor.
     *
     * @since S60 3.1
     * @param aValue    Capability value.     
     * @param aLocation Capability location.
     */
    IMPORT_C TAccPolValueRecord( TDesC8& aValue,
                                 TAccPolValueLocation aLocation );
                                                                                                           
    /**
     * C++ constructor.
     *     
     * @param aLocation Capability location.
     * @param aValue    Capability value.          
     */
    IMPORT_C TAccPolValueRecord( TAccPolValueLocation aLocation,
                                 CAccPolObjectCon* aObject = NULL );
								 
    public: // New functions
            
    /**
     * Returns the value of value record.
     *
     * @since S60 3.1     
     * @return TInt holding the value of the value record.
     */     
    IMPORT_C TInt Value() const;
        
    /**
     * Sets the integer value of value record.
     *
     * @since S60 3.1   
     * @param aValue Capability value.     
     * @return void
     */          
    IMPORT_C void SetValue( TInt aValue );
        
    /**
     * Gets the descriptor type value of value record.
     *
     * @since S60 3.1   
     * @param aValue Capability value.     
     * @return void
     */                   
    IMPORT_C void GetValueL( TDesC8& aValue ) const;
        
    /**
     * Returns the reference to the value of value record.
     *
     * @since S60 3.1
     * @return TDesC8 reference to the value.
     */
    IMPORT_C TDesC8& ValueRef();

    /**
     * Sets the descriptor type of value to the value record.
     *
     * @since S60 3.1   
     * @param aValue Reference to capability value.     
     * @return void
     */                   
    IMPORT_C void SetValue( const TDesC8& aValue );
        
    /**
     * Returns the type of the value of value record.
     *
     * @since S60 3.1     
     * return TAccPolValueType holding the value type of the value record.
     */     
    IMPORT_C TAccPolValueType Type() const;
         
    /**
     * Sets the type of the value of value record.
     *
     * @since S60 3.1 
     * @param aType Capability type.
     * return void.
     */        
    IMPORT_C void SetType( TAccPolValueType aType );
        
    /**
     * Returns the location of the value of value record.
     *
     * @since S60 3.1      
     * @return TAccPolValueLocation holding the value location of the value record.
     */        
    IMPORT_C TAccPolValueLocation Location() const;

    /**
     * Sets the location of the value of value record.
     *
     * @since S60 3.1      
     * @param aType Capability location.
     * @return void.
     */            
    IMPORT_C void SetLocation( TAccPolValueLocation aLocation );
    
    /**
     * Sets content of a value record.
     *
     * @since S60 3.1
     * @param aValue    Value.
     * @param aType     Type.
     * @param aLocation Location.
     * @return void
     */
    IMPORT_C void SetL( TInt aValue, 
                        TAccPolValueType aType, 
                        TAccPolValueLocation aLocation );                                                          
                                                              
  private:

    // The value of the value record. iType member defines the actual type of the value,
    TInt           iValue;
    TPtrC8         iValueRef;//Not owned
  
    // If value is defined as Bool, Int or unsigned Int iValue has meaning,
    // otherwise iValue is not used (empty).
    TAccPolValueType iType;

    // Location of the value (TAccPolValueLocation)
    TAccPolValueLocation iLocation;
  };

#endif // ACCNAMEVALUERECORD_H   

// End of File
