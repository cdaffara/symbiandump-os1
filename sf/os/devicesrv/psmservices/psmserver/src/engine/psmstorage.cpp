/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  PSM storage for backups and configs
*
*/


#include "psmstorage.h"
#include "psmstoragedefines.h"
#include "psmtrace.h"

// -----------------------------------------------------------------------------
// CPsmStorage::CPsmStorage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsmStorage::CPsmStorage( TPsmsrvMode aMode, RFs& aFile ) :
    iFileSession( aFile ),
    iMode( aMode )
    {
    }

// -----------------------------------------------------------------------------
// CPsmStorage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPsmStorage::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::ConstructL()" ) ) );
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::ConstructL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CPsmStorage::~CPsmStorage()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::~CPsmStorage()" ) ) );

    if( iStorageFile.NotNull() )
        {
        iStorageFile.Close( );
        }
    iStorageParser.Close( );
    iDOMImpl.Close( );    

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::~CPsmStorage - return") ) );
    }

// -----------------------------------------------------------------------------
// CPsmBackupStorage::InitRootElementsL
// -----------------------------------------------------------------------------
//
void CPsmStorage::InitRootElementsL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::InitRootElementsL()" ) ) );
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::InitRootElementsL() - return" ) ) );
    }

void CPsmStorage::BackupConfigurationL( const RConfigInfoArray& /*aConfigArray*/ )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::BackupConfigurationL()" ) ) );
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CPsmStorage::CloseStorageL
// -----------------------------------------------------------------------------
//
void CPsmStorage::CloseStorageL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::CloseStorageL()" ) ) );

    // Cleanup storage elements
    if( iStorageFile.NotNull() )
        {
        iStorageFile.Close( );
        }

    iStorageParser.Close( );
    iDOMImpl.Close( );
        
    // Zero storage UID
    iStorageUid = 0;

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::CloseStorageL() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmStorage::FindConfigurationSet
// -----------------------------------------------------------------------------
//
TXmlEngElement CPsmStorage::FindConfigurationSetL( const TInt aMode )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::FindConfigurationSet()" ) ) );

    // Search correct settings for aUid
    TBool setFound( EFalse );
    TXmlEngElement settings;
    RXmlEngNodeList<TXmlEngElement> nodeList;   
    CleanupClosePushL(nodeList);

    if ( iStorageRoot.NotNull() )
        {
        iStorageRoot.GetElementsByTagNameL(nodeList, KPsmPluginGroupTag);
        settings = nodeList.Next().AsElement();
        }
    else
        {
        COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::FindConfigurationSet() - Storage not initialized! LEAVE" ) ) );
        User::Leave( KErrNotFound );
        }
    
    // Loop until correct set is found
    while ( settings.NotNull() && !setFound )
        {
        // Read mode from storage
        TInt mode = GetAttributeIntValueL( settings, KPsmPluginGroupAttribute );

        if ( aMode == mode )
            {
            // Set found, no need to loop anumore
            setFound = ETrue;
            }
        else
            {
            // Set not found, continue loop with next config set
            settings = nodeList.Next().AsElement();            
            }
        }
    CleanupStack::PopAndDestroy(&nodeList);
    if ( !setFound )
        {
        COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::FindConfigurationSet() - Mode not found: %i, LEAVE" ), aMode ) );
        User::Leave( KErrNotFound );
        }

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::FindConfigurationSet() - return" ) ) );
    return settings;
    }

// -----------------------------------------------------------------------------
// CPsmStorage::CreateConfigArrayL
// -----------------------------------------------------------------------------
//
void CPsmStorage::CreateConfigArrayL( RConfigInfoArray& aPsmConfigArray, const TInt aMode )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::CreateConfigArrayL()" ) ) );
    // Read configuration information from config files and generate config array    

    // Search correct settings for current mode
    TXmlEngElement settings = FindConfigurationSetL( aMode );

    // Get first setElemens
    RXmlEngNodeList<TXmlEngElement> nodeList;
    CleanupClosePushL(nodeList);
    settings.GetElementsByTagNameL(nodeList, KPsmSetItem);
    TXmlEngElement setElement = nodeList.Next().AsElement();

    // Loop through all set elements and update values to array
    while( setElement.NotNull() )
        {
        TPsmsrvConfigInfo configInfo;
        configInfo.iConfigId = GetAttributeIntValueL( setElement, KPsmSetItemKey );
        configInfo.iConfigType = (TPsmsrvConfigTypes)GetAttributeIntValueL( setElement,
                                                                           KPsmSetItemType );
        switch ( configInfo.iConfigType )
            {
            case EConfigTypeInt:
                {
                configInfo.iIntValue = 
                    GetAttributeIntValueL( setElement, KPsmSetItemValue );
                break;
                }
            case EConfigTypeReal:
                {
                configInfo.iRealValue = 
                    GetAttributeRealValueL( setElement, KPsmSetItemValue );
                break;
                }
            case EConfigTypeBuf:
                {
                GetAttributeStrValueL( setElement,
                                       KPsmSetItemValue,
                                       configInfo.iDesValue );
                break;
                }
            default:
                COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::CreateConfigArrayL() - Unknown type" ) ) );
            }

        // Add info to list
        aPsmConfigArray.AppendL( configInfo );

        // handle next element set
        setElement = nodeList.Next().AsElement();
        }
    CleanupStack::PopAndDestroy(&nodeList);
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::CreateConfigArrayL() - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmStorage::GetConfigurationL
// -----------------------------------------------------------------------------
//
void CPsmStorage::GetConfigurationL( RConfigInfoArray& aConfigArray )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::GetConfigurationL()" ) ) );

    // This is used for getting PSM configuration and for restoring original settings
    const TInt configCount( aConfigArray.Count() );

    // Search correct settings for current mode
    TXmlEngElement settings = FindConfigurationSetL( iMode );    

    // Get first setElemens
    RXmlEngNodeList<TXmlEngElement> nodeList;
    CleanupClosePushL(nodeList);
    settings.GetElementsByTagNameL(nodeList, KPsmSetItem);
    TXmlEngElement setElement = nodeList.Next().AsElement();

    // Loop through all set elements and update values to array
    while( setElement.NotNull() )
        {
        const TInt setKey = GetAttributeIntValueL( setElement, KPsmSetItemKey );
        const TInt setType = GetAttributeIntValueL( setElement, KPsmSetItemType );
        const TInt blocked = GetAttributeIntValueL( setElement, KPsmSetItemBlock );

        TBool configUpdated( EFalse );

        // Loop configurations and update values to aConfigArray (do not replace, just update value)
        // If some values are not found, they remains the same
        for ( TInt i = 0; i < configCount && !configUpdated; i++ )
            {
            TPsmsrvConfigInfo& configInfo = aConfigArray[i];

            // check keys
            if ( setKey == configInfo.iConfigId )
                {
                // Add blocked information
                configInfo.iBlocked = blocked;

                // correct config found, update value
                switch ( setType )
                    {
                    case EConfigTypeInt:
                        {
                        configInfo.iIntValue = 
                            GetAttributeIntValueL( setElement, KPsmSetItemValue );                        
                        break;
                        }
                    case EConfigTypeReal:
                        {
                        configInfo.iRealValue = 
                            GetAttributeRealValueL( setElement, KPsmSetItemValue );
                        break;
                        }
                    case EConfigTypeBuf:
                        {
                        GetAttributeStrValueL( setElement,
                                               KPsmSetItemValue,
                                               configInfo.iDesValue );
                        break;
                        }
                    default:
                        {
                        COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::GetConfigurationL() - Unknown type LEAVE" ) ) );
                        User::Leave( KErrUnknown );
                        }
                    }
                // No need to loop config array anymore with this set element
                configUpdated = ETrue;
                }
            }
        // handle next element set
        setElement = nodeList.Next().AsElement();
        }

    CleanupStack::PopAndDestroy(&nodeList);
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::GetConfigurationL() - return" ) ) );
    }

// ---------------------------------------------------------------------------
// CPsmStorage::GetAttributeIntValueL
// ---------------------------------------------------------------------------
//
TInt CPsmStorage::GetAttributeIntValueL( const TXmlEngElement& aElement, const TDesC8& aAttrib ) const
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::GetAttributeIntValueL()" ) ) );
    COMPONENT_TRACE( ( _L( "  Attribute: %S" ), &aAttrib ) );

    TInt intValue( 0 );

    // Check availability
    if ( aElement.AttributeNodeL( aAttrib ).NotNull() )
        {
        // Read attribute value
        TPtrC8 ptr = aElement.AttributeValueL( aAttrib );

        // Cast literal value into TInt
        TLex8 lexValue( ptr );
        lexValue.Val( intValue );
        }

    COMPONENT_TRACE( ( _L( "  IntValue: %i" ), intValue ) );
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::GetAttributeIntValueL() - return" ) ) );
    return intValue;
    }

// ---------------------------------------------------------------------------
// CPsmStorage::GetAttributeStrValueL
// ---------------------------------------------------------------------------
//
void CPsmStorage::GetAttributeStrValueL(
    const TXmlEngElement& aElement, const TDesC8& aAttrib, TDes8& aTarget ) const
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::GetAttributeStrValueL()" ) ) );
    COMPONENT_TRACE( ( _L( "  Attribute: %S" ), &aAttrib ) );

    // Check availability
    if ( aElement.AttributeNodeL( aAttrib ).NotNull() )
        {
        aTarget = aElement.AttributeValueL( aAttrib );
        }

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::GetAttributeStrValueL() - return" ) ) );
    }

// ---------------------------------------------------------------------------
// CPsmStorage::GetAttributeHexValueL
// ---------------------------------------------------------------------------
//
TUint CPsmStorage::GetAttributeHexValueL( const TXmlEngElement& aElement, const TDesC8& aAttrib ) const
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::GetAttributeHexValueL()" ) ) );
    COMPONENT_TRACE( ( _L( "  Attribute: %S" ), &aAttrib ) );

    TUint32 hexValue( 0 );

    // Check availability
    if ( aElement.AttributeNodeL( aAttrib ).NotNull() )
        {
        // Read attribute value
        TPtrC8 ptr = aElement.AttributeValueL( aAttrib );

        // Get bounded value and cast it into TUint32 (hex)
        TRadix radix( EHex );
        TUint limit( 0xFFFFFFFF );

        // Append string into Lex and skip first two characters, 0x
        TLex8 lexValue( ptr );
        lexValue.Inc( 2 );

        // Read value
        lexValue.BoundedVal( hexValue, radix, limit );
        }

    COMPONENT_TRACE( ( _L( "  HexValue: %x" ), hexValue ) );
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::GetAttributeHexValueL() - return" ) ) );
    return hexValue;
    }

// ---------------------------------------------------------------------------
// CPsmStorage::GetAttributeRealValueL
// ---------------------------------------------------------------------------
//
TReal CPsmStorage::GetAttributeRealValueL( const TXmlEngElement& aElement, const TDesC8& aAttrib ) const
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::GetAttributeRealValueL()" ) ) );
    COMPONENT_TRACE( ( _L( "  Attribute: %S" ), &aAttrib ) );

    TReal realValue( 0 );

    // Check availability
    if ( aElement.AttributeNodeL( aAttrib ).NotNull() )
        {
        // Read attribute value
        TPtrC8 ptr = aElement.AttributeValueL( aAttrib );

        // Cast literal value into TReal
        TLex8 lexValue( ptr );
        lexValue.Val( realValue );
        }

    COMPONENT_TRACE( ( _L( "  IntValue: %f" ), realValue ) );
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::GetAttributeRealValueL() - return" ) ) );
    return realValue;
    }

// ---------------------------------------------------------------------------
// CPsmStorage::SetAttributeIntValue
// ---------------------------------------------------------------------------
//
void CPsmStorage::SetAttributeValueL( 
    TXmlEngElement& aElement, const TDesC8& aAttrib, const TInt aValue ) const
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::SetAttributeValueL() - Integer" ) ) );
    COMPONENT_TRACE( ( _L( "  Attribute: %S" ), &aAttrib ) );   
    COMPONENT_TRACE( ( _L( "  Value: %i" ), aValue ) );

    TBuf8<KPsmMaxAttribLength> attributeValue;
    attributeValue.Num( aValue, EDecimal );
    aElement.SetAttributeL( aAttrib, attributeValue );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::SetAttributeValueL() - return" ) ) );
    }

// ---------------------------------------------------------------------------
// CPsmStorage::SetAttributeStrValue
// ---------------------------------------------------------------------------
//
void CPsmStorage::SetAttributeValueL( 
    TXmlEngElement& aElement, const TDesC8& aAttrib, const TDesC8& aValue ) const
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::SetAttributeValueL() - String" ) ) );
    COMPONENT_TRACE( ( _L( "  Attribute: %S" ), &aAttrib ) );
    COMPONENT_TRACE( ( _L( "  Value: %S" ), &aValue ) );

    aElement.SetAttributeL( aAttrib, aValue );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::SetAttributeValueL() - return" ) ) );
    }

// ---------------------------------------------------------------------------
// CPsmStorage::SetAttributeRealValue
// ---------------------------------------------------------------------------
//
void CPsmStorage::SetAttributeValueL( 
    TXmlEngElement& aElement, const TDesC8& aAttrib, const TReal aValue ) const
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::SetAttributeValueL() - Real" ) ) );
    COMPONENT_TRACE( ( _L( "  Attribute: %S" ), &aAttrib ) );
    COMPONENT_TRACE( ( _L( "  Value: %f" ), aValue ) );

    TBuf8<KPsmMaxAttribLength> attributeValue;
    TRealFormat* realFormat = new ( ELeave ) TRealFormat();
    attributeValue.Num( aValue, *realFormat );
    delete realFormat;
    aElement.SetAttributeL( aAttrib, attributeValue );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::SetAttributeValueL() - return" ) ) );
    }

// ---------------------------------------------------------------------------
// CPsmStorage::SetAttributeHexValue
// ---------------------------------------------------------------------------
//
void CPsmStorage::SetAttributeHexValueL( 
    TXmlEngElement& aElement, const TDesC8& aAttrib, const TUint32 aValue ) const
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::SetAttributeHexValueL() - Hex" ) ) );
    COMPONENT_TRACE( ( _L( "  Attribute: %S" ), &aAttrib ) );   
    COMPONENT_TRACE( ( _L( "  Value: 0x%x" ), aValue ) );

    TBuf8<KPsmMaxAttribLength> attributeValue;
    attributeValue.Append( _L("0x") );
    attributeValue.AppendNumUC( aValue, EHex );
    aElement.SetAttributeL( aAttrib, attributeValue );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmStorage::SetAttributeHexValueL() - return" ) ) );
    }

// End of file
