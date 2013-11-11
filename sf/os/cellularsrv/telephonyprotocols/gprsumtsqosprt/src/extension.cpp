// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <networking/umtsnifcontrolif.h>
#include "umts_qos_variables.h"
#include "parameters.h"
#include "extension.h"
#include "guqos_log.h"


RExtensionData::RExtensionData()
	{
	SetUmtsType();	// Default to Umts type
	}


RExtensionData::~RExtensionData()
	{
	Close();
	}

void RExtensionData::Close()
	{
	iBuf.Close();
	}

void RExtensionData::SetUmtsType()
	{
#ifdef SYMBIAN_NETWORKING_UMTSR5
	iType = KPfqosR5ExtensionUmts;
#else
    iType = KPfqosExtensionUmts;
#endif    
	iData.Set(iBuf);
	}

// SBLP
void RExtensionData::SetSblpType()
	{
	iType = KPfqosExtensionSBLP;
	iData.Set(iBuf);
	}



// Internal help function.
static void SetIntValue(pfqos_configblock_int& data, TInt aValue, const TDesC& aName)
	{
	data.len = sizeof(pfqos_configblock_int)/8;
	data.padding = data.reserved = 0;
	data.type = KPfqosTypeInteger;
	data.value = aValue;
	TPtr8 ptr((TUint8*)data.id, 0, KPfqosMaxName);
	if (ptr.MaxLength() >= aName.Length())
		ptr.Copy(aName);
	ptr.ZeroTerminate();
	LOG(Log::Printf(_L("\t\t'%S' = %d"), &aName, aValue));
	}


TInt RExtensionData::SetErrorCode(TInt aErrorCode)
	{

	const int byte_len = (sizeof(pfqos_configure)+sizeof(pfqos_extension)+sizeof(pfqos_configblock_int)*1);
	const int real_len = ((byte_len + 7) / 8) * 8;
	LOG(Log::Printf(_L("\tSetErrorCode %d (size %d"), aErrorCode, real_len));

	if (iBuf.MaxLength() < real_len)
		{
		TInt ret = iBuf.ReAlloc(real_len);
		if (ret != KErrNone)
			{
			LOG(Log::Printf(_L("\tSetErrorCode failed: out of memory %d"), ret));	
			return ret;
			}
		}

	iBuf.SetLength(0);
	pfqos_configure header;
	header.pfqos_configure_len = static_cast<TUint16>((byte_len + 7) / 8);
	header.pfqos_ext_type = EPfqosExtExtension;
	header.reserved = 0;
	header.protocol_id = 0;
	iBuf.Append((TUint8*)&header, sizeof(pfqos_configure));

	pfqos_extension extensionType;
	extensionType.pfqos_ext_len = 0;
	extensionType.pfqos_ext_type = EPfqosExtExtension;
	extensionType.pfqos_extension_type = iType;
	iBuf.Append((TUint8*)&extensionType, sizeof(pfqos_extension));
	LOG(Log::Printf(_L("\t\textensionType = %d"), (TInt)iType));

	// Error code
	pfqos_configblock_int ext;
	SetIntValue(ext, aErrorCode, KDescErrorCode);
	iBuf.Append((TUint8*)&ext, sizeof(pfqos_configblock_int));

	iBuf.AppendFill(0, header.pfqos_configure_len * 8 - byte_len);
	iData.Set(iBuf);
	return KErrNone;
	}

//
// Create an extension data block to be sent to QoS API library.
//
TInt RExtensionData::CreateExtension(const TContextConfig& aRel99, TInt aErrorCode)
	{
	TQoSNegotiated qos;
	aRel99.GetUMTSQoSNeg(qos);

	TUint compression;
	aRel99.GetPdpCompression(compression);
	
#ifdef SYMBIAN_NETWORKING_UMTSR5
	const int byte_len = (sizeof(pfqos_configure)+sizeof(pfqos_extension)+sizeof(pfqos_configblock_int)*16);
#else
	const int byte_len = (sizeof(pfqos_configure)+sizeof(pfqos_extension)+sizeof(pfqos_configblock_int)*14);
#endif
	const int real_len = ((byte_len + 7) / 8) * 8;
	LOG(Log::Printf(_L("\tCreateExtension from TContextConfig (size %d)"), real_len));
	
	if (iBuf.MaxLength() < real_len)
		{
		TInt ret = iBuf.ReAlloc(real_len);
		if (ret != KErrNone)
			{
			LOG(Log::Printf(_L("\tCreateExtension failed: out of memory %d"), ret));	
			return ret;
			}
		}


	iBuf.SetLength(0);
	pfqos_configure header;
	header.pfqos_configure_len = static_cast<TUint16>((byte_len + 7) / 8);
	header.pfqos_ext_type = EPfqosExtExtension;
	header.reserved = 0;
	header.protocol_id = 0;
	iBuf.Append((TUint8*)&header, sizeof(pfqos_configure));

	pfqos_extension extensionType;
	extensionType.pfqos_ext_len = 0;
	extensionType.pfqos_ext_type = EPfqosExtExtension;
#ifdef SYMBIAN_NETWORKING_UMTSR5
	extensionType.pfqos_extension_type = KPfqosR5ExtensionUmts;
#else
	extensionType.pfqos_extension_type = KPfqosExtensionUmts;
#endif	
	iBuf.Append((TUint8*)&extensionType, sizeof(pfqos_extension));
	LOG(Log::Printf(_L("\t\textensionType = %d"), (TInt)KPfqosExtensionUmts));

	// negotiated set
	pfqos_configblock_int ext;
	SetIntValue(ext, qos.iTrafficClass, KDescTrafficClassNegotiated);
	iBuf.Append((TUint8*)&ext, sizeof(pfqos_configblock_int));

	SetIntValue(ext, qos.iDeliveryOrderReqd, KDescDeliveryOrderNegotiated);
	iBuf.Append((TUint8*)&ext, sizeof(pfqos_configblock_int));

	SetIntValue(ext, qos.iDeliverErroneousSDU, KDescDeliveryOfErroneusSduNegotiated);
	iBuf.Append((TUint8*)&ext, sizeof(pfqos_configblock_int));

	SetIntValue(ext, qos.iBER, KDescResidualBerNegotiated);
	iBuf.Append((TUint8*)&ext, sizeof(pfqos_configblock_int));

	SetIntValue(ext, qos.iSDUErrorRatio, KDescErrorRatioNegotiated);
	iBuf.Append((TUint8*)&ext, sizeof(pfqos_configblock_int));

	SetIntValue(ext, qos.iTrafficHandlingPriority, KDescPriorityNegotiated);
	iBuf.Append((TUint8*)&ext, sizeof(pfqos_configblock_int));

	SetIntValue(ext, qos.iTransferDelay, KDescTransferDelayNegotiated);
	iBuf.Append((TUint8*)&ext, sizeof(pfqos_configblock_int));

	SetIntValue(ext, qos.iMaxSDUSize, KDescMaxSduSizeNegotiated);
	iBuf.Append((TUint8*)&ext, sizeof(pfqos_configblock_int));

	SetIntValue(ext, qos.iMaxRate.iUplinkRate, KDescMaxBitrateUplinkNegotiated);
	iBuf.Append((TUint8*)&ext, sizeof(pfqos_configblock_int));

	SetIntValue(ext, qos.iMaxRate.iDownlinkRate, KDescMaxBitrateDownlinkNegotiated);
	iBuf.Append((TUint8*)&ext, sizeof(pfqos_configblock_int));

	SetIntValue(ext, qos.iGuaranteedRate.iUplinkRate, KDescGuaBitrateUplinkNegotiated);
	iBuf.Append((TUint8*)&ext, sizeof(pfqos_configblock_int));

	SetIntValue(ext, qos.iGuaranteedRate.iDownlinkRate, KDescGuaBitrateDownlinkNegotiated);
	iBuf.Append((TUint8*)&ext, sizeof(pfqos_configblock_int));

#ifdef SYMBIAN_NETWORKING_UMTSR5
	SetIntValue(ext, qos.iSignallingIndication, KDescSignallingIndicatorNegotiated);
	iBuf.Append((TUint8*)&ext, sizeof(pfqos_configblock_int));

	SetIntValue(ext, qos.iSourceStatisticsDescriptor, KDescSrcStatDescNegotiated);
	iBuf.Append((TUint8*)&ext, sizeof(pfqos_configblock_int));
#endif // SYMBIAN_NETWORKING_UMTSR5

	// Header compression
	SetIntValue(ext, compression, KDescHeaderCompression);
	iBuf.Append((TUint8*)&ext, sizeof(pfqos_configblock_int));

	// Error code
	SetIntValue(ext, aErrorCode, KDescErrorCode);
	iBuf.Append((TUint8*)&ext, sizeof(pfqos_configblock_int));
	iData.Set(iBuf);
	return KErrNone;
	}
