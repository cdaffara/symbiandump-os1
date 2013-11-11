// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>
#include "L2CapDebugControlInterface.h"

#include <bt_subscribe.h>
#include "l2util.h"
#include "BTSec.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

CDebugControlInterface::CDebugControlInterface()
 : iSDUFlushQueuedSDUFlushed(0),
   iSDUFlushPartialSentSDUFlushed(0),
   iSDUFlushSentSDUFlushed(0),
   iFlushedPDUMuxerResend(0),
   iFlushedPDUDataFlowResend(0),
   iFlushedPDUIFrame(0)
	{
	LOG_FUNC
	for(TInt i=0;i<EMaxType;i++)
		{
		for(TInt j=0;j<EMaxAllocationType;j++)
			{
			iAllocations[i][j] = NULL;
			}
		}
	DefinePublications();
	}

CDebugControlInterface::~CDebugControlInterface()
	{
	LOG_FUNC
	DeletePublications();

	iPDUTimerRecords.ResetAndDestroy();	
	iPDUTimerRecords.Close();
	iPDUGroupTimerRecords.ResetAndDestroy();
	iPDUGroupTimerRecords.Close();
	}
	
	
/*static*/ CDebugControlInterface* CDebugControlInterface::NewL()
	{
	LOG_STATIC_FUNC
	// This is done this way to allow future enhancement of this class.
	CDebugControlInterface* self = new(ELeave) CDebugControlInterface();
	return self;
	}

void CDebugControlInterface::DefinePublications()
	{
	LOG_FUNC

//Capabilities don't really matter here because these
//keys are only used in debug builds
_LIT_SECURITY_POLICY_PASS(KPassPolicy);

	for(TInt i=0;i<EMaxType;i++)
		{
		for(TInt j=0;j<EMaxAllocationType;j++)
			{
			(void)(iProperty.Define(KPropertyKeyBluetoothL2CapDebugBase + (i * 0x10) + j,
									RProperty::EInt,
									KPassPolicy,
									KPassPolicy));
			}
		}
	(void)(iProperty.Define(KPropertyKeyBluetoothL2DBGGetTimerGrpCID,
							RProperty::EInt,
							KPassPolicy,
							KPassPolicy));

	(void)(iProperty.Define(KPropertyKeyBluetoothL2DBGMinPDUGetTime,
							RProperty::EInt,
							KPassPolicy,
							KPassPolicy));
							
	(void)(iProperty.Define(KPropertyKeyBluetoothL2DBGMaxPDUGetTime,
							RProperty::EInt,
							KPassPolicy,
							KPassPolicy));

	(void)(iProperty.Define(KPropertyKeyBluetoothL2DBGMinPDUSentTime,
							RProperty::EInt,
							KPassPolicy,
							KPassPolicy));
							
	(void)(iProperty.Define(KPropertyKeyBluetoothL2DBGMaxPDUSentTime,
							RProperty::EInt,
							KPassPolicy,
							KPassPolicy));

	(void)(iProperty.Define(KPropertyKeyBluetoothL2DBGAvePDUGetTime,
							RProperty::EInt,
							KPassPolicy,
							KPassPolicy));
							
	(void)(iProperty.Define(KPropertyKeyBluetoothL2DBGAvePDUSentTime,
							RProperty::EInt,
							KPassPolicy,
							KPassPolicy));

	(void)(iProperty.Define(KPropertyKeyBluetoothQueuedSDUFlushedCounter,
							RProperty::EInt,
							KPassPolicy,
							KPassPolicy));
							
	(void)(iProperty.Define(KPropertyKeyBluetoothPartialSentSDUFlushedCounter,
							RProperty::EInt,
							KPassPolicy,
							KPassPolicy));
							
	(void)(iProperty.Define(KPropertyKeyBluetoothSentSDUFlushedCounter,
							RProperty::EInt,
							KPassPolicy,
							KPassPolicy));
							
	(void)(iProperty.Define(KPropertyKeyBluetoothFlushedPDUMuxerResend,
							RProperty::EInt,
							KPassPolicy,
							KPassPolicy));
							
	(void)(iProperty.Define(KPropertyKeyBluetoothFlushedPDUDataFlowResend,
							RProperty::EInt,
							KPassPolicy,
							KPassPolicy));
							
	(void)(iProperty.Define(KPropertyKeyBluetoothFlushedPDUIFrame,
							RProperty::EInt,
							KPassPolicy,
							KPassPolicy));
							
	(void)(iProperty.Define(KPropertyKeyBluetoothIncompleteErroredSDUReceived,
							RProperty::EInt,
							KPassPolicy,
							KPassPolicy));
	}

void CDebugControlInterface::DeletePublications()
	{
	LOG_FUNC
	TSecureId thisSID = RProcess().SecureId();
	
	for(TInt i=0;i<EMaxType;i++)
		{
		for(TInt j=0;j<EMaxAllocationType;j++)
			{
			iProperty.Delete(thisSID, 
			                 KPropertyKeyBluetoothL2CapDebugBase + (i * 0x10) + j);
			}
		}

	iProperty.Delete(thisSID, KPropertyKeyBluetoothL2DBGGetTimerGrpCID);

	iProperty.Delete(thisSID, KPropertyKeyBluetoothL2DBGMinPDUGetTime);
	iProperty.Delete(thisSID, KPropertyKeyBluetoothL2DBGMaxPDUGetTime);

	iProperty.Delete(thisSID, KPropertyKeyBluetoothL2DBGMinPDUSentTime);
	iProperty.Delete(thisSID, KPropertyKeyBluetoothL2DBGMaxPDUSentTime);

	iProperty.Delete(thisSID, KPropertyKeyBluetoothL2DBGAvePDUGetTime);
	iProperty.Delete(thisSID, KPropertyKeyBluetoothL2DBGAvePDUSentTime);


	iProperty.Delete(thisSID, KPropertyKeyBluetoothQueuedSDUFlushedCounter);
	iProperty.Delete(thisSID, KPropertyKeyBluetoothPartialSentSDUFlushedCounter);
	iProperty.Delete(thisSID, KPropertyKeyBluetoothSentSDUFlushedCounter);
	iProperty.Delete(thisSID, KPropertyKeyBluetoothFlushedPDUMuxerResend);
	iProperty.Delete(thisSID, KPropertyKeyBluetoothFlushedPDUDataFlowResend);
	iProperty.Delete(thisSID, KPropertyKeyBluetoothFlushedPDUIFrame);
	iProperty.Delete(thisSID, KPropertyKeyBluetoothIncompleteErroredSDUReceived);
	}

		
// Debug Interface.
void CDebugControlInterface::ObjectAllocation(TObjectType aObjectType, TAllocationType aAllocType)
	{
	LOG_FUNC
	TSecureId thisSID = RProcess().SecureId();
	
	iAllocations[aObjectType][aAllocType]++;
	(void)iProperty.Set(thisSID,
						KPropertyKeyBluetoothL2CapDebugBase + (aObjectType * 0x10) + aAllocType,
						iAllocations[aObjectType][aAllocType]);
	
	
	if(aAllocType == EAllocated && 
	                 (iAllocations[aObjectType][EAllocated] - iAllocations[aObjectType][EDeleted]) > iAllocations[aObjectType][EPeak])
		{
		iAllocations[aObjectType][EPeak] = static_cast<TUint16>(iAllocations[aObjectType][EAllocated] - iAllocations[aObjectType][EDeleted]);
		(void)iProperty.Set(thisSID,
							KPropertyKeyBluetoothL2CapDebugBase + (aObjectType * 0x10) + EPeak,
							iAllocations[aObjectType][EPeak]);
		}
	}

void CDebugControlInterface::PDUTimer(TPDUTimerEvent aEvent, TAny* aUID, TUint16 aGrpID)
	{
	LOG_FUNC
	TPDUTimerRecord* rec = NULL;
	TInt rerr = KErrNone;
	
	switch(aEvent)
		{
		case EBFrameCreated:
			rec = new TPDUTimerRecord(aUID, aGrpID, TPDUTimerRecord::EPDUBFrameTimer);
			if(rec)
				{
				rerr = iPDUTimerRecords.Append(rec);
				}
			else
				{
				rerr = KErrNoMemory;
				}
			break;				

		case EIFrameCreated:
			rec = new TPDUTimerRecord(aUID, aGrpID, TPDUTimerRecord::EPDUIFrameTimer);
			if(rec)
				{
				rerr = iPDUTimerRecords.Append(rec);
				}
			else
				{
				rerr = KErrNoMemory;
				}
			break;				
			
		case EUnsegmentedFrameCreated:
			rec = new TPDUTimerRecord(aUID, aGrpID, TPDUTimerRecord::EPDUUnsegmentedFrameTimer);
			if(rec)
				{
				rerr = iPDUTimerRecords.Append(rec);
				}
			else
				{
				rerr = KErrNoMemory;
				}
			break;				

		case EGetPDUCalled:
			{
			TIdentityRelation<TPDUTimerRecord> relation(UIDLinkMatch);
			TPDUTimerRecord temp(aUID, 0, TPDUTimerRecord::TPDUTimerType(0));
			TInt ix = iPDUTimerRecords.Find(&temp, relation);
			if(ix != KErrNotFound)
				{
				iPDUTimerRecords[ix]->MarkGetPDUCalled();
				}
			else
				{
				rerr = KErrNotFound;
				}
			}
			break;				

		case EPDUSent:
			{
			TPDUTimerRecord* uidRec = NULL;
			TIdentityRelation<TPDUTimerRecord> relation(UIDLinkMatch);
			TPDUTimerRecord temp(aUID, 0, TPDUTimerRecord::TPDUTimerType(0));
			TInt ix = iPDUTimerRecords.Find(&temp, relation);
			if(ix != KErrNotFound)
				{
				uidRec = iPDUTimerRecords[ix];
				uidRec->MarkPDUSent();

				TIdentityRelation<TPDUGroupTimerRecord> relGroup(GroupLinkMatch);
				TPDUGroupTimerRecord tempGrp(aGrpID, iProperty);
				TInt grpIx = iPDUGroupTimerRecords.Find(&tempGrp, relGroup);
				if(grpIx == KErrNotFound)
					{					
					TPDUGroupTimerRecord* grpRecord = new TPDUGroupTimerRecord(aGrpID, iProperty);
					if(grpRecord)
						{
						rerr = iPDUGroupTimerRecords.Append(grpRecord);
						grpIx = iPDUGroupTimerRecords.Count() - 1;
						}
					else
						{
						rerr = KErrNoMemory;
						}
					}
				if(rerr == KErrNone)
					{
					iPDUGroupTimerRecords[grpIx]->UpdateGroup(uidRec->GetPDUTI(),
														      uidRec->SentPDUTI());			
					}

				iPDUTimerRecords.Remove(ix);
				delete uidRec;
				}
			else
				{
				rerr = KErrNotFound;
				}
			}
			break;
			
		default:
			Panic(EL2CAPUpdatePDUTimerDebugFailed);
			break;
		};
	}

void CDebugControlInterface::UpdateFlushCounters(TFlushCountersEvent aEvent)
	{
	LOG_FUNC
	TSecureId thisSID = RProcess().SecureId();
	
	switch(aEvent)
		{
		case ESDUFlushQueuedSDUFlushed:
			iSDUFlushQueuedSDUFlushed++;
			(void)(iProperty.Set(thisSID,
								 KPropertyKeyBluetoothQueuedSDUFlushedCounter,
								 iSDUFlushQueuedSDUFlushed));
			break;	

		case ESDUFlushPartialSentSDUFlushed:
			iSDUFlushPartialSentSDUFlushed++;
			(void)(iProperty.Set(thisSID,
								 KPropertyKeyBluetoothPartialSentSDUFlushedCounter,
								 iSDUFlushPartialSentSDUFlushed));
			break;	

		case ESDUFlushSentSDUFlushed:
			iSDUFlushSentSDUFlushed++;
			(void)(iProperty.Set(thisSID,
								 KPropertyKeyBluetoothSentSDUFlushedCounter,
								 iSDUFlushSentSDUFlushed));
			break;	

		case EFlushedPDUMuxerResend:
			iFlushedPDUMuxerResend++;
			(void)(iProperty.Set(thisSID,
								 KPropertyKeyBluetoothFlushedPDUMuxerResend,
								 iFlushedPDUMuxerResend));
			break;	

		case EFlushedPDUDataFlowResend:
			iFlushedPDUDataFlowResend++;
			(void)(iProperty.Set(thisSID,
								 KPropertyKeyBluetoothFlushedPDUDataFlowResend,
								 iFlushedPDUDataFlowResend));
			break;	

		case EFlushedPDUIFrame:
			iFlushedPDUIFrame++;
			(void)(iProperty.Set(thisSID,
								 KPropertyKeyBluetoothFlushedPDUIFrame,
								 iFlushedPDUIFrame));
			break;	

		case EIncompleteErroredSDUReceived:
			iIncompleteErroredSDUReceived++;
			(void)(iProperty.Set(thisSID,
								 KPropertyKeyBluetoothIncompleteErroredSDUReceived,
								 iIncompleteErroredSDUReceived));
			break;
			
		default:
			Panic(EL2CAPInvalidDebugFlushCounterEvent);
			break;	
		};
	}
	
		
/*static*/ TBool CDebugControlInterface::UIDLinkMatch(const TPDUTimerRecord& aA, 
                                                      const TPDUTimerRecord& aB)
	{
	LOG_STATIC_FUNC
	return aA.GetUID() == aB.GetUID();
	}

/*static*/ TBool CDebugControlInterface::GroupLinkMatch(const TPDUGroupTimerRecord& aA, 
                 	                                    const TPDUGroupTimerRecord& aB)
	{
	LOG_STATIC_FUNC
	return aA.GetGroupID() == aB.GetGroupID();
	}


TPDUTimerRecord::TPDUTimerRecord(TAny* aUID, TUint16 aGroupID, TPDUTimerType aTimerType)
 : iUID(aUID),
   iGroupID(aGroupID),
   iTimerType(aTimerType)
	{
	LOG_FUNC
	iTime.HomeTime();
	}
	
void TPDUTimerRecord::MarkGetPDUCalled()
	{
	LOG_FUNC
	TTime temp;
	temp.HomeTime();
	iGetPDUTI = temp.MicroSecondsFrom(iTime);	
	}
	
void TPDUTimerRecord::MarkPDUSent()
	{
	LOG_FUNC
	TTime temp;
	temp.HomeTime();
	iSentPDUTI = temp.MicroSecondsFrom(iTime);		
	}



TPDUGroupTimerRecord::TPDUGroupTimerRecord(TUint16 aGroupID, RProperty& aProperty)
 : iMinGetPDU(0),
   iMinSentPDU(0),
   iMaxGetPDU(0),
   iMaxSentPDU(0),
   iAveGetPDU(0),
   iAveSentPDU(0),
   iSampleSize(0),
   iGroupID(aGroupID),
   iProperty(aProperty)
	{
	LOG_FUNC
	}

void TPDUGroupTimerRecord::UpdateGroup(const TTimeIntervalMicroSeconds& aGetPDUTime,
					 				   const TTimeIntervalMicroSeconds& aSentPDUTime)
	{
	LOG_FUNC
	// Covert the arguments to ms intervals.
	TInt newGetPDUTime = I64INT(aGetPDUTime.Int64()) / 1000;
	TInt newSentPDUTime = I64INT(aSentPDUTime.Int64()) / 1000;

	if(newGetPDUTime < iMinGetPDU || iSampleSize == 0)
		{
		iMinGetPDU = newGetPDUTime;
		}
	if(newGetPDUTime > iMaxGetPDU || iSampleSize == 0)
		{
		iMaxGetPDU = newGetPDUTime;
		}

	if(newSentPDUTime < iMinSentPDU || iSampleSize == 0)
		{
		iMinSentPDU = newSentPDUTime;
		}
	if(newSentPDUTime > iMaxSentPDU || iSampleSize == 0)
		{
		iMaxSentPDU = newSentPDUTime;
		}

	iAveGetPDU = ((iAveGetPDU * iSampleSize) + newGetPDUTime) / (iSampleSize + 1);
	iAveSentPDU = ((iAveSentPDU * iSampleSize) + newSentPDUTime) / (iSampleSize + 1);
	++iSampleSize;

	TInt cid = 0;
	TSecureId thisSID = RProcess().SecureId();
	
	if(iProperty.Get(thisSID,
					 KPropertyKeyBluetoothL2DBGSetTimerGrpCID,
					 cid) == KErrNone)
		{
		if(cid == iGroupID)
			{
			(void)iProperty.Set(thisSID,
			                    KPropertyKeyBluetoothL2DBGGetTimerGrpCID,				
								iGroupID);

			(void)iProperty.Set(thisSID,
			                    KPropertyKeyBluetoothL2DBGMinPDUGetTime,				
								iMinGetPDU);
			(void)iProperty.Set(thisSID,
			                    KPropertyKeyBluetoothL2DBGMaxPDUGetTime,				
								iMaxGetPDU);

			(void)iProperty.Set(thisSID,
			                    KPropertyKeyBluetoothL2DBGMinPDUSentTime,				
								iMinSentPDU);
			(void)iProperty.Set(thisSID,
			                    KPropertyKeyBluetoothL2DBGMaxPDUSentTime,				
								iMaxSentPDU);

			(void)iProperty.Set(thisSID,
			                    KPropertyKeyBluetoothL2DBGAvePDUGetTime,				
								iAveGetPDU);
			(void)iProperty.Set(thisSID,
			                    KPropertyKeyBluetoothL2DBGAvePDUSentTime,				
								iAveSentPDU);

			}
		}								
	}
