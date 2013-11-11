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
* Description:  Sensor and channel property container.
*
*/


#include <sensrvproperty.h>
#include "sensrvtrace.h"

_LIT(KSensrvPropertyPanic, "SensrvProperty");

// Tracing macros for property contents
#ifdef API_TRACE_DEBUG    

#define _TRACE_INT_PROPERTY_ API_TRACE((_L("Sensor Util - TSensrvProperty::TSensrvProperty - iPropertyId=%d, iItemIndex=%d, iIntValue=%d, iFlags=%d, iIntValueMax=%d, iIntValueMin=%d, iPropertyType=ESensrvIntProperty"),iPropertyId,iItemIndex,iIntValue,iFlags.Value(),iIntValueMax,iIntValueMin));
#define _TRACE_REAL_PROPERTY_ API_TRACE((_L("Sensor Util - TSensrvProperty::TSensrvProperty - iPropertyId=%d, iItemIndex=%d, iRealValue=%f, iFlags=%d, iRealValueMax=%f, iRealValueMin=%f, iPropertyType=ESensrvRealProperty"),iPropertyId,iItemIndex,iRealValue,iFlags.Value(),iRealValueMax,iRealValueMin));
#define COPY_BUFFER(buffer) \
TBuf<KSensrvPropertyTextBufferSize> buf;\
buf.Copy(buffer);

#define _TRACE_BUFFER_PROPERTY_ \
COPY_BUFFER(iBufValue);\
API_TRACE((_L("Sensor Util - TSensrvProperty::TSensrvProperty - iPropertyId=%d, iItemIndex=%d, iBufValue=%S, iFlags=%d, iPropertyType=ESensrvBufferProperty"),iPropertyId,iItemIndex,&buf,iFlags.Value()));

#define _TRACE_BUFVALUE(function,buffer) \
COPY_BUFFER(buffer);\
API_TRACE((_L("Sensor Util - TSensrvProperty::%S - buffer:%S"), &function, &buf));

_LIT(KSensrvFunctionNameSetValue, "SetValue");
_LIT(KSensrvFunctionNameGetValue, "GetValue");
_LIT(KSensrvFunctionNameGetValueRef, "GetValueRef");

#else

#define _TRACE_INT_PROPERTY_
#define _TRACE_REAL_PROPERTY_
#define _TRACE_BUFFER_PROPERTY_
#define _TRACE_BUFVALUE(function,buffer) 

#endif


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TSensrvProperty::TSensrvProperty()
// ---------------------------------------------------------------------------
//
EXPORT_C TSensrvProperty::TSensrvProperty()
    : iArrayIndex( ESensrvSingleProperty ),
      iFlags( EFalse ),
      iPropertyType( ESensrvUninitializedProperty ),
      iReserved( 0 )
    {
    API_TRACE((_L("Sensor Util - TSensrvProperty::TSensrvProperty - iPropertyType=ESensrvUninitializedProperty")));
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::TSensrvProperty()
// ---------------------------------------------------------------------------
//
EXPORT_C TSensrvProperty::TSensrvProperty( const TSensrvPropertyId aPropertyId,
                                  const TInt aItemIndex,
                                  const TInt aValue,
                                  const TInt aMaxValue,
                                  const TInt aMinValue,
                                  const TBool aReadOnly,
                                  const TSensrvPropertyType aPropertyType )
    : iPropertyId( aPropertyId ),
      iItemIndex( aItemIndex ),
      iArrayIndex( ESensrvSingleProperty ),
      iIntValue( aValue ),
      iFlags( aReadOnly ),
      iIntValueMax( aMaxValue ),
      iIntValueMin( aMinValue ),
      iPropertyType( aPropertyType ),
      iReserved( 0 )
    {
    __ASSERT_ALWAYS( ESensrvIntProperty == iPropertyType, User::Panic( KSensrvPropertyPanic, 0 ));

    _TRACE_INT_PROPERTY_
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::TSensrvProperty()
// ---------------------------------------------------------------------------
//
EXPORT_C TSensrvProperty::TSensrvProperty( const TSensrvPropertyId aPropertyId,
                                           const TInt aItemIndex,
                                           const TInt aValue )
    : iPropertyId( aPropertyId ),
      iItemIndex( aItemIndex ),
      iArrayIndex( ESensrvSingleProperty ),
      iIntValue( aValue ),
      iFlags( EFalse ),
      iIntValueMax( 0 ),
      iIntValueMin( 0 ),
      iPropertyType( ESensrvIntProperty ),
      iReserved( 0 )
    {
    _TRACE_INT_PROPERTY_
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::TSensrvProperty()
// ---------------------------------------------------------------------------
//
EXPORT_C TSensrvProperty::TSensrvProperty( const TSensrvPropertyId aPropertyId,
                                           const TInt aItemIndex,
                                           const TReal aValue,
                                           const TReal aMaxValue,
                                           const TReal aMinValue,
                                           const TBool aReadOnly,
                                           const TSensrvPropertyType aPropertyType )
    : iPropertyId( aPropertyId ),
      iItemIndex( aItemIndex ),
      iArrayIndex( ESensrvSingleProperty ),
      iRealValue( aValue ),
      iFlags( aReadOnly ),
      iRealValueMax( aMaxValue ),
      iRealValueMin( aMinValue ),
      iPropertyType( aPropertyType ),
      iReserved( 0 )
    {
    __ASSERT_ALWAYS( ESensrvRealProperty == iPropertyType, User::Panic( KSensrvPropertyPanic, 0 ));

    _TRACE_REAL_PROPERTY_
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::TSensrvProperty()
// ---------------------------------------------------------------------------
//
EXPORT_C TSensrvProperty::TSensrvProperty( const TSensrvPropertyId aPropertyId,
                                           const TInt aItemIndex,
                                           const TReal aValue )
    : iPropertyId( aPropertyId ),
      iItemIndex( aItemIndex ),
      iArrayIndex( ESensrvSingleProperty ),
      iRealValue( aValue ),
      iFlags( EFalse ),
      iRealValueMax( 0 ),
      iRealValueMin( 0 ),
      iPropertyType( ESensrvRealProperty ),
      iReserved( 0 )
    {
    _TRACE_REAL_PROPERTY_
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::TSensrvProperty()
// ---------------------------------------------------------------------------
//
EXPORT_C TSensrvProperty::TSensrvProperty( const TSensrvPropertyId aPropertyId,
                                           const TInt aItemIndex,
                                           const TDesC8& aValue,
                                           const TBool aReadOnly,
                                           const TSensrvPropertyType aPropertyType )
    : iPropertyId( aPropertyId ),
      iItemIndex( aItemIndex ),
      iArrayIndex( ESensrvSingleProperty ),
      iBufValue( aValue ),
      iFlags( aReadOnly ),
      iPropertyType( aPropertyType ),
      iReserved( 0 )
    {
    __ASSERT_ALWAYS( ESensrvBufferProperty == iPropertyType, User::Panic( KSensrvPropertyPanic, 0 ));

    _TRACE_BUFFER_PROPERTY_
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::TSensrvProperty()
// ---------------------------------------------------------------------------
//
EXPORT_C TSensrvProperty::TSensrvProperty( const TSensrvPropertyId aPropertyId,
                                           const TInt aItemIndex,
                                           const TDesC8& aValue )
    : iPropertyId( aPropertyId ),
      iItemIndex( aItemIndex ),
      iArrayIndex( ESensrvSingleProperty ),
      iBufValue( aValue ),
      iFlags( EFalse ),
      iPropertyType( ESensrvBufferProperty ),
      iReserved( 0 )
    {
    _TRACE_BUFFER_PROPERTY_
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::GetPropertyId()
// ---------------------------------------------------------------------------
//
EXPORT_C TSensrvPropertyId TSensrvProperty::GetPropertyId() const
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::GetPropertyId - return %d" ), iPropertyId ) );
    return iPropertyId;
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::SetPropertyId()
// ---------------------------------------------------------------------------
//
EXPORT_C void TSensrvProperty::SetPropertyId( TSensrvPropertyId aPropertyId )
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::SetPropertyId(%d)" ), aPropertyId ) );
    iPropertyId = aPropertyId;
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::PropertyItemIndex()
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TSensrvProperty::PropertyItemIndex() const
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::PropertyItemIndex - return %d" ), iItemIndex ) );
    return iItemIndex;
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::SetItemIndex()
// ---------------------------------------------------------------------------
//
EXPORT_C void TSensrvProperty::SetItemIndex( TInt aItemIndex )
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::SetItemIndex(%d)" ), aItemIndex ) );
    iItemIndex = aItemIndex;
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::PropertyType()
// ---------------------------------------------------------------------------
//
EXPORT_C TSensrvPropertyType TSensrvProperty::PropertyType() const
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::PropertyType - return %d" ), iPropertyType ) );
    return iPropertyType;
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::ReadOnly()
// ---------------------------------------------------------------------------
//
EXPORT_C TBool TSensrvProperty::ReadOnly() const
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::ReadOnly - return %d" ), iFlags.Value() ) );
    return ( iFlags.Value() != 0 );
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::SetReadOnly()
// ---------------------------------------------------------------------------
//
EXPORT_C void TSensrvProperty::SetReadOnly( TBool aReadOnly )
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::SetReadOnly(aReadOnly:%d)" ), aReadOnly ) );
    iFlags = aReadOnly;
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::SetValue()
// ---------------------------------------------------------------------------
//
EXPORT_C void TSensrvProperty::SetValue( const TInt aValue )
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::SetValue(Value:%d)" ), aValue ) );

    if( ESensrvUninitializedProperty == iPropertyType )
        {
        iPropertyType = ESensrvIntProperty;
        }
    else
        {
        __ASSERT_ALWAYS( ESensrvIntProperty == iPropertyType, User::Panic( KSensrvPropertyPanic, 0 ));
        }

    iIntValue = aValue;

    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::SetValue - return void" ) ) );
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::SetValue()
// ---------------------------------------------------------------------------
//
EXPORT_C void TSensrvProperty::SetValue( const TReal aValue )
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::SetValue(Value:%e)" ), aValue ) );

    if( ESensrvUninitializedProperty == iPropertyType )
        {
        iPropertyType = ESensrvRealProperty;
        }
    else
        {
        __ASSERT_ALWAYS( ESensrvRealProperty == iPropertyType, User::Panic( KSensrvPropertyPanic, 0 ));
        }

    iRealValue = aValue;
    
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::SetValue - return void" ) ) );
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::SetValue()
// ---------------------------------------------------------------------------
//
EXPORT_C void TSensrvProperty::SetValue( const TDesC8& aValue )
    {
    _TRACE_BUFVALUE(KSensrvFunctionNameSetValue,aValue);

    if( ESensrvUninitializedProperty == iPropertyType )
        {
        iPropertyType = ESensrvBufferProperty;
        }
    else
        {
        __ASSERT_ALWAYS( ESensrvBufferProperty == iPropertyType, User::Panic( KSensrvPropertyPanic, 0 ));
        }

    iBufValue = aValue;
    
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::SetValue - return void" ) ) );
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::GetValue()
// ---------------------------------------------------------------------------
//
EXPORT_C void TSensrvProperty::GetValue( TInt& aValue ) const
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::GetValue return %d" ), iIntValue ) );

    __ASSERT_ALWAYS( ESensrvIntProperty == iPropertyType, User::Panic( KSensrvPropertyPanic, 0 ));

    aValue = iIntValue;
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::GetValue()
// ---------------------------------------------------------------------------
//
EXPORT_C void TSensrvProperty::GetValue( TReal& aValue ) const
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::GetValue return %e" ), iRealValue ) );

    //__ASSERT_ALWAYS( ESensrvRealProperty == iPropertyType, User::Panic( KSensrvPropertyPanic, 0 ));

    if( ESensrvRealProperty == iPropertyType )
        aValue = iRealValue;
    else if( ESensrvIntProperty == iPropertyType )
        aValue = iIntValue;
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::GetValue()
// ---------------------------------------------------------------------------
//
EXPORT_C void TSensrvProperty::GetValue( TDes8& aValue ) const
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::SetValue(<retval>)" ) ) );

    __ASSERT_ALWAYS( ESensrvBufferProperty == iPropertyType, User::Panic( KSensrvPropertyPanic, 0 ));

    aValue.Copy(iBufValue);

    _TRACE_BUFVALUE(KSensrvFunctionNameGetValue,aValue);
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::GetValueRef()
// ---------------------------------------------------------------------------
//
EXPORT_C TDes8& TSensrvProperty::GetValueRef()
    {
    _TRACE_BUFVALUE(KSensrvFunctionNameGetValueRef,iBufValue);

    __ASSERT_ALWAYS( ESensrvBufferProperty == iPropertyType, User::Panic( KSensrvPropertyPanic, 0 ));

    return iBufValue;
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::GetMaxValue()
// ---------------------------------------------------------------------------
//
EXPORT_C void TSensrvProperty::GetMaxValue( TInt& aMaxValue ) const
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::GetMaxValue return %d" ), iIntValueMax ) );

    __ASSERT_ALWAYS( ESensrvIntProperty == iPropertyType, User::Panic( KSensrvPropertyPanic, 0 ));

    aMaxValue = iIntValueMax;
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::GetMinValue()
// ---------------------------------------------------------------------------
//
EXPORT_C void TSensrvProperty::GetMinValue( TInt& aMinValue ) const
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::GetMinValue return %d" ), iIntValueMin ) );

    __ASSERT_ALWAYS( ESensrvIntProperty == iPropertyType, User::Panic( KSensrvPropertyPanic, 0 ));

    aMinValue = iIntValueMin;
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::SetMaxValue
// ---------------------------------------------------------------------------
//
EXPORT_C void TSensrvProperty::SetMaxValue( TInt aMaxValue )
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::SetMaxValue(aMaxValue=%d)" ), aMaxValue ) );

    if( ESensrvUninitializedProperty == iPropertyType )
        {
        iPropertyType = ESensrvIntProperty;
        }
    else
        {
        __ASSERT_ALWAYS( ESensrvIntProperty == iPropertyType, User::Panic( KSensrvPropertyPanic, 0 ) );
        }

    iIntValueMax = aMaxValue;

    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::SetMaxValue - return void" ) ) );
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::SetMinValue()
// ---------------------------------------------------------------------------
//
EXPORT_C void TSensrvProperty::SetMinValue( TInt aMinValue )
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::SetMinValue(aMinValue= %d" ), aMinValue ) );

    if( ESensrvUninitializedProperty == iPropertyType )
        {
        iPropertyType = ESensrvIntProperty;
        }
    else
        {
        __ASSERT_ALWAYS( ESensrvIntProperty == iPropertyType, User::Panic( KSensrvPropertyPanic, 0 ));
        }

    iIntValueMin = aMinValue;

    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::SetMinValue - return void" ) ) );
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::GetMaxValue()
// ---------------------------------------------------------------------------
//
EXPORT_C void TSensrvProperty::GetMaxValue( TReal& aMaxValue ) const
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::GetMaxValue return %e" ), iRealValueMax ) );

    //__ASSERT_ALWAYS( ESensrvRealProperty == iPropertyType, User::Panic( KSensrvPropertyPanic, 0 ));

    if( ESensrvRealProperty == iPropertyType )
        aMaxValue = iRealValueMax;
    else if ( ESensrvIntProperty == iPropertyType )
        aMaxValue = iIntValueMax;
    
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::GetMinValue()
// ---------------------------------------------------------------------------
//
EXPORT_C void TSensrvProperty::GetMinValue( TReal& aMinValue ) const
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::GetMinValue return %e" ), iRealValueMin ) );

    //__ASSERT_ALWAYS( ESensrvRealProperty == iPropertyType, User::Panic( KSensrvPropertyPanic, 0 ));
    if( ESensrvRealProperty == iPropertyType )
        aMinValue = iRealValueMin;
    else if( ESensrvIntProperty == iPropertyType )
        aMinValue = iIntValueMin;
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::SetMaxValue
// ---------------------------------------------------------------------------
//
EXPORT_C void TSensrvProperty::SetMaxValue( const TReal& aMaxValue )
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::SetMaxValue(aMaxValue=%e" ), aMaxValue ) );

    if( ESensrvUninitializedProperty == iPropertyType )
        {
        iPropertyType = ESensrvRealProperty;
        }
    else
        {
        __ASSERT_ALWAYS( ESensrvRealProperty == iPropertyType, User::Panic( KSensrvPropertyPanic, 0 ));
        }

    iRealValueMax = aMaxValue;

    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::SetMaxValue - return void" ) ) );
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::SetMinValue
// ---------------------------------------------------------------------------
//
EXPORT_C void TSensrvProperty::SetMinValue( const TReal& aMinValue )
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::SetMinValue(aMinValue=%e" ), aMinValue ) );

    if( ESensrvUninitializedProperty == iPropertyType )
        {
        iPropertyType = ESensrvRealProperty;
        }
    else
        {
        __ASSERT_ALWAYS( ESensrvRealProperty == iPropertyType, User::Panic( KSensrvPropertyPanic, 0 ));
        }

    iRealValueMin = aMinValue;

    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::SetMinValue - return void" ) ) );
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::SetSecurityInfo
// ---------------------------------------------------------------------------
//
EXPORT_C void TSensrvProperty::SetSecurityInfo( const TSecurityInfo& aSecurityInfo )
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::SetSecurityInfo()" ) ) );
    iSecurityInfo = aSecurityInfo;
    }

// ---------------------------------------------------------------------------
// TSensrvProperty::GetSecurityInfo()
// ---------------------------------------------------------------------------
//
EXPORT_C TSecurityInfo TSensrvProperty::GetSecurityInfo() const
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::GetSecurityInfo()" ) ) );
    return iSecurityInfo;
    }
    
// ---------------------------------------------------------------------------
// TSensrvProperty::SetArrayIndex()
// ---------------------------------------------------------------------------
//
EXPORT_C void TSensrvProperty::SetArrayIndex( const TInt aArrayIndex )
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::SetArrayIndex(%d)"), aArrayIndex ) );
    iArrayIndex = aArrayIndex;
    }
   
// ---------------------------------------------------------------------------
// TSensrvProperty::GetArrayIndex()
// ---------------------------------------------------------------------------
//
EXPORT_C TInt TSensrvProperty::GetArrayIndex() const
    {
    API_TRACE( ( _L( "Sensor Util - TSensrvProperty::GetArrayIndex() - return %d"), iArrayIndex ) );
    return iArrayIndex;
    }

// End of file
