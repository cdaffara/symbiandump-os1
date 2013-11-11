//  TestTelephonyPktSharedData.inl
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  The inline function implementation of CTestTelephonyPktSharedData

#ifndef TESTTELEPHONYPKTSHAREDDATA_INL_
#define TESTTELEPHONYPKTSHAREDDATA_INL_

// user include
#include "TestLtsyPktSharedData.h"

/**
 * Get the reference of specified packet context
 * aServiceIndex - the index in the packet service array
 * aContextIndex - the index in the packet context array
 * @return - the reference of the RPacketContext object
 */
RPacketContext& CTestLtsyPktSharedData::PacketContext(TInt aServiceIndex, TInt aContextIndex)
	{
	return iServices[aServiceIndex].iContexts[aContextIndex].iPktContext;
	}

/**
 * Get the reference of specified packet QoS profile
 * aServiceIndex - the index in the packet service array
 * aContextIndex - the index in the packet context array
 * @return - the reference of the QoS profile object
 */
RPacketQoS& CTestLtsyPktSharedData::PacketQos(TInt aServiceIndex, TInt aContextIndex)
	{
	return iServices[aServiceIndex].iContexts[aContextIndex].iPktQos;
	}

/**
 * Get the reference of specified packet service
 * aServiceIndex - the index in the packet service array
 * @return - the reference of the RPacketService object
 */
RPacketService& CTestLtsyPktSharedData::PacketService(TInt aServiceIndex)
	{
	return iServices[aServiceIndex].iPktService;
	}

/**
 * Get the reference of specified context data
 * aServiceIndex - the index in the packet service array
 * aContextIndex - the index in the packet context array
 * @return - the reference of the TContextData object
 */
CTestLtsyPktSharedData::TContextData& CTestLtsyPktSharedData::ContextData(TInt aServiceIndex, TInt aContextIndex)
	{
	return iServices[aServiceIndex].iContexts[aContextIndex];
	}

#endif /*TESTTELEPHONYPKTSHAREDDATA_INL_*/

// end of file
