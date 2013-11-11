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

#include <cfutil.h>
#include <cfextras.h>
#include <elements/cfmacro.h>
#include <cflog.h>
#include <cfforwardmsg.h>



#ifdef _DEBUG
// Panic category for "absolutely impossible!" vanilla ASSERT()-type panics from this module
// (if it could happen through user error then you should give it an explicit, documented, category + code)
_LIT(KSpecAssert_ElemCommsFwcftl, "ElemCommsFwcftl.");
#endif

using namespace CommsFW;

EXPORT_C TInt CCFModuleChannelHandler::Send(const CommsFW::TCFMessage &aMessage)
/** Send a message on the outgoing channel, blocks until the message has been sent.
The rootserver is expected to run at high priority and retrieve pending messages
quickly, also this exchange usually only happens at module startup and shutdown so
it can usually be considered safe to block here.
@param aMessage Message to send on the outgoing channel.
@publishedPartner
@released
*/
    {
	TRequestStatus status;
	iChannelPair.NotifySpaceAvailable(status);
	User::WaitForRequest(status);
	return iChannelPair.Send(aMessage);
    }

EXPORT_C CCFModuleChannelHandler::~CCFModuleChannelHandler()
/** D'tor. Dequeues this instance from the active scheduler.
@publishedPartner
@released
*/
    {
	Cancel();
    }

EXPORT_C CCFModuleChannelHandler::CCFModuleChannelHandler(TInt aPriority):CActive(aPriority)
/** C'tor. Initialise the parent CActive.
@publishedPartner
@released
*/
    {
	// Do nothing
    }

EXPORT_C /*virtual*/ void CCFModuleChannelHandler::CFMessageDiscover(const CommsFW::TCFDiscoverMsg& aMessage)
/** Default implementation of the TCFDiscoverMsg message. It will send a response indicating 
there is no sub-modules in this module. Should be overridden if sub-modules are present in the module.
@param aMessage The Discovery message which has been received from the Rootserver.
@publishedPartner
@released
*/
    {
	// Default is no submodules, so we just return KErrNone
	TCFDiscoverRespMsg resp(aMessage.Identifier(), KErrNone, EFalse);
	TInt err = Send(resp);
	if(KErrNone != err)
		{
		__CFLOG_1(KLogCommsFw, KLogWarning, _L8("CCFModuleChannelHandler::CFMessageDiscover(): Send returned %d"), err);
		CFChannelError(err);
		}
    }

EXPORT_C /*virtual*/ void CCFModuleChannelHandler::CFMessageBind(const CommsFW::TCFBindMsg& aMessage)
/** Default implementation of the TCFBindMsg message. It will send a response indicating 
there is no submodules to bind to in this module. Should be overridden if sub-modules are present 
in the module.
@param aMessage The Bind message which has been received from the Rootserver.
@publishedPartner
@released
*/
    {
	// Default is no submodules, so we just return KErrNotFound
	TCFBindCompleteMsg resp(aMessage.Identifier(), KErrNotFound);
	TInt err = Send(resp);
	if(KErrNone != err)
		{
		__CFLOG_1(KLogCommsFw, KLogWarning, _L8("CCFModuleChannelHandler::CFMessageBind(): Send returned %d"), err);
		CFChannelError(err);
		}
    }

EXPORT_C /*virtual*/ void CCFModuleChannelHandler::CFMessageUnbind(const CommsFW::TCFUnbindMsg& aMessage)
/** Default implementation of the TCFUnbindMsg message. It will send a response indicating 
there is no submodules to unbind from in this module. Should be overridden if sub-modules are present 
in the module.
@param aMessage The Unbind message which has been received from the Rootserver.
@publishedPartner
@released
*/
    {
	// Default is no submodules, so we just return KErrNotFound
	TCFUnbindCompleteMsg resp(aMessage.Identifier(), KErrNotFound);
	TInt err = Send(resp);
	if(KErrNone != err)
		{
		__CFLOG_1(KLogCommsFw, KLogWarning, _L8("CCFModuleChannelHandler::CFMessageUnbind(): Send returned %d"), err);
		CFChannelError(err);
		}
    }

EXPORT_C /*virtual*/ void CCFModuleChannelHandler::CFMessageShutdown(const CommsFW::TCFShutdownMsg& aMessage)
/** Default implementation of the TCFShutdownMsg message. it will do nothing. Should be overridden 
if the module supports Shutdown.
@param aMessage The Shutdown message which has been received from the Rootserver.
@publishedPartner
@released
*/
    {
	(void) aMessage; // Avoid compiler warnings
    }

EXPORT_C /*virtual*/ void CCFModuleChannelHandler::CFMessageForward(const CommsFW::TCFForwardMsg& aMessage)
/** Default implementation of the CFMessageForward message. it will do nothing. Should be overridden 
if the module supports Message Forwarding.
@param aMessage The Forward message which has been received from the Rootserver.
@publishedPartner
@released
*/
    {
    aMessage.Msg().Complete( KErrNotSupported );
    }

EXPORT_C /*virtual*/ void CCFModuleChannelHandler::CFMessageUnknown(const CommsFW::TCFMessage& aMessage)
/** Default implementation for handling unknown messages. it will do nothing. Should be overridden 
if the module wants to implement some handling for receiving unknown messages.
@param aMessage The message which has been received from the Rootserver.
@publishedPartner
@released
*/
    {
	(void) aMessage; // Avoid compiler warnings
    }

EXPORT_C void CCFModuleChannelHandler::ConstructL(CommsFW::RCFChannel::TMsgQueues &aRxQueues, CommsFW::RCFChannel::TMsgQueues &aTxQueues)
/** Second level constructor, must be called from derived implementations or the module 
will PANIC when attempting to use the channels.
@param aRxQueues Queues on which to listen for incoming messages.
@param aTxQueues Queues on which to send outgoing messages.
@publishedPartner
@released
*/
    {
	TInt ret;
	if((ret = iChannelPair.CreateRecv(aRxQueues)) != KErrNone ||
	   (ret = iChannelPair.CreateSend(aTxQueues)) != KErrNone)
		{
		User::Leave(ret);
		}
	CActiveScheduler::Add(this);
	SetActive();
	iChannelPair.NotifyDataAvailable(*this);
    }

EXPORT_C /*virtual*/ void CCFModuleChannelHandler::DoCancel()
/** Cancel function, will be called during destruction. Closes the channels contained.
@internalComponent
*/
    {
 	iChannelPair.CancelDataAvailable();
 	iChannelPair.CloseNonGracefully();
    }

EXPORT_C /*virtual*/ void CCFModuleChannelHandler::CFChannelError(TInt aError)
/** Called when errors are encountered on using the channels. Does nothing but
creating a log entry for [CommsFw,Warning]. Should be overridden if the module
wants to supports handling of errors.
@param aError The errorcode.
@publishedPartner
@released
*/
    {
	// Do nothing is default, maybe instead we should panic?
	(void) aError;
	__CFLOG_1(KLogCommsFw, KLogWarning, _L8("ERROR CCFModuleChannelHandler::CFChannelError(%d)!"), aError);
    }

EXPORT_C /*virtual*/ void CCFModuleChannelHandler::RunL()
/** Receive a message and register for listening for more data on the incoming channel.
wants to supports handling of errors.
@internalComponent
*/
    {
	if (iStatus != KErrNone)
		{
		__CFLOG_1(KLogCommsFw, KLogWarning, _L8("CCFModuleChannelHandler::RunL(): NotifyDataAvailable returned %d"), iStatus.Int());
		CFChannelError(iStatus.Int());
		return;
		}
	
	// No error. Take the message from the channel.
	TCFMessage msg;
	TInt err(iChannelPair.Receive(msg));
	if (KErrNone != err)
		{
		__CFLOG_1(KLogCommsFw, KLogWarning, _L8("CCFModuleChannelHandler::RunL(): Receive returned %d"), iStatus.Int());
		CFChannelError(err);
		return;
		}

	switch (msg.Code())
		{
		case TCFCommsMessage::ECodeDiscover:
		CFMessageDiscover(reinterpret_cast<const TCFDiscoverMsg&>(msg));
		break;
		
		case TCFCommsMessage::ECodeBind:
		CFMessageBind(reinterpret_cast<const TCFBindMsg&>(msg));
		break;
		
		case TCFCommsMessage::ECodeUnbind:
		CFMessageUnbind(reinterpret_cast<const TCFUnbindMsg&>(msg));
		break;
		
		case TCFCommsMessage::ECodeShutdown:
		CFMessageShutdown(reinterpret_cast<const TCFShutdownMsg&>(msg));
		break;

		case TCFCommsMessage::ECodeForward:
		CFMessageForward(reinterpret_cast<const TCFForwardMsg&>(msg));
		break;

		default:
		CFMessageUnknown(msg);
		break;
		}

	//Pend again
	SetActive();
	iChannelPair.NotifyDataAvailable(*this);
	}


EXPORT_C TBool CommsFW::GetVarFromIniData(const TDesC8& aIniData, const TDesC8& aSection, const TDesC8& aVarName, TPtrC8& aResult)
/** Find a variable's value given a section name and a var name.
@param aSection The section to search.
@param aVarName The name to find.
@param aResult The value.
@return Success or fail.
*/
	{
	__ASSERT_DEBUG(aSection.Length() <= KMaxIniDataSectionNameLength, User::Panic(KSpecAssert_ElemCommsFwcftl, 1));
	__ASSERT_DEBUG(aVarName.Length() <= KMaxIniDataSectionNameLength, User::Panic(KSpecAssert_ElemCommsFwcftl, 2));

	TBuf8<KMaxIniDataSectionNameLength + 2> sectionToken;			// 2 extra chars for [sectionName]
	TInt sectionStart = 0;
	TInt sectionLen = 0;	// reassure compiler
	TPtrC8 section;
	// Sections are optional
	if(aSection.Length() > 0)
		{
		_LIT8(KSectionTokenString,"[%S]");
		sectionToken.Format(KSectionTokenString,&aSection);
#ifdef SYMBIAN_NETWORKING_CFTRANSPORT
		sectionStart = aIniData.FindF(sectionToken);
#else
		sectionStart = aIniData.Find(sectionToken);
#endif
		if (sectionStart == KErrNotFound)
			return EFalse;
		section.Set(aIniData.Mid(sectionStart));
#ifdef SYMBIAN_NETWORKING_CFTRANSPORT
		sectionStart += section.FindF(TPtrC8(_S8("]")));
#else
		sectionStart += section.Find(TPtrC8(_S8("]")));
#endif
		if (sectionStart == KErrNotFound)
			return EFalse;
		sectionStart++;
		section.Set(aIniData.Mid(sectionStart));
		
#ifdef SYMBIAN_NETWORKING_CFTRANSPORT
		sectionLen = section.FindF(TPtrC8(_S8("[")));
#else
		sectionLen = section.Find(TPtrC8(_S8("[")));
#endif
		if (sectionLen == KErrNotFound)
			sectionLen = section.Length();
		}
	else
		{
		sectionLen = aIniData.Length();
		}
	TInt pos = 0;
	FOREVER
		{
		section.Set(aIniData.Mid(sectionStart, sectionLen));
#ifdef SYMBIAN_NETWORKING_CFTRANSPORT
		pos = section.FindF(aVarName);
#else
		pos = section.Find(aVarName);
#endif
		if (pos==KErrNotFound)
			return EFalse;
		// Check that the token is at the start of the line, to avoid finding
		// substrings or commented statements). If it isn't then continue
		// searching from that point
		TUint8 precedingChar;
		if (pos == 0 ||
			(precedingChar = section[pos - 1]) == '\n' ||
			precedingChar == '\r')
			{
			break;
			}
		sectionStart += (pos + 1);
		sectionLen -= (pos + 1);
		}

	section.Set(section.Mid(pos));
    // Found token - now find position of '='
	pos = section.Locate('=');
	if((KErrNotFound==pos) || ((pos+1)==section.Length()))
		{
		return EFalse;
		}

	// Shrink section to start after'='
	TLex8 lex(section.Mid(++pos));
	section.Set(section.Mid(pos));
//	sectionStart+=pos;

	// Set length of section to be to end of line or end of file
	TInt length=section.Locate('\r');
	if(length==KErrNotFound)
		{
		(void)lex.NextToken();
		length=lex.MarkedToken().Length();
		}
	aResult.Set(section.Ptr(), length);
	return(ETrue);
	}

//@TODO - why Configurator can't move to using same?
EXPORT_C TBool CommsFW::GetVarFromIniData(const TDesC8& aIniData, const TDesC8 &aSection, const TDesC8 &aVarName, TInt &aResult)
	{
	TPtrC8 ptr;
	if (CommsFW::GetVarFromIniData(aIniData, aSection, aVarName, ptr))
		{
		TUint32 result;
		if (CommsFW::ConvertVal(ptr, result)==KErrNone)
			{
			aResult = result;
			return(ETrue);
			}
		}
	return(EFalse);
	}

//
// COwnEntryList
//

EXPORT_C COwnEntryList::TOwnEntry::TOwnEntry(const TParse& aPath, const TEntry& aEntry)
	{
	TPtr fullName(iName.Des());
	fullName.Copy(aPath.DriveAndPath());
	fullName.Append(aEntry.iName);
	}

EXPORT_C COwnEntryList::COwnEntryList(TInt aGranularity)
: iArray(aGranularity)
	{
	}

EXPORT_C COwnEntryList::~COwnEntryList()
	{
	iArray.Close();
	}

/*
EXPORT_C COwnEntryList* COwnEntryList::NewL(TInt aGranularity)
	{
	COwnEntryList* self = new(ELeave) COwnEntryList(aGranularity);
	CleanupStack::PushL(self);
	if(aDir)
		{
		self->AddL(*aDir);
		}
	CleanupStack::Pop(self);
	return self;
	}
*/

EXPORT_C TInt COwnEntryList::Count() const
	{
	return iArray.Count();
	}

EXPORT_C const COwnEntryList::TOwnEntry& COwnEntryList::operator[](TInt anIndex) const
	{
	return iArray[anIndex];
	}

EXPORT_C void COwnEntryList::AddL(const TOwnEntry& aEntry)
	{
	User::LeaveIfError(iArray.Append(aEntry));
	}

EXPORT_C void COwnEntryList::AddL(const COwnEntryList& aDir)
	{
	const TInt cnt = aDir.Count();
	for(TInt i = 0; i < cnt; ++i)
		{
		AddL(aDir[i]);
		}
	}

EXPORT_C void COwnEntryList::WildScanAcrossDrivesL(const TDesC& aDir, const TDesC& aFileMask)
	{
	iArray.Reset();
	TAutoClose<RFs> fs;
	User::LeaveIfError(fs.iObj.Connect());
	fs.PushL();
	TFindFile ff(fs.iObj);
	TParse fullEntry;
	CDir* dir;
	if(ff.FindWildByDir(aFileMask, aDir, dir) == KErrNone)
		{
		do 
			{
			CleanupStack::PushL(dir);
			const TInt cnt = dir->Count();
			for(TInt i = 0; i < cnt; ++i)
				{
				const TEntry& entry = (*dir)[i];
				fullEntry.Set(entry.iName, &ff.File(), NULL);      
				AddL(TOwnEntry(fullEntry, entry));
				}
			CleanupStack::PopAndDestroy(dir);
			}
		while(ff.FindWild(dir) == KErrNone);
		}	
	fs.Pop();
	}

EXPORT_C void COwnEntryList::UniqueWildScanAcrossDrivesL(const TDesC& aDir, const TDesC& aFileMask)
/**
 * Function is used to scan unique files from drives starting from Y: to A: then last Z:. The scanning 
 * is done by module name.Files with unique name module name will be scanned.This function is used by Rootserver to 
 * scan cmi files unique by name.It can be used by other internal components for the similar use.
 * 
 * @internalComponent
 */
	{
	iArray.Reset();
	TAutoClose<RFs> fs;
	User::LeaveIfError(fs.iObj.Connect());
	fs.PushL();
	TFindFile ff(fs.iObj);
	TParse fullEntry;
	CDir* dir;
	RArray<TEntry> files;
	CleanupClosePushL(files);
	TBool found;
	if(ff.FindWildByDir(aFileMask, aDir, dir) == KErrNone)
		{
		do 
			{
			CleanupStack::PushL(dir);
			const TInt cnt = dir->Count();
			for(TInt i = 0; i < cnt; ++i)
				{
				const TEntry& entry = (*dir)[i];
				if(files.Count()!=0)
					{
					found = EFalse;
					for( TInt Index=0;Index<files.Count();Index++)
						{
						if(files[Index].iName.CompareF(entry.iName)==0)
							{
							found = ETrue;
							}
						}
					if(!found)
						{
						files.AppendL(entry);
						fullEntry.Set(entry.iName, &ff.File(), NULL);      
						AddL(TOwnEntry(fullEntry, entry));
						}
					}
				else
					{
					files.AppendL(entry);
				    fullEntry.Set(entry.iName, &ff.File(), NULL);      
				    AddL(TOwnEntry(fullEntry, entry));
				    }
				}
				CleanupStack::PopAndDestroy(dir);
			}
		while(ff.FindWild(dir) == KErrNone);
		}	
	CleanupStack::PopAndDestroy(&files);
	fs.Pop();
	}

//
// class RCFHeap
//

#if defined(__CFLOG_ACTIVE) || defined(SYMBIAN_TRACE_ENABLE)

void RCFSharedHeap::WalkFunc(TAny* aContext, TCellType aType, TAny* aBase, TInt /*aSize*/)
	{
	(void)aBase;
	if(aType == EGoodAllocatedCell)
		{
		TLogInfo* logInfo = reinterpret_cast<TLogInfo*>(aContext);
		__CFLOG_VAR((logInfo->iSubSystem, logInfo->iComponent, _L8("  0x%x"), (TUint) aBase + (TUint) RHeap::EAllocCellSize));
		
#ifdef SYMBIAN_TRACE_ENABLE
	TLogTextBuf buf;
	TBool loggingSuccess = EFalse;
	if(logInfo->iSubSystem.Length() < KMaxLogTextLength)
		{
		buf.Append(logInfo->iSubSystem);
		_LIT8(KSpace, " ");
		if(buf.Length() + KSpace().Length() < KMaxLogTextLength)
			{			
			buf.Append(KSpace);			
			if(buf.Length() + logInfo->iComponent.Length() < KMaxLogTextLength)
				{
				buf.Append(logInfo->iComponent);
				if(buf.Length() + KSpace().Length() < KMaxLogTextLength)
					{			
					buf.Append(KSpace);	
					_LIT8(KHexNumberFormat, " 0x%x");
					buf.AppendFormatIgnoreOverflow(KHexNumberFormat, (TUint) aBase + (TUint) RHeap::EAllocCellSize);
					loggingSuccess = UTracePfAny(KPrimaryFilter, KText, ETrue, EFalse, 0, buf.Ptr(), buf.Length());	
					}
				}	
			}	
		}
		if(loggingSuccess == EFalse) //May happen if buffer length exceeds KMaxLogTextLength due to longer names of logInfo->iSubSystem and/or logInfo->iComponent 
									 //Atleast the below log helps to identify that there is a leak
			{
			_LIT8(KCantLogNames, "??? ??? 0x%x"); //Atleast this should fit
			buf.Zero();
			buf.AppendFormatIgnoreOverflow(KCantLogNames, (TUint) aBase + (TUint) RHeap::EAllocCellSize);
			loggingSuccess = UTracePfAny(KPrimaryFilter, KText, ETrue, EFalse, 0, buf.Ptr(), buf.Length());		
			}
#endif //SYMBIAN_TRACE_ENABLE			
		}
	}

#endif //defined(__CFLOG_ACTIVE) || defined(SYMBIAN_TRACE_ENABLE)

EXPORT_C TInt RCFSharedHeap::AccessCount() const 
	{
	return iAccessCount;
	}

EXPORT_C void RCFSharedHeap::LogAllocatedCells(const TDesC8& aSubSystem, const TDesC8& aComponent)
	{
#if defined(__CFLOG_ACTIVE) || defined(SYMBIAN_TRACE_ENABLE)
	TLogInfo logInfo(aSubSystem, aComponent);
	DebugFunction(EWalk, (TAny*)WalkFunc, &logInfo);
#else
	(void) aSubSystem;
	(void) aComponent;
#endif
	}


//
// Other utils
//

namespace CommsFW
{

EXPORT_C TInt ConvertVal(const TDesC8& aVar, TUint32& aResult)
	{
	TPtrC8 ptr(aVar);
	TRadix radix;
	_LIT8(KHexPrefix, "0x");
	if(ptr.Left(2).CompareF(KHexPrefix) == 0)
		{
		ptr.Set(ptr.Mid(2));
		radix = EHex;
		}
	else
		{
		radix = EDecimal;
		}
	TLex8 lex(ptr);
	TUint32 result;
	TInt err = lex.Val(result, radix);
	if (err==KErrNone)
		{
		aResult = result;
		}
	return err;
	}

EXPORT_C TInt ConvertVal(const TDesC8& aVar, TUint16& aResult)
	{
	TUint32 result = 0xFFFFFFFF;
	TInt err = ConvertVal(aVar, result);
	if (err==KErrNone && result<=0xFFFF)
		{
		aResult = result;
		}
	return err;
	}

}


