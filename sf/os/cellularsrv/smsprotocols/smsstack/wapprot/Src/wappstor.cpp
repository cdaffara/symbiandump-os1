// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wappstorTraces.h"
#endif

#include "wappstor.h"
#include "ws_main.h"
#include "smsstackutils.h"


_LIT(KStoreName,"wapreast.dat");

const TInt KWapReassemblyStoreUidValue=0x100008CB;
const TUid KWapReassemblyStoreUid={KWapReassemblyStoreUidValue};  //  Used for second uid of SAR stores


CWapReassemblyStore* CWapReassemblyStore::NewL(RFs& aFs)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPREASSEMBLYSTORE_NEWL_1, "CWapReassemblyStore::NewL()");

    CWapReassemblyStore* reassembly=new (ELeave)CWapReassemblyStore(aFs);
   	CleanupStack::PushL(reassembly);
	reassembly->ConstructL();
	CleanupStack::Pop(reassembly);
    return reassembly;
    } // CWapReassemblyStore::NewL


CWapReassemblyStore::~CWapReassemblyStore()
    {
    this->Close();
    }


TBool CWapReassemblyStore::AddMessageL( TInt& aIndex, const CWapDatagram& aDatagram)                                                
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPREASSEMBLYSTORE_ADDMESSAGEL_1, "CWapReassemblyStore::AddMessageL()");

    CArrayPtrFlat<CWapDatagram::TSegmentData>* segmentArray = new
        (ELeave) CArrayPtrFlat<CWapDatagram::TSegmentData> (8);

    CleanupStack::PushL(segmentArray);
	// coverity[double_push]
    CleanupResetAndDestroyPushL(*segmentArray);
    
    TBool isComplete = aDatagram.IsComplete();

    // count of incomplete WAP short messages
    TInt Count = Entries().Count();
    if (!isComplete)
        {
        TWapReassemblyEntry Entry;
        TBool     isFound = EFalse;

        // go through all entries in the reassembly store
        // and find the short message entry,
        // which matches with the given entry
        for(aIndex=0;aIndex<Count; aIndex++)
            {
            TInt ToPort = 0;
            TInt FromPort = 0;
            aDatagram.Ports(FromPort,ToPort);
            Entry = (TWapReassemblyEntry&)Entries()[aIndex];
            isFound =    ((Entry.Reference() ==
                             aDatagram.ConcatenatedMessageReference())
                      && (Entry.Total() ==
                             aDatagram.NumConcatenatedMessages())
                      && (Entry.ToPort() == ToPort)
                      && (Entry.Description1() ==
                             aDatagram.FromAddress()));
            if (isFound)
                break;
            }
        if (isFound)
            {
            isFound = EFalse;

            // new short message fragment received for an existing
            // incomplete WAP datagram
            TStreamId StreamdId = Entry.DataStreamId();
            CWapDatagram* tempDatagram = CWapDatagram::NewL(KNullDesC8);
            CleanupStack::PushL(tempDatagram);

            TRAPD(ret, InternalizeEntryL(StreamdId,*tempDatagram,*segmentArray));
            if(ret == KErrCorrupt)
                {
                Close();
                User::LeaveIfError(iFs.Delete(iFullPathBuf));
                DoOpenL();        //create a new file
                }
            else
                User::LeaveIfError(ret);

            // For the first: discard duplicates
            // It takes place by comparing indexes of TSegmentDatas
            CWapDatagram::TSegmentData *segmentData = new (ELeave)CWapDatagram::TSegmentData;
            CleanupStack::PushL(segmentData);
            aDatagram.SegmentData(*segmentData);

            if(aDatagram.NumConcatenatedMessages() < segmentData->iSegmentNumber)
            {
                isFound = ETrue;                // out of range discard
                CleanupStack::PopAndDestroy(segmentData);
            }
            else
            {
                Count=segmentArray->Count();
                for (TInt i=0; i<Count; i++)
                    {
                    CWapDatagram::TSegmentData* thisSegmentData =
                               segmentArray->At(i);
                if (thisSegmentData->iSegmentNumber
                        == segmentData->iSegmentNumber)
                    {
                    // duplicate found. It is not saved.
                    isFound = ETrue;
                    CleanupStack::PopAndDestroy(segmentData);
                    break;
                    }
                }
            }
            if (!isFound)
                {
                TInt j=0;
                for (; (j<segmentArray->Count()) && (segmentData->iSegmentNumber>(*segmentArray)[j]->iSegmentNumber); j++)
                {
                }
                segmentArray->InsertL(j,segmentData);
                CleanupStack::Pop(segmentData);
                if (segmentArray->Count() ==
                        aDatagram.NumConcatenatedMessages())
                    // all fragments of a datagram are available
                    isComplete = ETrue;

                BeginTransactionLC();
                ExternalizeEntryL(StreamdId,*tempDatagram,*segmentArray);
                PopulateEntry(Entry,*tempDatagram,segmentArray->Count());
                ChangeEntryL(aIndex,Entry);
                CommitTransactionL();
                }
            CleanupStack::PopAndDestroy(tempDatagram);
            }
            // else - a duplicate was found. Ignored.
        else
            {
            // a first short message fragment received for a
            // non-existing WAP datagram
            CWapDatagram::TSegmentData *segmentData = new (ELeave)CWapDatagram::TSegmentData;
            CleanupStack::PushL(segmentData);
            aDatagram.SegmentData(*segmentData);
            if(aDatagram.NumConcatenatedMessages() < segmentData->iSegmentNumber)
                {
                CleanupStack::PopAndDestroy(segmentData);
                isComplete=EFalse;
                }
            else
                {
                segmentArray->AppendL(segmentData);
                CleanupStack::Pop(segmentData);
                    aIndex = Count;
                CreateEntryL(aDatagram,*segmentArray);
                }
            }
        }
    else // the datagram is complete
        {
        CWapDatagram::TSegmentData *segmentData = new (ELeave)CWapDatagram::TSegmentData;
        CleanupStack::PushL(segmentData);
        aDatagram.SegmentData(*segmentData);
        if(aDatagram.NumConcatenatedMessages() < segmentData->iSegmentNumber)
            {
            CleanupStack::PopAndDestroy(segmentData);
            isComplete=EFalse;
            }
        else
            {
            segmentArray->AppendL(segmentData);
            CleanupStack::Pop(segmentData);
            aIndex = Count;
            CreateEntryL(aDatagram,*segmentArray);
            }
        }

    CleanupStack::PopAndDestroy(2, segmentArray);  // segmentArray elements (Reset and Destroy), segmentArray
    return isComplete;
    } // CWapReassemblyStore::AddMessageL

void CWapReassemblyStore::GetDatagramL( TInt            aIndex,
                                                CWapDatagram&   aDatagram)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPREASSEMBLYSTORE_GETDATAGRAML_1, "CWapReassemblyStore::GetDatagramL()");

    CArrayPtrFlat<CWapDatagram::TSegmentData>* segmentArray = new
        (ELeave) CArrayPtrFlat<CWapDatagram::TSegmentData> (8);
    
    // here we need to push 'segmentArray' pointer to the cleanup stack, since it's a heap allocation (pointer must be deleted)
    // CleanupResetAndDestroyPushL() just trigers ResetAndDestroy() to be called on CleanupStack::PopAndDestroy()     
    CleanupStack::PushL(segmentArray);
	// coverity[double_push]
    CleanupResetAndDestroyPushL(*segmentArray);

    // defect fix for EDNJJUN-4WYJGP
    // Unable to send sms cause sms*.dat is corrupted
    TRAPD(ret, InternalizeEntryL(Entries()[aIndex].DataStreamId(), aDatagram,*segmentArray));
    if(ret == KErrCorrupt)
        {
        Close();             //because the file is in use
        User::LeaveIfError(iFs.Delete(iFullPathBuf));
        DoOpenL();        //create a new file
        }
    else
        User::LeaveIfError(ret);

    if(aDatagram.Alphabet() == TSmsDataCodingScheme::ESmsAlphabet7Bit)
        {
        aDatagram.DecodeConcatenatedMessagesL(*segmentArray);
        }

    CleanupStack::PopAndDestroy(2, segmentArray);  // segmentArray elements (Reset and Destroy), segmentArray
    } // CWapReassemblyStore::GetDatagramL

TBool CWapReassemblyStore::FindAndDeleteDatagramL( CWapDatagram& aDatagram)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPREASSEMBLYSTORE_FINDANDDELETEDATAGRAML_1, "CWapReassemblyStore::FindAndDeleteDatagramL()");

	TInt index;
	TBool isFound = EFalse;
	TWapReassemblyEntry entry;
	TInt toPort = 0;
    TInt fromPort = 0;
    aDatagram.Ports(fromPort,toPort);

	TInt Count = Entries().Count();
	for(index=0;index<Count; index++)
            {
            entry = (TWapReassemblyEntry&)Entries()[index];
            isFound =    ((entry.Reference() ==
                             aDatagram.ConcatenatedMessageReference())
                      && (entry.ToPort() == toPort)
					  &&(entry.Total() ==
                             aDatagram.NumConcatenatedMessages()));
			if (isFound)
				{
				BeginTransactionLC();
				DeleteEntryL(index);
				CommitTransactionL();
				return isFound;
				}
			}
	return isFound;
	} // CWapReassemblyStore::FindAndDeleteDatagramL


void CWapReassemblyStore::ConstructL()
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPREASSEMBLYSTORE_CONSTRUCTL_1, "CWapReassemblyStore::ConstructL()");

    //get full path of reassembly store
    PrivatePath(iFullPathBuf);
	//append store name
	iFullPathBuf.Append(KStoreName);
    OpenStoreL();
    } // CWapReassemblyStore::ConstructL


/**
 *  internalize all the entries from the permanent file store to internal memory
 *  
 *  @note You have to call CSARStore::OpenFileLC() before calling this function
 *  @param aStreamId, unique id associated with the stream
 *  @param aDatagram, the datagram which will be internalized
 *  @param aSegmentArray, the array of segments for the datagram
 */
void CWapReassemblyStore::InternalizeEntryL(
                            TStreamId                   aStreamId,
                            CWapDatagram&               aDatagram,
                            CArrayPtr<CWapDatagram::TSegmentData>&  aSegmentArray)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPREASSEMBLYSTORE_INTERNALIZEENTRYL_1, "CWapReassemblyStore::InternalizeEntryL Start");

	BeginTransactionLC();
    RStoreReadStream ReadStream;
    TInt32 Count;

    ReadStream.OpenLC(FileStore(),aStreamId);
    ReadStream >> aDatagram;

	if(aDatagram.Alphabet() == TSmsDataCodingScheme::ESmsAlphabet8Bit)
		{
		aDatagram.InternalizeBufferL(ReadStream);
		}
    else
		{
		Count=ReadStream.ReadInt32L();

		aSegmentArray.Reset();
	    for (TInt i=0; i<Count; i++)
		    {
			CWapDatagram::TSegmentData* Segment = new (ELeave) CWapDatagram::TSegmentData;
			CleanupStack::PushL(Segment);
			aSegmentArray.AppendL(Segment);
			CleanupStack::Pop();

			Segment->iSegmentNumber = ReadStream.ReadInt32L();
			ReadStream >> Segment->iData;
			}
		}
    // Closes the ReadStream
    CleanupStack::PopAndDestroy();
	CommitTransactionL();
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPREASSEMBLYSTORE_INTERNALIZEENTRYL_2, "CWapReassemblyStore::InternalizeEntryL End");
    } // CWapReassemblyStore::InternalizeEntryL


/**
 *  externalizes all the entries from the internal memory to the permanent file store
 *  
 *  @note You have to call CSARStore::OpenFileLC() before calling this function
 *  @param aStreamId, unique id associated with the stream
 *  @param aDatagram, the datagram which should be externalized
 *  @param aSegmentArray, the array of segments for the datagram
 */
void CWapReassemblyStore::ExternalizeEntryL(
                            TStreamId&          aStreamId,
                            const CWapDatagram& aDatagram,
                            const CArrayPtr<CWapDatagram::TSegmentData>& aSegmentArray)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPREASSEMBLYSTORE_EXTERNALIZEENTRYL_1, "CWapReassemblyStore::ExternalizeEntryL Start");
    
    TInt32 Count = aSegmentArray.Count();
    RStoreWriteStream WriteStream;

    if (aStreamId==KNullStreamId)
        aStreamId=WriteStream.CreateLC(FileStore());
    else
        WriteStream.ReplaceLC(FileStore(),aStreamId);
    WriteStream<<aDatagram;

	if(aDatagram.Alphabet() == TSmsDataCodingScheme::ESmsAlphabet8Bit)
		{
		aDatagram.ExternalizeBufferL(WriteStream);
		}
    else
		{
		WriteStream.WriteInt32L(Count);
		for(TInt i=0; i<Count; i++)
			{
			WriteStream.WriteInt32L(aSegmentArray[i]->iSegmentNumber);
			WriteStream<<aSegmentArray[i]->iData;
			}

		}
	// Closes the ReadStream
    WriteStream.CommitL();
    CleanupStack::PopAndDestroy();
    } // CWapReassemblyStore::ExternalizeEntryL


void CWapReassemblyStore::PopulateEntry(TWapReassemblyEntry& aEntry,
                                        const CWapDatagram& aDatagram,
                                        TInt                aNumDatagrams)
    {
    OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPREASSEMBLYSTORE_POPULATEENTRY_1, "CWapReassemblyStore::PopulateEntry()");

    TInt ToPort = 0;
    TInt FromPort = 0;

    aDatagram.Ports(FromPort,ToPort);
            // complete or not complete
    if (!aDatagram.IsComplete())
        {
        aEntry.SetReference(aDatagram.ConcatenatedMessageReference());
        aEntry.SetTotal(aDatagram.NumConcatenatedMessages());
        aEntry.SetCount(aNumDatagrams);
        }
        else
        {
		//Wap Datagram might contain reference number which will be 
		//needed at the time of deleting the datagram from permanent store file
		//Refer To Defect Fix: PDEF114607.
		aEntry.SetReference(aDatagram.ConcatenatedMessageReference());
		aEntry.SetTotal(1);
		aEntry.SetCount(1);
        }

    aEntry.SetToPort(ToPort);
    aEntry.SetDescription1(aDatagram.FromAddress());
    aEntry.SetTime(aDatagram.Time());
    } // CWapReassemblyStore::PopulateEntry


void CWapReassemblyStore::CreateEntryL(const CWapDatagram&                 aDatagram,
                                       const CArrayPtr<CWapDatagram::TSegmentData>& aSegmentArray)
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPREASSEMBLYSTORE_CREATEENTRYL_1, "CWapReassemblyStore::CreateEntryL");

    TWapReassemblyEntry Entry;
    TStreamId WriteStream = KNullStreamId;
	BeginTransactionLC();
    ExternalizeEntryL(WriteStream,aDatagram,aSegmentArray);
    Entry.SetDataStreamId(WriteStream);
    PopulateEntry(Entry,aDatagram,aDatagram.NumConcatenatedMessages());
    AddEntryL(Entry);
	CommitTransactionL();
	} // CWapReassemblyStore::CreateEntryL


CWapReassemblyStore::CWapReassemblyStore(RFs& aFs)
    :CSARStore(aFs)
    {
    } // CWapReassemblyStore::CWapReassemblyStore


/**
 *  Open the wap reassembly store.
 *  Use RFs::PrivatePath to generate private path.
 */
void CWapReassemblyStore::OpenStoreL()
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CWAPREASSEMBLYSTORE_OPENSTOREL_1, "CWapReassemblyStore::OpenStoreL()");

	OpenL(iFullPathBuf,KWapReassemblyStoreUid);
	} // CWapReassemblyStore::OpenStoreL

// EOF - WAPPSTOR.CPP
