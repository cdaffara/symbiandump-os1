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
* Description:  Property class of Orientation SSY
*
*/


// Includes
#include "SsyConfiguration.h"
#include "SsyProperty.h"
#include "SsyTrace.h"


// ----------------------------------------------------------------------------------
// FindMatchingProperty()
// Helper function for find a matching property in an array.
// ----------------------------------------------------------------------------------
// 
TBool FindMatchingProperty
        ( const TSSyChannelProperties& aLeft,
          const TSSyChannelProperties& aRight )
	{
    if ( ( aLeft.iPropertyId == aRight.iPropertyId ) &&
         ( aLeft.iItemIndex == aRight.iItemIndex ) )
        {
        return ETrue;
        }

	return EFalse;
	}

// ----------------------------------------------------------------------------------
// CSSYProperty::CSSYProperty()
// ----------------------------------------------------------------------------------
//  
CSSYProperty::CSSYProperty( const TInt /*aSensorNumber*/, 
                            const TSSyChannelProperties* aProperties, 
                            const TInt aNumberOfProperties )
    {
    SSY_TRACE_IN();

    for ( TInt index = 0; index < aNumberOfProperties ; index++ )
        {
        if(iChannelProperties.Append( aProperties[ index ])!= KErrNone )
        {
         iChannelProperties.Reset();
         break;
        }
        }

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYProperty::~CSSYProperty()
// ----------------------------------------------------------------------------------
// 
CSSYProperty::~CSSYProperty()
    {
    SSY_TRACE_IN();

    iRegisteredChannels.Reset();
    iChannelProperties.Reset();
    iChannelProperties.Close();
    iRegisteredChannels.Close();

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYProperty::RegisterChannel()
// ----------------------------------------------------------------------------------
//  
TInt CSSYProperty::RegisterChannel( const TSensrvChannelId aChannelID )
    {
    SSY_TRACE_IN();
    SSY_TRACE( EExtended, "ORIENTATIONSSY:Register channelId %d", aChannelID );

    for ( TInt index = 0; index < iRegisteredChannels.Count(); index++ )
        {
        if ( iRegisteredChannels[index] == aChannelID )
            {
            return KErrAlreadyExists;
            }
        }

    iRegisteredChannels.Append( aChannelID );

    SSY_TRACE_OUT();
    return KErrNone;
    }

// ----------------------------------------------------------------------------------
// CSSYProperty::FindPropertyIndex()
// ----------------------------------------------------------------------------------
//  
TInt CSSYProperty::FindPropertyIndex( const TSensrvProperty& aProperty )
    {
    SSY_TRACE_IN();

    // find the property
    TIdentityRelation<TSSyChannelProperties> findFunction( FindMatchingProperty );
    TSSyChannelProperties property;
    property.iPropertyId = aProperty.GetPropertyId();
    property.iItemIndex = aProperty.PropertyItemIndex();
    property.iPropertyType = aProperty.PropertyType();

    SSY_TRACE( EExtended, "ORIENTATIONSSY:Find property: id:%d index:%d type:%d", property.iPropertyId, property.iItemIndex, property.iPropertyType );

    TInt index = iChannelProperties.Find( property, findFunction );

    SSY_TRACE_OUT();
    return index;
    }

// ----------------------------------------------------------------------------------
// CSSYProperty::GetProperty()
// ----------------------------------------------------------------------------------
//  
TInt CSSYProperty::GetProperty( TSensrvProperty& aProperty )
    {
    SSY_TRACE_IN();

    TInt returnValue( KErrNone );
    TInt index = FindPropertyIndex( aProperty );
    
    if ( index == KErrNotFound )
        {
        SSY_TRACE( EExtended, "ORIENTATIONSSY:Property not found" );
        returnValue = KErrNotFound;
        }
    else if( iChannelProperties.Count() >= index )
        {
        switch ( iChannelProperties[ index ].iPropertyType )
            {
            case ESensrvIntProperty:
                {
                TSensrvProperty property
                    (
                    iChannelProperties[ index ].iPropertyId,
                    iChannelProperties[ index ].iItemIndex,
                    iChannelProperties[ index ].iIntValue,
                    iChannelProperties[ index ].iIntMaxValue,
                    iChannelProperties[ index ].iIntMinValue,
                    iChannelProperties[ index ].iReadOnly,
                    iChannelProperties[ index ].iPropertyType
                    );
                aProperty = property;
                break;
                }
            case ESensrvRealProperty:
                {
                TSensrvProperty property
                    (
                    iChannelProperties[ index ].iPropertyId,
                    iChannelProperties[ index ].iItemIndex,
                    iChannelProperties[ index ].iRealValue,
                    iChannelProperties[ index ].iRealMaxValue,
                    iChannelProperties[ index ].iRealMinValue,
                    iChannelProperties[ index ].iReadOnly,
                    iChannelProperties[ index ].iPropertyType
                    );
                aProperty = property;
                break;
                }
            case ESensrvBufferProperty:
                {
                TSensrvProperty property
                    (
                    iChannelProperties[ index ].iPropertyId,
                    iChannelProperties[ index ].iItemIndex,
                    iChannelProperties[ index ].iBufValue,
                    iChannelProperties[ index ].iReadOnly,
                    iChannelProperties[ index ].iPropertyType
                    );
                aProperty = property;
                break;
                }
            default:
                {
                SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: Invalid propertype type!" );
                ASSERT_DEBUG_SSY( 0 );
                returnValue = KErrGeneral;
                break;
                }
            }
        }

    SSY_TRACE_OUT();
    return returnValue;
    }

// ----------------------------------------------------------------------------------
// CSSYProperty::GetAllProperties()
// ----------------------------------------------------------------------------------
//  
TInt CSSYProperty::GetAllProperties( RSensrvPropertyList& aChannelPropertyList )
    {
    SSY_TRACE_IN();
    TInt returnValue( KErrNone );
    aChannelPropertyList.Reset();
    
    for ( TInt index = 0; index < iChannelProperties.Count(); index++ )
        {
        switch ( iChannelProperties[ index ].iPropertyType )
            {
            case ESensrvIntProperty:
                {
                TSensrvProperty property
                    (
                    iChannelProperties[ index ].iPropertyId,
                    iChannelProperties[ index ].iItemIndex,
                    iChannelProperties[ index ].iIntValue,
                    iChannelProperties[ index ].iIntMaxValue,
                    iChannelProperties[ index ].iIntMinValue,
                    iChannelProperties[ index ].iReadOnly,
                    iChannelProperties[ index ].iPropertyType
                    );
                aChannelPropertyList.Append( property );
                break;
                }
            case ESensrvRealProperty:
                {
                TSensrvProperty property
                    (
                    iChannelProperties[ index ].iPropertyId,
                    iChannelProperties[ index ].iItemIndex,
                    iChannelProperties[ index ].iRealValue,
                    iChannelProperties[ index ].iRealMaxValue,
                    iChannelProperties[ index ].iRealMinValue,
                    iChannelProperties[ index ].iReadOnly,
                    iChannelProperties[ index ].iPropertyType
                    );
                aChannelPropertyList.Append( property );
                break;
                }
            case ESensrvBufferProperty:
                {
                TSensrvProperty property
                    (
                    iChannelProperties[ index ].iPropertyId,
                    iChannelProperties[ index ].iItemIndex,
                    iChannelProperties[ index ].iBufValue,
                    iChannelProperties[ index ].iReadOnly,
                    iChannelProperties[ index ].iPropertyType
                    );
                aChannelPropertyList.Append( property );
                break;
                }
            default:
                {
                SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: Invalid property type!" );
                ASSERT_DEBUG_SSY( 0 );
                returnValue = KErrGeneral;
                break;
                }
            }
        }

    SSY_TRACE_OUT();
    return returnValue;
    }

// ----------------------------------------------------------------------------------
// CSSYProperty::SetProperty()
// ----------------------------------------------------------------------------------
//  
TInt CSSYProperty::SetProperty( 
    const TSensrvProperty& aProperty, 
    RSensrvChannelList& aAffectedChannels )
    {
    SSY_TRACE_IN();

    TInt intValue( 0 );
    TReal realValue( 0 );
    TInt returnValue( KErrNone );
    TInt index = FindPropertyIndex( aProperty );
    aAffectedChannels.Reset();
    
    if ( index == KErrNotFound )
        {
        SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: Property not found: 0x%x", aProperty.GetPropertyId() );
        returnValue = KErrNotFound;
        }
    else if ( iChannelProperties[ index ].iReadOnly )
        {
        SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: Trying to set a readonly property: 0x%x", aProperty.GetPropertyId() );
        returnValue = KErrGeneral;
        }

    else if ( aProperty.GetPropertyId() == KSensrvPropIdDataRate )
        {
        // Since Orientation is an event, the data rate property is made readonly.
        // In ssyconfiguration.h file, the datarate property is defined as writable,
        // it is not modified due to backward compatibility. 
        // Now it is changed and the orientation channel property is made readonly always.
        
        if( iChannelProperties.Count() >= index )
            {
            iChannelProperties[index].iReadOnly = ETrue;
            }
        
        // No need to get data rate anymore as it's already got
        iDataRateUpdate = ETrue;
        }
    else if( iChannelProperties.Count() >= index )
        {
        switch ( iChannelProperties[index].iPropertyType )
            {
            case ESensrvIntProperty:
                {
                aProperty.GetValue( intValue );

                if ( iChannelProperties[ index ].iIntValue == intValue )
                    {
                    SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: Trying to set a property to a same value that it already has: 0x%x", aProperty.GetPropertyId() );
                    returnValue = KErrAlreadyExists;
                    }
                else
                    {
                    iChannelProperties[ index ].iIntValue = intValue;
                    }
                break;
                }
            case ESensrvRealProperty:
                {
                aProperty.GetValue( realValue );
                iChannelProperties[ index ].iRealValue = realValue;
                break;
                }
            case ESensrvBufferProperty:
            default:
                {
                SSY_TRACE( EError, "ORIENTATIONSSY:ERROR: Trying to set a buffer property value, this is not supported at the moment: 0x%x", aProperty.GetPropertyId() );
                // no buffer value can be set at the moment, and other values are not supported
                returnValue = KErrArgument;
                break;
                }
            }

        if ( KErrNone == returnValue )
            {
            for ( TInt index = 0; index < iRegisteredChannels.Count(); index++ )
                {
                aAffectedChannels.Append( iRegisteredChannels[index] );
                }
            }
        }

    SSY_TRACE_OUT();
    return returnValue;
    }

// ----------------------------------------------------------------------------------
// CSSYProperty::GetAffectedChannels()
// ----------------------------------------------------------------------------------
//  
void CSSYProperty::GetAffectedChannels( RSensrvChannelList& aAffectedChannels )
    {
    SSY_TRACE_IN();

    aAffectedChannels.Reset();
    
    for ( TInt index = 0; index < iRegisteredChannels.Count(); index++ )
        {
        aAffectedChannels.Append( iRegisteredChannels[index] );
        }

    SSY_TRACE_OUT();
    }

// ----------------------------------------------------------------------------------
// CSSYProperty::DataRateUpdated()
// ----------------------------------------------------------------------------------
//  
TBool CSSYProperty::DataRateUpdated()
    {
    return iDataRateUpdate;
    }

// End of File
