// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// umtsapi.h - UMTS QoS API
//

#include "umtsextn.h"
#include "qosextn_constants.h"
#include <networking/pfqos.h>

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "umtsextnTraces.h"
#endif
//
// TUmtsQoSParameters
//
//lint -e{1927} would like initializing in initializer list instead of in body.
EXPORT_C TUmtsQoSParameters::TUmtsQoSParameters()
	{
	iTrafficClass = ETrafficClassUnspecified;
	iDeliveryOrder = EDeliveryOrderUnspecified;
	iDeliveryOfErroneusSdu = EErroneousSDUDeliveryUnspecified;
	iResidualBer = EBERUnspecified;
	iErrorRatio = ESDUErrorRatioUnspecified;
	iPriority = ETrafficPriorityUnspecified;
	iTransferDelay = KTransferDelayUnspecified;
	iMaxSduSize = KMaxSDUUnspecified;
	iMaxBitrateUplink = KMaxBitRateUnspecified;
	iMaxBitrateDownlink = KMaxBitRateUnspecified;
	iGuaBitrateUplink = KGrtdBitRateUnspecified;
	iGuaBitrateDownlink = KGrtdBitRateUnspecified;
	}

EXPORT_C TUmtsTrafficClass TUmtsQoSParameters::TrafficClass() const
	{
	return iTrafficClass;
	}


EXPORT_C TUmtsDeliveryOrder TUmtsQoSParameters::DeliveryOrder() const
	{
	return iDeliveryOrder;
	}

EXPORT_C TUmtsErroneousSDUDelivery TUmtsQoSParameters::DeliveryOfErroneusSdu() const
	{
	return iDeliveryOfErroneusSdu;
	}

EXPORT_C TUmtsBitErrorRatio TUmtsQoSParameters::ResidualBer() const
	{
	return iResidualBer;
	}

EXPORT_C TUmtsSDUErrorRatio TUmtsQoSParameters::ErrorRatio() const
	{
	return iErrorRatio;
	}

EXPORT_C TUmtsTrafficHandlingPriority TUmtsQoSParameters::Priority() const
	{
	return iPriority;
	}

EXPORT_C TInt TUmtsQoSParameters::TransferDelay() const
	{
	return iTransferDelay;
	}

EXPORT_C TInt TUmtsQoSParameters::MaxSduSize() const
	{
	return iMaxSduSize;
	}

EXPORT_C TInt TUmtsQoSParameters::MaxBitrateUplink() const
	{
	return iMaxBitrateUplink;
	}

EXPORT_C TInt TUmtsQoSParameters::MaxBitrateDownlink() const
	{
	return iMaxBitrateDownlink;
	}

EXPORT_C TInt TUmtsQoSParameters::GuaranteedBitrateUplink() const
	{
	return iGuaBitrateUplink;
	}

EXPORT_C TInt TUmtsQoSParameters::GuaranteedBitrateDownlink() const
	{
	return iGuaBitrateDownlink;
	}


EXPORT_C TInt TUmtsQoSParameters::SetTrafficClass(TUmtsTrafficClass aTrafficClass)
	{
	iTrafficClass = aTrafficClass;
	return KErrNone;
	}

EXPORT_C TInt TUmtsQoSParameters::SetDeliveryOrder(TUmtsDeliveryOrder aDeliveryOrder)
	{
	iDeliveryOrder = aDeliveryOrder;
	return KErrNone;
	}

EXPORT_C TInt TUmtsQoSParameters::SetDeliveryOfErroneusSdu(TUmtsErroneousSDUDelivery aDeliveryOfErroneusSdu)
	{
	iDeliveryOfErroneusSdu = aDeliveryOfErroneusSdu;
	return KErrNone;
	}

EXPORT_C TInt TUmtsQoSParameters::SetResidualBer(TUmtsBitErrorRatio aResidualBer)
	{
	iResidualBer = aResidualBer;
	return KErrNone;
	}

EXPORT_C TInt TUmtsQoSParameters::SetErrorRatio(TUmtsSDUErrorRatio aErrorRatio)
	{
	iErrorRatio = aErrorRatio;
	return KErrNone;
	}

EXPORT_C TInt TUmtsQoSParameters::SetPriority(TUmtsTrafficHandlingPriority aPriority)
	{
	iPriority = aPriority;
	return KErrNone;
	}

EXPORT_C TInt TUmtsQoSParameters::SetTransferDelay(TUint aTransferDelay)
	{
	if (aTransferDelay > KTransferDelayMax)
		return KErrArgument;
	iTransferDelay = aTransferDelay;
	return KErrNone;
	}

EXPORT_C TInt TUmtsQoSParameters::SetMaxSduSize(TUint aMaxSduSize)
	{
	if (aMaxSduSize > KMaxSDUMaximum)
		return KErrArgument;
	iMaxSduSize = aMaxSduSize;
	return KErrNone;
	}

EXPORT_C TInt TUmtsQoSParameters::SetMaxBitrateUplink(TUint aMaxBitrateUplink)
	{
	if (aMaxBitrateUplink > KMaxBitRateMaximum)
		return KErrArgument;
	iMaxBitrateUplink = aMaxBitrateUplink;
	return KErrNone;
	}

EXPORT_C TInt TUmtsQoSParameters::SetMaxBitrateDownlink(TUint aMaxBitrateDownlink)
	{
	if (aMaxBitrateDownlink > KMaxBitRateMaximum)
		return KErrArgument;
	iMaxBitrateDownlink = aMaxBitrateDownlink;
	return KErrNone;
	}

EXPORT_C TInt TUmtsQoSParameters::SetGuaranteedBitrateUplink(TUint aGuaBitrateUplink)
	{
	if (aGuaBitrateUplink > KGrtdBitRateMaximum)
		return KErrArgument;
	iGuaBitrateUplink = aGuaBitrateUplink;
	return KErrNone;
	}

EXPORT_C TInt TUmtsQoSParameters::SetGuaranteedBitrateDownlink(TUint aGuaBitrateDownlink)
	{
	if (aGuaBitrateDownlink > KGrtdBitRateMaximum)
		return KErrArgument;
	iGuaBitrateDownlink = aGuaBitrateDownlink;
	return KErrNone;
	}


//
// TNegotiatedUmtsQoSParameters
//
EXPORT_C TNegotiatedUmtsQoSParameters::TNegotiatedUmtsQoSParameters() : iCompression(0), iErrorCode(KErrNone)
	{
	}

EXPORT_C TUint TNegotiatedUmtsQoSParameters::HeaderCompression() const
	{
	return iCompression;
	}

EXPORT_C TInt TNegotiatedUmtsQoSParameters::ErrorCode() const
	{
	return iErrorCode;
	}

// Buffer size
const TUint KUmtsApiBufSize = 8192;

//
// TUmtsQoSPolicy
//
EXPORT_C CUmtsQoSPolicy* CUmtsQoSPolicy::NewL()
	{
	CUmtsQoSPolicy* policy = new (ELeave) CUmtsQoSPolicy();
	CleanupStack::PushL(policy);
	policy->ConstructL();
	CleanupStack::Pop();
	return policy;
	}

CUmtsQoSPolicy::CUmtsQoSPolicy() : iBufPtr(0,0)
	{
	iType = KPfqosExtensionUmts;
	}

void CUmtsQoSPolicy::ConstructL()
	{
	iData = HBufC8::NewL(KUmtsApiBufSize);
	TPtr8 tmp(iData->Des());
	iBufPtr.Set(tmp);
	}

EXPORT_C CUmtsQoSPolicy::~CUmtsQoSPolicy()
	{
	delete iData;
	iData = 0;
	}

static void SetIntValue(pfqos_configblock_int& data, TInt aValue, const TDesC8& aName)
	{
	data.len = sizeof(pfqos_configblock_int)/8;
	data.padding = data.reserved = 0;
	data.type = KPfqosTypeInteger;
	data.value = aValue;
	Mem::FillZ(data.id, KPfqosMaxName);
	Mem::Copy(data.id, aName.Ptr(), aName.Length());
	}

EXPORT_C TDesC8& CUmtsQoSPolicy::Data()
	{
	iBufPtr.SetLength(0);
	const int byte_len = (sizeof(pfqos_configure)+sizeof(pfqos_extension)+sizeof(pfqos_configblock_int)*37);

	pfqos_configure iHeader;
	iHeader.pfqos_configure_len = (TUint16)((byte_len + 7) / 8);
	iHeader.pfqos_ext_type = EPfqosExtExtension;
	iHeader.reserved = 0;
	iHeader.protocol_id = 0;
	iBufPtr.Append((TUint8*)&iHeader, sizeof(pfqos_configure));

	pfqos_extension aExtensionType;
	aExtensionType.pfqos_ext_len = 0;
	aExtensionType.pfqos_ext_type = EPfqosExtExtension;
	aExtensionType.pfqos_extension_type = KPfqosExtensionUmts;
	iBufPtr.Append((TUint8*)&aExtensionType, sizeof(pfqos_extension));

	pfqos_configblock_int iExt;

	// Minimum
	SetIntValue(iExt, iMinimum.iTrafficClass, KDescTrafficClassMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iDeliveryOrder, KDescDeliveryOrderMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iDeliveryOfErroneusSdu, KDescDeliveryOfErroneusSduMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iResidualBer, KDescResidualBerMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iErrorRatio, KDescErrorRatioMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iPriority, KDescPriorityMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iTransferDelay, KDescTransferDelayMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iMaxSduSize, KDescMaxSduSizeMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iMaxBitrateUplink, KDescMaxBitrateUplinkMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iMaxBitrateDownlink, KDescMaxBitrateDownlinkMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iGuaBitrateUplink, KDescGuaBitrateUplinkMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iGuaBitrateDownlink, KDescGuaBitrateDownlinkMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	// Requested
	SetIntValue(iExt, iRequested.iTrafficClass, KDescTrafficClassRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iDeliveryOrder, KDescDeliveryOrderRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iDeliveryOfErroneusSdu, KDescDeliveryOfErroneusSduRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iResidualBer, KDescResidualBerRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iErrorRatio, KDescErrorRatioRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iPriority, KDescPriorityRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iTransferDelay, KDescTransferDelayRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iMaxSduSize, KDescMaxSduSizeRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iMaxBitrateUplink, KDescMaxBitrateUplinkRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iMaxBitrateDownlink, KDescMaxBitrateDownlinkRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iGuaBitrateUplink, KDescGuaBitrateUplinkRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iGuaBitrateDownlink, KDescGuaBitrateDownlinkRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	// Negotiated
	SetIntValue(iExt, iNegotiated.iTrafficClass, KDescTrafficClassNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iDeliveryOrder, KDescDeliveryOrderNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iDeliveryOfErroneusSdu, KDescDeliveryOfErroneusSduNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iResidualBer, KDescResidualBerNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iErrorRatio, KDescErrorRatioNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iPriority, KDescPriorityNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iTransferDelay, KDescTransferDelayNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iMaxSduSize, KDescMaxSduSizeNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iMaxBitrateUplink, KDescMaxBitrateUplinkNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iMaxBitrateDownlink, KDescMaxBitrateDownlinkNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iGuaBitrateUplink, KDescGuaBitrateUplinkNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iGuaBitrateDownlink, KDescGuaBitrateDownlinkNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));


	SetIntValue(iExt, iCompression, KDescHeaderCompression);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	iBufPtr.AppendFill(0, iHeader.pfqos_configure_len * 8 - byte_len);
	return iBufPtr;
	}

EXPORT_C TInt CUmtsQoSPolicy::ParseMessage(const TDesC8& aData)
	{
	const TUint8 *p = aData.Ptr();
	TInt length = aData.Length();
	//lint --e{826} Not a suspicious conversion, as lint thinks (shut off all of them for this function)
	struct pfqos_configure *ext = (struct pfqos_configure *) p;

	if (length < (TInt)sizeof(pfqos_configure))
		return KErrGeneral;		// EMSGSIZE (impossible message size)

	if (ext->pfqos_configure_len * 8 != length)
		return KErrGeneral;		// EMSGSIZE (incorrect message length)

	if (ext->pfqos_ext_type == EPfqosExtExtension)
		{
		p += sizeof(struct pfqos_configure);
		pfqos_extension *aExtensionType = (pfqos_extension *) p;

		if (aExtensionType->pfqos_extension_type != KPfqosExtensionUmts)
			return KErrGeneral;
	
		while (length > 0)
			{
			struct pfqos_configblock *block = (struct pfqos_configblock *)(p + sizeof(pfqos_extension));
			int block_len = block->len;

			if (block_len < 1)
				break;
			block_len *= 8;

			if (block_len > length)
				break;

			if (block->type == KPfqosTypeInteger)
				{
				TPtrC8 tmp((TUint8 *)&block->id[0]);
				struct pfqos_configblock_int *block_int = (struct pfqos_configblock_int*) block;

				// Negotiated set
				//lint -e{961} would like terminating 'else' here...
				if (tmp.Compare(_L8("DeliveryOfErroneusSduNegotiated"))==0)
					{
					iNegotiated.iDeliveryOfErroneusSdu = (TUmtsErroneousSDUDelivery)block_int->value;
					}
				else if (tmp.Compare(_L8("DeliveryOrderNegotiated"))==0)
					{
					iNegotiated.iDeliveryOrder = (TUmtsDeliveryOrder)block_int->value;
					}
				else if (tmp.Compare(_L8("ErrorRatioNegotiated"))==0)
					{
					iNegotiated.iErrorRatio = (TUmtsSDUErrorRatio)block_int->value;
					}
				else if (tmp.Compare(_L8("GuaBitrateDownlinkNegotiated"))==0)
					{
					iNegotiated.iGuaBitrateDownlink = block_int->value;
					}
				else if (tmp.Compare(_L8("GuaBitrateUplinkNegotiated"))==0)
					{
					iNegotiated.iGuaBitrateUplink = block_int->value;
					}
				else if (tmp.Compare(_L8("MaxBitrateDownlinkNegotiated"))==0)
					{
					iNegotiated.iMaxBitrateDownlink = block_int->value;
					}
				else if (tmp.Compare(_L8("MaxBitrateUplinkNegotiated"))==0)
					{
					iNegotiated.iMaxBitrateUplink = block_int->value;
					}
				else if (tmp.Compare(_L8("MaxSduSizeNegotiated"))==0)
					{
					iNegotiated.iMaxSduSize = block_int->value;
					}
				else if (tmp.Compare(_L8("PriorityNegotiated"))==0)
					{
					iNegotiated.iPriority = (TUmtsTrafficHandlingPriority)block_int->value;
					}
				else if (tmp.Compare(_L8("ResidualBerNegotiated"))==0)
					{
					iNegotiated.iResidualBer = (TUmtsBitErrorRatio)block_int->value;
					}
				else if (tmp.Compare(_L8("TrafficClassNegotiated"))==0)
					{
					iNegotiated.iTrafficClass = (TUmtsTrafficClass)block_int->value;
					}
				else if (tmp.Compare(_L8("TransferDelayNegotiated"))==0)
					{
					iNegotiated.iTransferDelay = block_int->value;
					}
				// Minimum set
				else if (tmp.Compare(_L8("DeliveryOfErroneusSduMinimum"))==0)
					{
					iMinimum.iDeliveryOfErroneusSdu = (TUmtsErroneousSDUDelivery)block_int->value;
					}
				else if (tmp.Compare(_L8("DeliveryOrderMinimum"))==0)
					{
					iMinimum.iDeliveryOrder = (TUmtsDeliveryOrder)block_int->value;
					}
				else if (tmp.Compare(_L8("ErrorRatioMinimum"))==0)
					{
					iMinimum.iErrorRatio = (TUmtsSDUErrorRatio)block_int->value;
					}
				else if (tmp.Compare(_L8("GuaBitrateDownlinkMinimum"))==0)
					{
					iMinimum.iGuaBitrateDownlink = block_int->value;
					}
				else if (tmp.Compare(_L8("GuaBitrateUplinkMinimum"))==0)
					{
					iMinimum.iGuaBitrateUplink = block_int->value;
					}
				else if (tmp.Compare(_L8("MaxBitrateDownlinkMinimum"))==0)
					{
					iMinimum.iMaxBitrateDownlink = block_int->value;
					}
				else if (tmp.Compare(_L8("MaxBitrateUplinkMinimum"))==0)
					{
					iMinimum.iMaxBitrateUplink = block_int->value;
					}
				else if (tmp.Compare(_L8("MaxSduSizeMinimum"))==0)
					{
					iMinimum.iMaxSduSize = block_int->value;
					}
				else if (tmp.Compare(_L8("PriorityMinimum"))==0)
					{
					iMinimum.iPriority = (TUmtsTrafficHandlingPriority)block_int->value;
					}
				else if (tmp.Compare(_L8("ResidualBerMinimum"))==0)
					{
					iMinimum.iResidualBer = (TUmtsBitErrorRatio)block_int->value;
					}
				else if (tmp.Compare(_L8("TrafficClassMinimum"))==0)
					{
					iMinimum.iTrafficClass = (TUmtsTrafficClass)block_int->value;
					}
				else if (tmp.Compare(_L8("TransferDelayMinimum"))==0)
					{
					iMinimum.iTransferDelay = block_int->value;
					}
				// Requested set
				else if (tmp.Compare(_L8("DeliveryOfErroneusSduRequested"))==0)
					{
					iRequested.iDeliveryOfErroneusSdu = (TUmtsErroneousSDUDelivery)block_int->value;
					}
				else if (tmp.Compare(_L8("DeliveryOrderRequested"))==0)
					{
					iRequested.iDeliveryOrder = (TUmtsDeliveryOrder)block_int->value;
					}
				else if (tmp.Compare(_L8("ErrorRatioRequested"))==0)
					{
					iRequested.iErrorRatio = (TUmtsSDUErrorRatio)block_int->value;
					}
				else if (tmp.Compare(_L8("GuaBitrateDownlinkRequested"))==0)
					{
					iRequested.iGuaBitrateDownlink = block_int->value;
					}
				else if (tmp.Compare(_L8("GuaBitrateUplinkRequested"))==0)
					{
					iRequested.iGuaBitrateUplink = block_int->value;
					}
				else if (tmp.Compare(_L8("MaxBitrateDownlinkRequested"))==0)
					{
					iRequested.iMaxBitrateDownlink = block_int->value;
					}
				else if (tmp.Compare(_L8("MaxBitrateUplinkRequested"))==0)
					{
					iRequested.iMaxBitrateUplink = block_int->value;
					}
				else if (tmp.Compare(_L8("MaxSduSizeRequested"))==0)
					{
					iRequested.iMaxSduSize = block_int->value;
					}
				else if (tmp.Compare(_L8("PriorityRequested"))==0)
					{
					iRequested.iPriority = (TUmtsTrafficHandlingPriority)block_int->value;
					}
				else if (tmp.Compare(_L8("ResidualBerRequested"))==0)
					{
					iRequested.iResidualBer = (TUmtsBitErrorRatio)block_int->value;
					}
				else if (tmp.Compare(_L8("TrafficClassRequested"))==0)
					{
					iRequested.iTrafficClass = (TUmtsTrafficClass)block_int->value;
					}
				else if (tmp.Compare(_L8("TransferDelayRequested"))==0)
					{
					iRequested.iTransferDelay = block_int->value;
					}
				else if (tmp.Compare(_L8("HeaderCompression"))==0)
					{
					iNegotiated.iCompression = block_int->value;
					}
				else if (tmp.Compare(_L8("ErrorCode"))==0)
					{
					iNegotiated.iErrorCode = block_int->value;
					}
				}
			p += block_len;
			length -= block_len;
			}
		}
	return KErrNone;
	}

EXPORT_C CExtensionBase* CUmtsQoSPolicy::CreateL()
	{
	CUmtsQoSPolicy *extension = CUmtsQoSPolicy::NewL();
	return extension;
	}

EXPORT_C TInt CUmtsQoSPolicy::Copy(const CExtensionBase& aExtension)
	{
	if (aExtension.Type() != iType)
		return KErrArgument;
	const CUmtsQoSPolicy& policy = (const CUmtsQoSPolicy&)aExtension;
	policy.GetQoSMinimum(iMinimum);
	policy.GetQoSRequested(iRequested);
	policy.GetQoSNegotiated(iNegotiated);
	iCompression = policy.HeaderCompression();
	return KErrNone;
	}

EXPORT_C void CUmtsQoSPolicy::SetQoSRequested(const TUmtsQoSParameters& aRequested)
	{

	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSREQUESTED_1, "<------------------------------------------------\n");
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSREQUESTED_2, "CUmtsQoSPolicy::SetQoSRequested");
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSREQUESTED_3, "\n");
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSREQUESTED_4, "REQUESTED R99 VALUES SUPPLIED BY CLIENT IS \n");
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSREQUESTED_5, "\n");
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSREQUESTED_6, " [aRequested.iTrafficClass         = %d]\n",aRequested.iTrafficClass);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSREQUESTED_7, " [aRequested.iDeliveryOrder            = %d]\n",aRequested.iDeliveryOrder);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSREQUESTED_8, " [aRequested.iDeliveryOfErroneusSdu = %d]\n",aRequested.iDeliveryOfErroneusSdu);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSREQUESTED_9, " [aRequested.iMaxSduSize           = %d]\n",aRequested.iMaxSduSize);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSREQUESTED_10, " [aRequested.iMaxBitrateUplink    = %d]\n",aRequested.iMaxBitrateUplink);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSREQUESTED_11, " [aRequested.iMaxBitrateDownlink  = %d]\n",aRequested.iMaxBitrateDownlink);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSREQUESTED_12, " [aRequested.iResidualBer         = %d]\n",aRequested.iResidualBer);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSREQUESTED_13, " [aRequested.iErrorRatio          = %d]\n",aRequested.iErrorRatio);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSREQUESTED_14, " [aRequested.iPriority            = %d]\n",aRequested.iPriority);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSREQUESTED_15, " [aRequested.iTransferDelay       = %d]\n",aRequested.iTransferDelay);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSREQUESTED_16, " [aRequested.iGuaBitrateUplink    = %d]\n",aRequested.iGuaBitrateUplink);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSREQUESTED_17, " [aRequested.iGuaBitrateDownlink  = %d]\n",aRequested.iGuaBitrateDownlink);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSREQUESTED_18, "------------------------------------------------>\n");
	
	iRequested = aRequested;
	}

EXPORT_C void CUmtsQoSPolicy::SetQoSMinimum(const TUmtsQoSParameters& aMinimum)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSMINIMUM_1, "<------------------------------------------------\n");
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSMINIMUM_2, "CUmtsQoSPolicy::SetQoSMinimum");
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSMINIMUM_3, "\n");
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSMINIMUM_4, "MINIMUM R99 VALUES SUPPLIED BY CLIENT IS \n");
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSMINIMUM_5, "\n");
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSMINIMUM_6, " [aMinimum.iTrafficClass             = %d]\n",aMinimum.iTrafficClass);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSMINIMUM_7, " [aMinimum.iDeliveryOrder            = %d]\n",aMinimum.iDeliveryOrder);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSMINIMUM_8, " [aMinimum.iDeliveryOfErroneusSdu    = %d]\n",aMinimum.iDeliveryOfErroneusSdu);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSMINIMUM_9, " [aMinimum.iMaxSduSize               = %d]\n",aMinimum.iMaxSduSize);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSMINIMUM_10, " [aMinimum.iMaxBitrateUplink        = %d]\n",aMinimum.iMaxBitrateUplink);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSMINIMUM_11, " [aMinimum.iMaxBitrateDownlink      = %d]\n",aMinimum.iMaxBitrateDownlink);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSMINIMUM_12, " [aMinimum.iResidualBer             = %d]\n",aMinimum.iResidualBer);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSMINIMUM_13, " [aMinimum.iErrorRatio              = %d]\n",aMinimum.iErrorRatio);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSMINIMUM_14, " [aMinimum.iPriority                = %d]\n",aMinimum.iPriority);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSMINIMUM_15, " [aMinimum.iTransferDelay           = %d]\n",aMinimum.iTransferDelay);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSMINIMUM_16, " [aMinimum.iGuaBitrateUplink        = %d]\n",aMinimum.iGuaBitrateUplink);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSMINIMUM_17, " [aMinimum.iGuaBitrateDownlink      = %d]\n",aMinimum.iGuaBitrateDownlink);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSQOSPOLICY_SETQOSMINIMUM_18, "------------------------------------------------>\n");
	
	iMinimum = aMinimum;
	}

EXPORT_C void CUmtsQoSPolicy::GetQoSRequested(TUmtsQoSParameters& aRequested) const
	{
	aRequested = iRequested;
	}

EXPORT_C void CUmtsQoSPolicy::GetQoSMinimum(TUmtsQoSParameters& aMinimum) const
	{
	aMinimum = iMinimum;
	}

EXPORT_C void CUmtsQoSPolicy::GetQoSNegotiated(TNegotiatedUmtsQoSParameters& aNegotiated) const
	{
	aNegotiated = iNegotiated;
	}

EXPORT_C TInt CUmtsQoSPolicy::SetHeaderCompression(TUint aCompression)
	{
	if (aCompression & ~(KPdpDataCompression|KPdpHeaderCompression))
		return KErrNotSupported;
	iCompression = aCompression;
	return KErrNone;
	}

EXPORT_C TUint CUmtsQoSPolicy::HeaderCompression() const
	{
	return iCompression;
	}

//
// TUmtsR5QoSParameters
//
EXPORT_C TUmtsR5QoSParameters::TUmtsR5QoSParameters()
	: TUmtsQoSParameters(),
	  iSignallingIndicator(EFalse),
	  iSrcStatisticsDesc(ESourceStatisticsDescriptorUnknown)
	{
	}

EXPORT_C TBool TUmtsR5QoSParameters::SignallingIndicator() const
	{
	return iSignallingIndicator;
	}

EXPORT_C TUmtsSourceStatisticsDescriptor TUmtsR5QoSParameters::SourceStatisticsDescriptor() const
	{
	return iSrcStatisticsDesc;
	}

EXPORT_C TInt TUmtsR5QoSParameters::SetSignallingIndicator(TBool aSignallingIndicator)
	{
	iSignallingIndicator = aSignallingIndicator;
	return KErrNone;
	}

EXPORT_C TInt TUmtsR5QoSParameters::SetSourceStatisticsDescriptor(TUmtsSourceStatisticsDescriptor aSrcStatisticsDesc)
	{
	iSrcStatisticsDesc = aSrcStatisticsDesc;
	return KErrNone;
	}


//
// TNegotiatedUmtsR5QoSParameters
//
EXPORT_C TNegotiatedUmtsR5QoSParameters::TNegotiatedUmtsR5QoSParameters() : iCompression(0), iErrorCode(KErrNone)
	{
	}

EXPORT_C TUint TNegotiatedUmtsR5QoSParameters::HeaderCompression() const
	{
	return iCompression;
	}

EXPORT_C TInt TNegotiatedUmtsR5QoSParameters::ErrorCode() const
	{
	return iErrorCode;
	}


// Buffer size
// const TUint KUmtsApiBufSize = 8192; should be defined in case
// if the umts r5 and r99 needs to be supported seperately

//
// CUmtsR5QoSPolicy
//
EXPORT_C CUmtsR5QoSPolicy* CUmtsR5QoSPolicy::NewL()
	{
	CUmtsR5QoSPolicy* policy = new (ELeave) CUmtsR5QoSPolicy();
	CleanupStack::PushL(policy);
	policy->ConstructL();
	CleanupStack::Pop();
	return policy;
	}

CUmtsR5QoSPolicy::CUmtsR5QoSPolicy() : iBufPtr(0,0)
	{
	iType = KPfqosR5ExtensionUmts;
	}

void CUmtsR5QoSPolicy::ConstructL()
	{
	iData = HBufC8::NewL(KUmtsApiBufSize);
	TPtr8 tmp(iData->Des());
	iBufPtr.Set(tmp);
	}

EXPORT_C CUmtsR5QoSPolicy::~CUmtsR5QoSPolicy()
	{
	delete iData;
	iData = 0;
	}

EXPORT_C TDesC8& CUmtsR5QoSPolicy::Data()
	{
	iBufPtr.SetLength(0);
	const int byte_len = (sizeof(pfqos_configure)+sizeof(pfqos_extension)+sizeof(pfqos_configblock_int)*43);

	pfqos_configure iHeader;
	iHeader.pfqos_configure_len = (TUint16)((byte_len + 7) / 8);
	iHeader.pfqos_ext_type = EPfqosExtExtension;
	iHeader.reserved = 0;
	iHeader.protocol_id = 0;
	iBufPtr.Append((TUint8*)&iHeader, sizeof(pfqos_configure));

	pfqos_extension aExtensionType;
	aExtensionType.pfqos_ext_len = 0;
	aExtensionType.pfqos_ext_type = EPfqosExtExtension;
	aExtensionType.pfqos_extension_type = KPfqosR5ExtensionUmts;
	iBufPtr.Append((TUint8*)&aExtensionType, sizeof(pfqos_extension));

	pfqos_configblock_int iExt;

	// Minimum
	SetIntValue(iExt, iMinimum.iTrafficClass, KDescTrafficClassMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iDeliveryOrder, KDescDeliveryOrderMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iDeliveryOfErroneusSdu, KDescDeliveryOfErroneusSduMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iResidualBer, KDescResidualBerMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iErrorRatio, KDescErrorRatioMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iPriority, KDescPriorityMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iTransferDelay, KDescTransferDelayMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iMaxSduSize, KDescMaxSduSizeMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iMaxBitrateUplink, KDescMaxBitrateUplinkMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iMaxBitrateDownlink, KDescMaxBitrateDownlinkMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iGuaBitrateUplink, KDescGuaBitrateUplinkMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iMinimum.iGuaBitrateDownlink, KDescGuaBitrateDownlinkMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));
	
	SetIntValue(iExt, iMinimum.iSignallingIndicator, KDescSignallingIndicatorMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));
	
	SetIntValue(iExt, iMinimum.iSrcStatisticsDesc, KDescSrcStatDescMinimum);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	// Requested
	SetIntValue(iExt, iRequested.iTrafficClass, KDescTrafficClassRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iDeliveryOrder, KDescDeliveryOrderRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iDeliveryOfErroneusSdu, KDescDeliveryOfErroneusSduRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iResidualBer, KDescResidualBerRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iErrorRatio, KDescErrorRatioRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iPriority, KDescPriorityRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iTransferDelay, KDescTransferDelayRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iMaxSduSize, KDescMaxSduSizeRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iMaxBitrateUplink, KDescMaxBitrateUplinkRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iMaxBitrateDownlink, KDescMaxBitrateDownlinkRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iGuaBitrateUplink, KDescGuaBitrateUplinkRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iRequested.iGuaBitrateDownlink, KDescGuaBitrateDownlinkRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));
	
	SetIntValue(iExt, iRequested.iSignallingIndicator, KDescSignallingIndicatorRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));
	
	SetIntValue(iExt, iRequested.iSrcStatisticsDesc, KDescSrcStatDescRequested);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	// Negotiated
	SetIntValue(iExt, iNegotiated.iTrafficClass, KDescTrafficClassNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iDeliveryOrder, KDescDeliveryOrderNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iDeliveryOfErroneusSdu, KDescDeliveryOfErroneusSduNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iResidualBer, KDescResidualBerNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iErrorRatio, KDescErrorRatioNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iPriority, KDescPriorityNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iTransferDelay, KDescTransferDelayNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iMaxSduSize, KDescMaxSduSizeNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iMaxBitrateUplink, KDescMaxBitrateUplinkNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iMaxBitrateDownlink, KDescMaxBitrateDownlinkNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iGuaBitrateUplink, KDescGuaBitrateUplinkNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iGuaBitrateDownlink, KDescGuaBitrateDownlinkNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));
	
	SetIntValue(iExt, iNegotiated.iSignallingIndicator, KDescSignallingIndicatorNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iNegotiated.iSrcStatisticsDesc, KDescSrcStatDescNegotiated);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	SetIntValue(iExt, iCompression, KDescHeaderCompression);
	iBufPtr.Append((TUint8*)&iExt, sizeof(pfqos_configblock_int));

	iBufPtr.AppendFill(0, iHeader.pfqos_configure_len * 8 - byte_len);
	return iBufPtr;
	}

EXPORT_C TInt CUmtsR5QoSPolicy::ParseMessage(const TDesC8& aData)
	{
	const TUint8 *p = aData.Ptr();
	TInt length = aData.Length();
	//lint --e{826} Not a suspicious conversion, as lint thinks (shut off all of them for this function)
	struct pfqos_configure *ext = (struct pfqos_configure *) p;

	if (length < (TInt)sizeof(pfqos_configure))
		return KErrGeneral;		// EMSGSIZE (impossible message size)

	if (ext->pfqos_configure_len * 8 != length)
		return KErrGeneral;		// EMSGSIZE (incorrect message length)

	if (ext->pfqos_ext_type == EPfqosExtExtension)
		{
		p += sizeof(struct pfqos_configure);
		pfqos_extension *aExtensionType = (pfqos_extension *) p;

		if (aExtensionType->pfqos_extension_type != KPfqosR5ExtensionUmts)
			return KErrGeneral;
	
		while (length > 0)
			{
			struct pfqos_configblock *block = (struct pfqos_configblock *)(p + sizeof(pfqos_extension));
			int block_len = block->len;

			if (block_len < 1)
				break;
			block_len *= 8;

			if (block_len > length)
				break;

			if (block->type == KPfqosTypeInteger)
				{
				TPtrC8 tmp((TUint8 *)&block->id[0]);
				struct pfqos_configblock_int *block_int = (struct pfqos_configblock_int*) block;

				// Negotiated set
				//lint -e{961} would like terminating 'else' here...
				if (tmp.Compare(_L8("DeliveryOfErroneusSduNegotiated"))==0)
					{
					iNegotiated.iDeliveryOfErroneusSdu = (TUmtsErroneousSDUDelivery)block_int->value;
					}
				else if (tmp.Compare(_L8("DeliveryOrderNegotiated"))==0)
					{
					iNegotiated.iDeliveryOrder = (TUmtsDeliveryOrder)block_int->value;
					}
				else if (tmp.Compare(_L8("ErrorRatioNegotiated"))==0)
					{
					iNegotiated.iErrorRatio = (TUmtsSDUErrorRatio)block_int->value;
					}
				else if (tmp.Compare(_L8("GuaBitrateDownlinkNegotiated"))==0)
					{
					iNegotiated.iGuaBitrateDownlink = block_int->value;
					}
				else if (tmp.Compare(_L8("GuaBitrateUplinkNegotiated"))==0)
					{
					iNegotiated.iGuaBitrateUplink = block_int->value;
					}
				else if (tmp.Compare(_L8("MaxBitrateDownlinkNegotiated"))==0)
					{
					iNegotiated.iMaxBitrateDownlink = block_int->value;
					}
				else if (tmp.Compare(_L8("MaxBitrateUplinkNegotiated"))==0)
					{
					iNegotiated.iMaxBitrateUplink = block_int->value;
					}
				else if (tmp.Compare(_L8("MaxSduSizeNegotiated"))==0)
					{
					iNegotiated.iMaxSduSize = block_int->value;
					}
				else if (tmp.Compare(_L8("PriorityNegotiated"))==0)
					{
					iNegotiated.iPriority = (TUmtsTrafficHandlingPriority)block_int->value;
					}
				else if (tmp.Compare(_L8("ResidualBerNegotiated"))==0)
					{
					iNegotiated.iResidualBer = (TUmtsBitErrorRatio)block_int->value;
					}
				else if (tmp.Compare(_L8("TrafficClassNegotiated"))==0)
					{
					iNegotiated.iTrafficClass = (TUmtsTrafficClass)block_int->value;
					}
				else if (tmp.Compare(_L8("TransferDelayNegotiated"))==0)
					{
					iNegotiated.iTransferDelay = block_int->value;
					}
				else if (tmp.Compare(_L8("SignallingIndicatorNegotiated"))==0)
					{
					iNegotiated.iSignallingIndicator = block_int->value;
					}
				else if (tmp.Compare(_L8("SrcStatDescNegotiated"))==0)
					{
					iNegotiated.iSrcStatisticsDesc = (TUmtsSourceStatisticsDescriptor)block_int->value;
					}
				// Minimum set
				else if (tmp.Compare(_L8("DeliveryOfErroneusSduMinimum"))==0)
					{
					iMinimum.iDeliveryOfErroneusSdu = (TUmtsErroneousSDUDelivery)block_int->value;
					}
				else if (tmp.Compare(_L8("DeliveryOrderMinimum"))==0)
					{
					iMinimum.iDeliveryOrder = (TUmtsDeliveryOrder)block_int->value;
					}
				else if (tmp.Compare(_L8("ErrorRatioMinimum"))==0)
					{
					iMinimum.iErrorRatio = (TUmtsSDUErrorRatio)block_int->value;
					}
				else if (tmp.Compare(_L8("GuaBitrateDownlinkMinimum"))==0)
					{
					iMinimum.iGuaBitrateDownlink = block_int->value;
					}
				else if (tmp.Compare(_L8("GuaBitrateUplinkMinimum"))==0)
					{
					iMinimum.iGuaBitrateUplink = block_int->value;
					}
				else if (tmp.Compare(_L8("MaxBitrateDownlinkMinimum"))==0)
					{
					iMinimum.iMaxBitrateDownlink = block_int->value;
					}
				else if (tmp.Compare(_L8("MaxBitrateUplinkMinimum"))==0)
					{
					iMinimum.iMaxBitrateUplink = block_int->value;
					}
				else if (tmp.Compare(_L8("MaxSduSizeMinimum"))==0)
					{
					iMinimum.iMaxSduSize = block_int->value;
					}
				else if (tmp.Compare(_L8("PriorityMinimum"))==0)
					{
					iMinimum.iPriority = (TUmtsTrafficHandlingPriority)block_int->value;
					}
				else if (tmp.Compare(_L8("ResidualBerMinimum"))==0)
					{
					iMinimum.iResidualBer = (TUmtsBitErrorRatio)block_int->value;
					}
				else if (tmp.Compare(_L8("TrafficClassMinimum"))==0)
					{
					iMinimum.iTrafficClass = (TUmtsTrafficClass)block_int->value;
					}
				else if (tmp.Compare(_L8("TransferDelayMinimum"))==0)
					{
					iMinimum.iTransferDelay = block_int->value;
					}
				else if (tmp.Compare(_L8("SignallingIndicatorMinimum"))==0)
					{
					iMinimum.iSignallingIndicator = block_int->value;
					}
				else if (tmp.Compare(_L8("SrcStatDescMinimum"))==0)
					{
					iMinimum.iSrcStatisticsDesc = (TUmtsSourceStatisticsDescriptor)block_int->value;
					}
				// Requested set
				else if (tmp.Compare(_L8("DeliveryOfErroneusSduRequested"))==0)
					{
					iRequested.iDeliveryOfErroneusSdu = (TUmtsErroneousSDUDelivery)block_int->value;
					}
				else if (tmp.Compare(_L8("DeliveryOrderRequested"))==0)
					{
					iRequested.iDeliveryOrder = (TUmtsDeliveryOrder)block_int->value;
					}
				else if (tmp.Compare(_L8("ErrorRatioRequested"))==0)
					{
					iRequested.iErrorRatio = (TUmtsSDUErrorRatio)block_int->value;
					}
				else if (tmp.Compare(_L8("GuaBitrateDownlinkRequested"))==0)
					{
					iRequested.iGuaBitrateDownlink = block_int->value;
					}
				else if (tmp.Compare(_L8("GuaBitrateUplinkRequested"))==0)
					{
					iRequested.iGuaBitrateUplink = block_int->value;
					}
				else if (tmp.Compare(_L8("MaxBitrateDownlinkRequested"))==0)
					{
					iRequested.iMaxBitrateDownlink = block_int->value;
					}
				else if (tmp.Compare(_L8("MaxBitrateUplinkRequested"))==0)
					{
					iRequested.iMaxBitrateUplink = block_int->value;
					}
				else if (tmp.Compare(_L8("MaxSduSizeRequested"))==0)
					{
					iRequested.iMaxSduSize = block_int->value;
					}
				else if (tmp.Compare(_L8("PriorityRequested"))==0)
					{
					iRequested.iPriority = (TUmtsTrafficHandlingPriority)block_int->value;
					}
				else if (tmp.Compare(_L8("ResidualBerRequested"))==0)
					{
					iRequested.iResidualBer = (TUmtsBitErrorRatio)block_int->value;
					}
				else if (tmp.Compare(_L8("TrafficClassRequested"))==0)
					{
					iRequested.iTrafficClass = (TUmtsTrafficClass)block_int->value;
					}
				else if (tmp.Compare(_L8("TransferDelayRequested"))==0)
					{
					iRequested.iTransferDelay = block_int->value;
					}
				else if (tmp.Compare(_L8("SignallingIndicatorRequested"))==0)
					{
					iRequested.iSignallingIndicator = block_int->value;
					}
				else if (tmp.Compare(_L8("SrcStatDescRequested"))==0)
					{
					iRequested.iSrcStatisticsDesc = (TUmtsSourceStatisticsDescriptor)block_int->value;
					}
				else if (tmp.Compare(_L8("HeaderCompression"))==0)
					{
					iNegotiated.iCompression = block_int->value;
					}
				else if (tmp.Compare(_L8("ErrorCode"))==0)
					{
					iNegotiated.iErrorCode = block_int->value;
					}
				}
			p += block_len;
			length -= block_len;
			}
		}
	return KErrNone;
	}

EXPORT_C CExtensionBase* CUmtsR5QoSPolicy::CreateL()
	{
	CUmtsR5QoSPolicy *extension = CUmtsR5QoSPolicy::NewL();
	return extension;
	}

EXPORT_C TInt CUmtsR5QoSPolicy::Copy(const CExtensionBase& aExtension)
	{
	if (aExtension.Type() != iType)
		return KErrArgument;
	const CUmtsR5QoSPolicy& policy = (const CUmtsR5QoSPolicy&)aExtension;
	policy.GetQoSMinimum(iMinimum);
	policy.GetQoSRequested(iRequested);
	policy.GetQoSNegotiated(iNegotiated);
	iCompression = policy.HeaderCompression();
	return KErrNone;
	}

EXPORT_C void CUmtsR5QoSPolicy::SetQoSRequested(const TUmtsR5QoSParameters& aRequested)
	{
    
   	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_1, "<------------------------------------------------\n");
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_2, "CUmtsR5QoSPolicy::SetQoSRequested");
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_3, "\n");
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_4, "REQUESTED R5 VALUES SUPPLIED BY CLIENT IS \n");
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_5, "\n");
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_6, " [aRequested.iTrafficClass           = %d]\n",aRequested.iTrafficClass);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_7, " [aRequested.iDeliveryOrder          = %d]\n",aRequested.iDeliveryOrder);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_8, " [aRequested.iDeliveryOfErroneusSdu = %d]\n",aRequested.iDeliveryOfErroneusSdu);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_9, " [aRequested.iMaxSduSize             = %d]\n",aRequested.iMaxSduSize);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_10, " [aRequested.iMaxBitrateUplink      = %d]\n",aRequested.iMaxBitrateUplink);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_11, " [aRequested.iMaxBitrateDownlink    = %d]\n",aRequested.iMaxBitrateDownlink);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_12, " [aRequested.iResidualBer           = %d]\n",aRequested.iResidualBer);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_13, " [aRequested.iErrorRatio            = %d]\n",aRequested.iErrorRatio);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_14, " [aRequested.iPriority              = %d]\n",aRequested.iPriority);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_15, " [aRequested.iTransferDelay         = %d]\n",aRequested.iTransferDelay);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_16, " [aRequested.iGuaBitrateUplink      = %d]\n",aRequested.iGuaBitrateUplink);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_17, " [aRequested.iGuaBitrateDownlink    = %d]\n",aRequested.iGuaBitrateDownlink);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_18, " [aRequested.iSignallingIndicator   = %d]\n",aRequested.iSignallingIndicator);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_19, " [aRequested.iSrcStatisticsDesc     = %d]\n",aRequested.iSrcStatisticsDesc);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSREQUESTED_20, "------------------------------------------------>\n");
	
	iRequested = aRequested;
	}

EXPORT_C void CUmtsR5QoSPolicy::SetQoSMinimum(const TUmtsR5QoSParameters& aMinimum)
	{
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_1, "<------------------------------------------------\n");
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_2, "CUmtsR5QoSPolicy::SetQoSMinimum");
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_3, "\n");
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_4, "MINIMUM R5 VALUES SUPPLIED BY CLIENT IS \n");
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_5, "\n");
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_6, " [aMinimum.iTrafficClass           = %d]\n",aMinimum.iTrafficClass);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_7, " [aMinimum.iDeliveryOrder          = %d]\n",aMinimum.iDeliveryOrder);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_8, " [aMinimum.iDeliveryOfErroneusSdu  = %d]\n",aMinimum.iDeliveryOfErroneusSdu);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_9, " [aMinimum.iMaxSduSize             = %d]\n",aMinimum.iMaxSduSize);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_10, " [aMinimum.iMaxBitrateUplink      = %d]\n",aMinimum.iMaxBitrateUplink);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_11, " [aMinimum.iMaxBitrateDownlink    = %d]\n",aMinimum.iMaxBitrateDownlink);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_12, " [aMinimum.iResidualBer           = %d]\n",aMinimum.iResidualBer);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_13, " [aMinimum.iErrorRatio            = %d]\n",aMinimum.iErrorRatio);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_14, " [aMinimum.iPriority              = %d]\n",aMinimum.iPriority);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_15, " [aMinimum.iTransferDelay         = %d]\n",aMinimum.iTransferDelay);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_16, " [aMinimum.iGuaBitrateUplink      = %d]\n",aMinimum.iGuaBitrateUplink);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_17, " [aMinimum.iGuaBitrateDownlink    = %d]\n",aMinimum.iGuaBitrateDownlink);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_18, " [aMinimum.iSignallingIndicator   = %d]\n",aMinimum.iSignallingIndicator);
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_19, " [aMinimum.iSrcStatisticsDesc     = %d]\n",aMinimum.iSrcStatisticsDesc);
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CUMTSR5QOSPOLICY_SETQOSMINIMUM_20, "------------------------------------------------>\n");
	
	iMinimum = aMinimum;
	}

EXPORT_C void CUmtsR5QoSPolicy::GetQoSRequested(TUmtsR5QoSParameters& aRequested) const
	{
	aRequested = iRequested;
	}

EXPORT_C void CUmtsR5QoSPolicy::GetQoSMinimum(TUmtsR5QoSParameters& aMinimum) const
	{
	aMinimum = iMinimum;
	}

EXPORT_C void CUmtsR5QoSPolicy::GetQoSNegotiated(TNegotiatedUmtsR5QoSParameters& aNegotiated) const
	{
	aNegotiated = iNegotiated;
	}

EXPORT_C TInt CUmtsR5QoSPolicy::SetHeaderCompression(TUint aCompression)
	{
	if (aCompression & ~(KPdpDataCompression|KPdpHeaderCompression))
		return KErrNotSupported;
	iCompression = aCompression;
	return KErrNone;
	}

EXPORT_C TUint CUmtsR5QoSPolicy::HeaderCompression() const
	{
	return iCompression;
	}

