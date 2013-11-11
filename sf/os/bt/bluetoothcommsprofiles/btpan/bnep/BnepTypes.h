//   Read the documentation to learn more about C++ code generator
//   versioning.


// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Reserved
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __BNEPTYPES_H
#define __BNEPTYPES_H 


#include <bttypes.h>


#define UNUSEDPARAMETER1 /* */
#define UNUSEDPARAMETER2 /* */
#define UNUSEDPARAMETER3 /* */
#define UNUSEDPARAMETER4 /* */
#define UNUSEDPARAMETER5 /* */
const TUint KMaxNetworkTypeFilterPairs = 16; 			// the maximum number of network type filters we will support
const TUint KMaxMultiAddrFilterPairs = 16; 				// the maximum number of multicast address filters we will support
const TUint KSizeOfBnepControlType = 1; 				// size in bytes of a BNEP control type header
const TUint KSizeOfBnepResponseCode = 2; 				// size in bytes of a BNEP control response code (all response codes are currently the same length)
const TUint KSizeOfNetTypeFilterListLength = 2; 		// size in bytes of the length member of the BNEP network packet type filter list
const TUint16 KMaxSizeOfNetTypeFilterList = 4*422; 		// maximum size in bytes of a net type filter list outside and extension header
const TUint KMaxSixeOfExtensionNetTypeFilterList = 4*64; // maximum size in bytes of a net type filter list in an exyension header
const TUint KSizeOfNetType = 2; 						// size in bytes of the BNEP network packet type field
const TUint KSizeOfMultiAddrFilterListLength = 2; 		// size in bytes of the length member of the BNEP multicast address filter list
const TUint KSizeOfMultiAddr = KBTDevAddrSize;			// size in bytes of the BNEP multicast address field
const TUint KMaxNetTypeFilters = 10;								// Maximum allowable number of filters
const TUint KNetTypeFilterTableBoundary = 2*KMaxNetTypeFilters;		// Upper boundary of filter table
const TUint KMaxMultiAddrFilters = 10;								// Maximum allowable number of filters
const TUint KMultiAddrFilterTableBoundary = 2*KMaxMultiAddrFilters; // Upper boundary of filter table
const TUint KExtensionHeaderHeaderBytes = 2; 			// Number of header bytes in BNEP extension header
const TUint KSizeOfBnepExtensionType	= 1;			// Size in bytes of the BNEP extension type



typedef enum
    {
    EBnepGeneralEthernet = 0x00,
    EBnepControl = 0x01,
    EBnepCompressedEthernet = 0x02,
    EBnepCompressedEthernetSourceOnly = 0x03,
    EBnepCompressedEthernetDestOnly = 0x04,
    EBnepHighestSigReserved = 0xfe
    // highest future use reserved value for BT SIG use
    // IEEE802.15 group has reserved 0xff
    } TBnepPacketType;




typedef enum
    {
    EBnepExtensionControl = 0x00
    } TBnepExtensionType;




typedef enum
    {
    EBnepControlCommandNotUnderstood = 0x00,
    EBnepSetupConnectionRequestMessage = 0x01,
    EBnepSetupConnectionResponseMessage = 0x02,
    EBnepFilterNetTypeSetMsg = 0x03,
    EBnepFilterNetTypeResponse = 0x04,
    EBnepFilterMultiAddrSetMsg = 0x05,
    EBnepFilterMultiAddrResponseMsg = 0x06
    } TBnepControlType;




typedef enum
    {
    EFilterNetTypeOperationSuccessful = 0x0000,
    EFilterNetTypeUnsupportedRequest = 0x0001,
    EFilterNetTypeInvalidNetworkingProtocolTypeRange = 0x0002,
    EFilterNetTypeTooManyFilters = 0x0003,
    EFilterNetTypeFailedDueToSecurity = 0x0004
    } TFilterNetTypeResponse;




typedef enum
    {
    EFilterMultiAddrOperationSuccessful = 0x0000,
    EFilterMultiAddrUnsupportedRequest = 0x0001,
    EFilterMultiAddrInvalidMulticastAddress = 0x0002,
    EFilterMultiAddrTooManyFilters = 0x0003,
    EFilterMultiAddrFailedDueToSecurity = 0x0004
    } TFilterMultiAddrResponse;



#endif
