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
//  Campfire node messages example.
//
//  This example demostrates the definition and usage of nodes and messages.
//
//  There are 6 people (nodes) sitting around  a campfire, 1 Scout Leader
//  and 5 Scouts. The scout leader whispers (a message) to the first scout.
//  Each scout then passes on the whisper to the person to their right until
//  it reachs back to the scout leader. At this point, the message should
//  have mutated in transit.
//
#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>
#include <e32def.h>

#include <elements/cftransport.h>
#include <elements/nm_common.h>
#include <elements/nm_node.h>
#include <elements/nm_log.h>
#include <elements/nm_address.h>
#include <elements/nm_signatures.h>
#include <elements/nm_messages_base.h>
#include <elements/nm_interfaces.h>

class TMyWorkerThreadRegister; 
LOCAL_D CConsoleBase* console; 

_LIT8(KLoggingTag, "NMExample1");

_LIT(KCampfirePanicCat, "Campfire");

const TInt KPanicUnknownMessage = 1;

/**
 * Definition of constants for whisper message.
 */
const TInt KWhisperMessageId = 0x1; // used to identify messages within a realm
const TInt KWhisperMessageRealm = 0x2002984C; // used to group messages, UID allocated from symbian signed

/**
 * Definition of Message which is passed from node to node. The message
 * contains a buffer of characters.
 *
 * Defining the message involves declaring the message signature type and
 * then creating a concrete message by combining this signature with a realm
 * id and a message id.
 *
 * A signature defines the type of information a message can carry. Multiple
 * messages can be defined using the same signature. 
*/
const TInt KMaxStringLen = 20; 
const TInt KStringSigId = 0x1; 
typedef TBuf<KMaxStringLen> TString;

/**
 * Declare the message signature for TSigString,
 * It will contain a TString which can be accessed as iString.
*/
DECLARE_MESSAGE_SIG_1( SigString, TString, String)

/**
 * Build attribute table for TSigString. The attribute table is associated with a
 * realm and a signature id. In this case the realm is the same as the realm the final
 * concrete message will be defined in.
 *
 * The attribute table is used to serialise and unserialise the data in the message.
 */
EXPORT_START_ATTRIBUTE_TABLE_AND_FN( TSigString, KWhisperMessageRealm, KStringSigId )
    REGISTER_ATTRIBUTE( TSigString, iString, TMeta<TString> )
END_ATTRIBUTE_TABLE_BASE( TSignatureBase, ESignatureBase )

/**
 * Define a constructor to for TSigString which creates a new TSigString when passed
 * in a descriptor containing a serialised instance of TSigString.
 */
DEFINE_MVIP_CTR(TSigString);

/**
 * Table of deserialising constructors. These are registered with the thread globals
 * so that meta objects can be constructed without explicitly needed to know the type.
 * For more details see "NetMeta_Design_Document.doc". This table is used in MainL.
 */
const TImplementationProxy SignatureImplementationTable[] =
	{
	MVIP_CTR_ENTRY(KStringSigId, TSigString),							
	};

/**
 * Defines the actual concrete message used in the example.
 */
typedef TMessageSigString<KWhisperMessageId, KWhisperMessageRealm> TWhisper;

/**
 * Base class for all people at the campfire. This class derives from
 * ASimpleNodeIdBase. Implementation of ReceivedL is left to CScout and
 * CScoutLeader, and these each treat the message differently.
 *
 * All instances of APerson are nodes.
 */
class APerson:  public Messages::ASimpleNodeIdBase
	{
public:
	APerson() : Messages::ASimpleNodeIdBase() {};
	
	const Messages::TNodeId& PersonToRight()
		{
		return iRight;
		}
	
	void SetPersonToRight(const Messages::TNodeId& aRight)
		{
		iRight = aRight;
		}
	
protected:
	Messages::TNodeId iRight;
	};

/**
 * A Scout. Instances of Scout are initialised with two letter, which
 * define how they will tranform the whisper, replacing the first letter
 * with the second letter
 */
class CScout : public CBase, public APerson
	{
public:
	CScout(TText aLetterFrom, TText aLetterTo);

	/**
	 * Implementaton of ASimpleNodeIdBase::ReceivedL.
	 */
	void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

private:
	TText iLetterFrom;
	TText iLetterTo;
	};

/**
 * The ScoutLeader. This class provides an interface to the user to interact
 * with the program.
 */
class CScoutLeader : public APerson, public CActive
	{
public:
	CScoutLeader();
	
	void Init();
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError); 

	/**
	 * Implementaton of ASimpleNodeIdBase::ReceivedL.
	 */
	void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
	};

CScout::CScout(TText aLetterFrom, TText aLetterTo) : APerson(), iLetterFrom(aLetterFrom), iLetterTo(aLetterTo)
	{
	NM_LOG_NODE_CREATE(KLoggingTag, CScout);
	}

/**
 * Handler for messages passed into the node.
 */
void CScout::ReceivedL(const Messages::TRuntimeCtxId& /*aSender*/, const Messages::TNodeId& /*aRecipient*/, Messages::TSignatureBase& aMessage)
	{
	/**
	   Messages are passed to ReceivedL in a generic form.
	   They must be cast to their proper type before being used.
	   In this case we know that the message will always be a TWhisper
	   so the cast is safe.
	*/
	if (!aMessage.IsMessage<TWhisper>())
		{
		User::Panic(KCampfirePanicCat, KPanicUnknownMessage);
		}

	/**
	   If aMessage isn't a TWhisper at this stage, (which is impossible since we have
	   just explicitly tested it) the cast would result in a panic.

	   There is a non panicking version, to which you pass a pointer to the message, and
	   returned a pointer to the casted to type.

	   TWhisper* w = Messages::message_cast<TWhisper>(&aMessage);
	   
	   In this case if aMessage isn't of type TWhisper, NULL will simply be returned.
	*/
	TWhisper& w = Messages::message_cast<TWhisper>(aMessage);

	// replace the letter
	for (TInt i = 0; i < w.iString.Length(); i++)
		{
		if (w.iString[i] == iLetterFrom)
			{
			w.iString[i] = iLetterTo;
			}
		}

	/**
	   Create a message containing a pointer to the buffer and post it
	   to the person on the right
	*/	   
	Messages::RNodeInterface::OpenPostMessageClose(Id(), PersonToRight(), w);
	}

CScoutLeader::CScoutLeader() : CActive(EPriorityStandard)
	{
	NM_LOG_NODE_CREATE(KLoggingTag, CScoutLeader);
	CActiveScheduler::Add(this);
	}

/**
 * Give the active object an initial kick to set it going.
*/
void CScoutLeader::Init()
	{
	iStatus = KRequestPending;
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

/**
 * The user interface of the program. Asks the user to input a whisper
 * and then starts passing it to the right.
 */
void CScoutLeader::RunL()
	{
	if (iStatus.Int() == KErrNone)
		{
		TString buf;
		
		console->Printf(_L("What to whisper?: "));
		TKeyCode c = console->Getch();
		TInt i = 0;
		while (c != '\n' && c != '\r' && i < buf.MaxLength())
			{
			console->Printf(_L("%c"), c);
			
			buf.Append(c);

			c = console->Getch();
			++i;
			}
		console->Printf(_L("\n"));

		if (buf.CompareC(_L("Bear!")) == 0)
			{
			CActiveScheduler::Stop();
			return;
			}

		/**
		   Contruct message and pass to the right
		*/
		TWhisper msg(buf);
		Messages::RNodeInterface::OpenPostMessageClose(Id(), PersonToRight(), msg);

		/**
		   Set the active object as active. The AO will
		   be completed when the node receives the message back.
		*/
		iStatus = KRequestPending;
		SetActive();
		}
	}

TInt CScoutLeader::RunError(TInt aError)
	{
	return aError; 
	}

void CScoutLeader::DoCancel()
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrCancel);
	}

/**
 * Message handler for scout leader
 */
void CScoutLeader::ReceivedL(const Messages::TRuntimeCtxId& /*aSender*/, const Messages::TNodeId& /*aRecipient*/, Messages::TSignatureBase& aMessage)
	{
	TWhisper& w = Messages::message_cast<TWhisper>(aMessage);
	TString& buf = w.iString;

	console->Printf(_L("Whisper came back as: %S\n"), &buf);
		
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}


/**
   This classes records information about a worker thread, which is useful
   when nodemessages is running across multiple threads. In this example,
   nodemessages is running in a single thread.

   Most users of node messages will never need to use this as they will be
   using node messages from within serverden which does all this setup for
   you.
 */
class TMyWorkerThreadRegister : public CommsFW::MWorkerThreadRegister
	{
public:
	CommsFW::TWorkerId SelfWorkerId() const
		{ return 1; }
	CommsFW::TWorkerId UpperBoundWorkerId() const
		{ return 1; }

	RAllocator& WorkerHeap(CommsFW::TWorkerId /*aWorkerId*/) const
		{
		return User::Allocator();
		}

	void PanicWorker(CommsFW::TWorkerId /*aWorkerId*/, const TDesC& aCategory, TInt aReason) const
		{
		User::Panic(aCategory, aReason);
		}
	};

LOCAL_C void MainL ()
	{
	/**
	   Setup the environment for passing messages.
	*/
	TMyWorkerThreadRegister workerregister;
	Messages::CGlobals* globals = &Messages::TlsGlobals::InstallGlobalsL();

	/*
	  Register the message signatures with the globals. This allows the transport to deserialised message from the message queue.
	*/	  
	globals->RegisterInterfaceL(TUid::Uid(KWhisperMessageRealm), sizeof(SignatureImplementationTable) / sizeof(SignatureImplementationTable[0]), SignatureImplementationTable);
	
	CommsFW::CCommsTransport* transport = CommsFW::CCommsTransport::NewL(workerregister, globals->VirtualCtor(), NULL);
	globals->SetTransportSender(&transport->GetSender());
	globals->SetTransportReceiver(&transport->GetReceiver());
	CleanupStack::PushL(transport);

	/**
	 * Create the scouts and lay them out.
	 *
	 * Layout:
     *  X = Scout Leader
	 *	1-5 = Scouts
	 *
	 * 	   	   X
	 *	   1   	   5
	 *
	 *	   2   	   4
     *     	   3
	 *
	 */
	CScoutLeader* l = new(ELeave) CScoutLeader();
	CleanupStack::PushL(l);
	CScout* s1 = new (ELeave) CScout('o', '0');
	CleanupStack::PushL(s1);
	CScout* s2 = new (ELeave) CScout('e', '3');
	CleanupStack::PushL(s2);
	CScout* s3 = new (ELeave) CScout('a', '4');
	CleanupStack::PushL(s3);
	CScout* s4 = new (ELeave) CScout('t', '7');
	CleanupStack::PushL(s4);
	CScout* s5 = new (ELeave) CScout('l', '1');
	CleanupStack::PushL(s5);

	l->SetPersonToRight(s1->Id());
	s1->SetPersonToRight(s2->Id());
	s2->SetPersonToRight(s3->Id());
	s3->SetPersonToRight(s4->Id());
	s4->SetPersonToRight(s5->Id());
	s5->SetPersonToRight(l->Id());

	/**
	   Print out ids of nodes. This can be used to see who is
	   sending messages to whom in a SVG diagram generated from
	   the node messages logs
	*/
	console->Printf(_L("ScoutLeader Id: 0x%x\n"), l->Id().Ptr());
	console->Printf(_L("Scout 1 Id: 0x%x\n"), s1->Id().Ptr());
	console->Printf(_L("Scout 2 Id: 0x%x\n"), s2->Id().Ptr());
	console->Printf(_L("Scout 3 Id: 0x%x\n"), s3->Id().Ptr());
	console->Printf(_L("Scout 4 Id: 0x%x\n"), s4->Id().Ptr());
	console->Printf(_L("Scout 5 Id: 0x%x\n"), s5->Id().Ptr());
	console->Printf(_L("Whisper 'Bear!' to quit.\n\n"));
	l->Init();

	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(7, transport);
	
	Messages::TlsGlobals::UnInstallGlobals();
	}

LOCAL_C void DoStartL ()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL (scheduler);
	CActiveScheduler::Install (scheduler);

	MainL ();

	CleanupStack::PopAndDestroy (scheduler);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();

	TRAPD(createError, console = Console::NewL(_L("Node Messages Campfire Example"), TSize(KConsFullScreen,KConsFullScreen)));
	if (createError)
		{
		return createError;
		}

	TRAPD(mainError, DoStartL());
	if (mainError)
		{
		console->Printf(_L(" failed, leave code = %d"), mainError);
		}
	console->Printf(_L(" [press any key]\n"));
	console->Getch();

	delete console;
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}

