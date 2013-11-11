// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : CTlv.cpp
// Part of     : Series 60 / utility
// Based on 3GPP TS 11.14 V8.8.0 
// Version     : 1.0
// EXTERNAL RESOURCES  
//



//  Include Files  

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ctlvTraces.h"
#endif

#include "CTlv.h"				    // Header of this class
#include "TSatUtility.h"			// Utility methods

// -----------------------------------------------------------------------------
// CTlvBase::CTlvBase
// Default constructor
// -----------------------------------------------------------------------------
//
CTlvBase::CTlvBase
        ( 
        // None
        ) : iData(0,0)
    {
    // Do nothing.
    }

// -----------------------------------------------------------------------------
// CTlvBase::Data
// return iData Pointer to TLV data
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CTlvBase::Data
        ( 
        // None
        ) const
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CTLVBASE_DATA_1, "UTILITY: CTlvBase::Data");
    return iData;
    }


// -----------------------------------------------------------------------------
// CTlvBase::SetData
// Set TLV data
// -----------------------------------------------------------------------------
//
EXPORT_C void CTlvBase::SetData
        ( 
        TPtrC8 aData
        )
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CTLVBASE_SETDATA_1, "UTILITY: CTlvBase::SetData");
    iData.Set( aData );
    }

// -----------------------------------------------------------------------------
// CTlvBase::GetSize
// Get total size, including tag
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CTlvBase::GetSize
        ( 
        // None
        ) const
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CTLVBASE_GETSIZE_1, "UTILITY: CTlvBase::GetSize");
    return iData.Length();
    }

// -----------------------------------------------------------------------------
// CTlvBase::GetTag
// Get tag part of TLV
// -----------------------------------------------------------------------------
//
EXPORT_C TUint8 CTlvBase::GetTag
        ( 
        // None
        ) const
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CTLVBASE_GETTAG_1, "UTILITY: CTlvBase::GetTag");
    return TUint8(iData[0] & KTagValueMask);
    }

// -----------------------------------------------------------------------------
// CTlvBase::GetComprehensionRequired
// Returns true if comprehension flag is on
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CTlvBase::GetComprehensionRequired
        ( 
        // None
        ) const
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CTLVBASE_GETCOMPREHENSIONREQUIRED_1, "UTILITY: CTlvBase::GetComprehensionRequired");
    return (iData[0] & KTagCrMask) ? ETrue : EFalse;
    }

// -----------------------------------------------------------------------------
// CTlvBase::GetLength
// Get length part of TLV
// -----------------------------------------------------------------------------
//
EXPORT_C TUint8 CTlvBase::GetLength
        ( 
        // None
        ) const
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CTLVBASE_GETLENGTH_1, "UTILITY: CTlvBase::GetLength");
    return (KTwoByteLengthCoding == iData[KTlvLengthStartPosition]) ?
        iData[KTlvLengthStartPosition + 1] :
        iData[KTlvLengthStartPosition];
    }

// -----------------------------------------------------------------------------
// CTlvBase::GetValue
// Get value part of TLV
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CTlvBase::GetValue
        ( 
        // None
        ) const
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CTLVBASE_GETVALUE_1, "UTILITY: CTlvBase::GetValue");
    TInt offset = (KTwoByteLengthCoding == iData[1]) ? 1 : 0;
    return iData.Mid(2+offset, iData[1+offset]);
    }

// -----------------------------------------------------------------------------
// CTlv::CTlv
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CTlv::CTlv
        (
        // None
        )
    {
    // None
    }

// -----------------------------------------------------------------------------
// CTlv::GetShortInfo
// Returns the short info (one byte). Returned data depends on data type.
// -----------------------------------------------------------------------------
//
EXPORT_C TUint8 CTlv::GetShortInfo
        (
        TTlvSpesificDataType aType // Info spesific data type
        )    
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CTLV_GETSHORTINFO_1, "UTILITY: CTlv::GetShortInfo, type: %d", aType);
    // Information is generally at index 2.
    TInt ind ( KTlvDataAreaStartPosition ); 
    TUint8 offset( 0 );

    // First determine if the length of the TLV is coded with 1 or 2 bytes to
    // determine the offset
    if ( KTwoByteLengthCoding == iData[KTlvLengthStartPosition] )
        {
        offset = 1;
        }

    // Start getting the contents of TLV data
    switch( aType )
        {
        case ETLV_SourceDeviceIdentity:
        case ETLV_TimeUnit:
        case ETLV_IdentifierOfItemChosen:
        case ETLV_MinimumLengthOfResponse: 
        case ETLV_Tone: 
        case ETLV_LocationStatus:  
        case ETLV_IconQualifier: 
        case ETLV_IconListQualifier:
        case ETLV_CardReaderStatus: 
        case ETLV_TimerIdentifier:
        case ETLV_BcRepeatIndicatorValues:
        case ETLV_MeStatus:  
        case ETLV_BrowserIdentity: 
        case ETLV_BrowserTerminationCause:
        case ETLV_BearerType:
        case ETLV_ChannelDataLength:
        case ETLV_TypeOfAddress:
        case ETLV_TransportProtocolType:                                                                                                                       
            {
            ind = KTlvDataAreaStartPosition;
            break;	
            }  
        case ETLV_DestinationDeviceIdentity:
        case ETLV_TimeInterval:
        case ETLV_MaximumLengthOfResponse:
        case ETLV_IconIdentifier:   
        case ETLV_TimingAdvance:                             
            {
            ind = KTlvDataAreaStartPosition + 1;
            break;	
            }                                                           
        case ETLV_TonAndNpi:
        case ETLV_CommandNumber:
        case ETLV_IdentifierOfItem:
        case ETLV_GeneralResult:
        case ETLV_DataCodingScheme: 
        case ETLV_NumberOfFiles: 
        case ETLV_CommandClassCLA:                                              
            {
            ind = KTlvDataAreaStartPosition + offset;
            break;	
            }
  	
        case ETLV_TypeOfCommand:
        case ETLV_CommandInstructionCodeIns:        
            {
            ind = KTlvDataAreaStartPosition + offset + 1;
            break;	
            } 
        case ETLV_CommandQualifier:
        case ETLV_P1Parameter:        
            {
            ind = KTlvDataAreaStartPosition + offset + 2;
            break;	
            }    	                                                                                                                     
        case ETLV_P2Parameter:
            {
            ind = KTlvDataAreaStartPosition + offset + 3;
            break;	
            }                                      
        case ETLV_Lc:
            {
            if ( GetLongInfo( ETLV_Lc ) > KCApduLengthWithoutLc ) 
                {
                ind = KTlvDataAreaStartPosition + offset + 4;
                }
            else
                {
                return 0;
                }
            break;	
            }    
        case ETLV_Le:
            {
            ind = KTlvDataAreaStartPosition + offset + 5 + GetShortInfo( ETLV_Lc );
            break;	
            }    
        case ETLV_StatusWordSW1:
            {
            ind = KTlvDataAreaStartPosition + offset + GetLength() - 2 + 1;
            break;	
            }    
        case ETLV_StatusWordSW2:
            {
            ind = KTlvDataAreaStartPosition + offset + GetLength() - 2 + 2;
            break;	
            } 
        default:
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CTLV_GETSHORTINFO_2, "UTILITY: CTlv::GetShortInfo, Type unknown");
            break;	
            }                                                                                                                                                              	
        }

    return iData[ind];
    }

// -----------------------------------------------------------------------------
// CTlv::GetData
// Returns the data. Returned data depends on data type.
// -----------------------------------------------------------------------------
//
EXPORT_C  TPtrC8 CTlv::GetData
        (
        TTlvSpesificDataType aType //Info spesific data type
        )    
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CTLV_GETDATA_1, "UTILITY: CTlv::GetData, Data length: %d", iData.Length());
    // Information is generally at index 2.
    TInt ind ( 2 ); 
    TUint8 offset( 0 );
    TInt length( 1 );

    // First determine if the length of the TLV is coded with 1 or 2 bytes.
    if ( KTwoByteLengthCoding == iData[KTlvLengthStartPosition] )
        {
        offset = 1;
        }

    // Start getting TLV data elements
    switch( aType )
        {
        case ETLV_CellBroadcastPage:
        case ETLV_Imei:   
        case ETLV_NetworkMeasurementResults:
        case ETLV_ItemsNextActionIndicator:
        case ETLV_BcchChannelList:
        case ETLV_Atr:  
        case ETLV_TimerValue:
        case ETLV_DateTimeAndTimeZone:
        case ETLV_IdentifierOfCardReader:
        case ETLV_NetworkAccessName:
        case ETLV_AID:   
        case ETLV_TransactionIdentifier:                                                                                                                                                                                                                            
            {
            ind = KTlvDataAreaStartPosition;
            length = GetLength();
            break;	
            }
        case ETLV_MccAndMnc:                                                                                                                       
            {
            ind = KTlvDataAreaStartPosition;
            length = KMccAndMncLength;
            break;	
            } 
        case ETLV_IconIdentifierList:
        case ETLV_Address:                                                                                                                                
            {
            ind = KTlvDataAreaStartPosition + 1;
            length = GetLength() - 1;
            break;	
            }                                                   
        case ETLV_AlphaIdentifier:
        case ETLV_SubAddress:
        case ETLV_CapabilityConfigurationParameters:
        case ETLV_SmsTPdu:
        case ETLV_EventList:
        case ETLV_Cause:
        case ETLV_CallControlRequestedAction: 
        case ETLV_AtCommand: 
        case ETLV_AtResponse:
        case ETLV_DtmfString: 
        case ETLV_Url: 
        case ETLV_ListOfBearers:
        case ETLV_PathToProvisioningFile:
        case ETLV_ChannelDataString:                                                                                                                                                                  
            {
            ind = KTlvDataAreaStartPosition + offset;
            length = GetLength();
            break;	
            } 
        case ETLV_RApduData:                                                                                                                       
            {
            ind = KTlvDataAreaStartPosition + offset;
            length = GetLength() - 2;            
            break;	
            }             
        case ETLV_AdditionalInformationOnResult:
        case ETLV_SsOrUssdString:
        case ETLV_TextString: 
        case ETLV_DefaultText:
        case ETLV_UssdString:
        case ETLV_Files:
        case ETLV_BearerParameters:
        case ETLV_TextStringOfItem:
        case ETLV_DiallingNumberString:                                                                                                                                                                                      
            {
            ind = KTlvDataAreaStartPosition + offset + 1;
            length = GetLength() - 1;
            break;	
            }
        case ETLV_Data:                                                                                                                       
            {
            TUint8 dataLength( GetShortInfo( ETLV_Lc ) );
            if ( dataLength )
                {
                ind = KTlvDataAreaStartPosition + offset + 5;
                length = dataLength;
                }
            else
                {
                OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CTLV_GETDATA_2, "UTILITY: CTlv::GetData, Data length 0");
                return 0;
                }            
            break;	
            }
        default:
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CTLV_GETDATA_3, "UTILITY: CTlv::GetData, Type unknown");
            break;	
            } 
        }

    OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CTLV_GETDATA_4, "UTILITY: CTlv::GetData, length: %d, ind: %d", length, ind);
    return iData.Mid( ind, length );
    }

// -----------------------------------------------------------------------------
// CTlv::GetLongInfo
// Returns the data. Returned data depends on data type.
// -----------------------------------------------------------------------------
//
EXPORT_C  TUint16 CTlv::GetLongInfo
        ( 
        TTlvSpesificDataType aType // Info spesific data type
        )
    {
    OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CTLV_GETLONGINFO_1, "UTILITY: CTlv::GetLongInfo, type: %d", aType);
    TUint16 ret( 0x0000 );

    switch( aType )
        { 
        case ETLV_LocationAreaCode:                                                                                                                                                                                                                            
            {
            TSatUtility::CopyTwo8toOne16LE( iData, ret, 
                KTlvLengthStartPosition + KMccAndMncLength );
            break;	
            }
        case ETLV_CellIdentityValue:                                                                                                                                                                                                                            
            {
            TSatUtility::CopyTwo8toOne16LE( iData, ret,  
                KTlvLengthStartPosition + KMccAndMncLength + 
                KLocationAreaCodeLength );
            break;	
            }            
        case ETLV_Language:                                                                                                                                                                                                                            
            {
            TSatUtility::CopyTwo8toOne16LE( iData, ret,  
            	KTlvDataAreaStartPosition ); 
            break;	
            }    
        case ETLV_BufferSize:                                                                                                                                                                                                                            
            {
            TSatUtility::CopyTwo8toOne16LE( iData, ret,  
                KTlvDataAreaStartPosition );
            break;	
            }    
        case ETLV_ChannelStatus:                                                                                                                                                                                                                            
            {
            TSatUtility::CopyTwo8toOne16LE( iData, ret,  
                KTlvLengthStartPosition );
            break;	
            }    
        case ETLV_PortNumber:                                                                                                                                                                                                                            
            {
            TSatUtility::CopyTwo8toOne16LE( iData, ret,  
                KTlvDataAreaStartPosition + 1 );
            break;	
            }   
        default:
            {
            OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CTLV_GETLONGINFO_2, "UTILITY: CTlv::GetLongInfo, Type unknown");
            break;
            }
        }

    return ret;
    }  

//End Of File
