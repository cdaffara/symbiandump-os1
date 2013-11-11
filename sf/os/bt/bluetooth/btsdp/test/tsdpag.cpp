// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// SDP Database library test code
// 
//

#include <e32test.h>
#include <f32file.h>
#include <btmanclient.h>
#include <btsdp.h>
#include <btextnotifiers.h>
#include "sdpkey.h"
#include "SDPDatabase.h"
#include "MAttributeVisitor.h"
#include "ServiceSearchVisitor.h"
#include "DataEncoder.h"
#include "ExtractorVisitor.h"
#include "EncoderVisitor.h"
#include "debug.h"
#ifdef __EPOC32__
#include <c32comm.h>
#endif


#include <es_sock.h>
#include <bt_sock.h>
#include <c32comm.h>
#include <btsdp.h>
#include "agtypes.h"
#include "agutil.h"
#include "ProtocolWrapper.h"

#include "TSDPSAS.h"



GLDEF_D RTest test(_L("SDP database DLL tests"));

void AgPanic(TSdpAgPanic aCode)
	{
	User::Panic(_L("SdpAgent"),aCode);
	}


#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else  // __GCC32__
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
// #define ETNA_PDD_NAME _L("EUART2") // for debugging over com2
#endif

CSdpDatabase *BuildDbL();
CSdpDatabase *BuildUpf4DbL();
// in Builddb.cpp

//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x0002, 0x5bff094c));	//Cambridge CASIRA
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x0002, 0x5bff0a4c));	//Cambridge CASIRA 2
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x0002, 0x5b013434));	//PANCAS 03
GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x0002, 0x5b0136bc));	//PANCAS 13
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x0002, 0x5b013610));	//PANCAS 16
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x0002, 0xee4d6133));	//NICK'S 7650
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x0060, 0x57411ee1));	//ANDY P'S 3650
//
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x0080, 0x370f2e22));
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x0050, 0xcd0002e0));

//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x0002, 0x5bffbbb9));	//Hasse's Casira
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x00e0, 0x0370b5d3));	//Helge Board 1
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x00e0, 0x0326d3b4));	//Helge Board 2
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x00d0, 0xb70329b8));	//EBMK sn:001 0028 0438
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x00d0, 0xb7032bbb));	//EBMK sn:001 0028 0353
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x00d0, 0xb7030ece));	//EBMK sn:001 0017 0344
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x00d0, 0xb7031750));	//EBMK sn:001 0017 0346
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0xaebd, 0x99000201)); //Ericcson 
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0xaec3, 0xdd562e32)); //RTX Telecom A/S
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0xae2d, 0xaa343536)); //Widcomm
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x0080, 0x3712205d)); //SSL
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x0080, 0x370F287D)); //SSL
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x0002, 0x5bff0609)); //John T's blue dongly
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x0080, 0x375A000E)); // EBDK 1 (0081 0097...)
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x0080, 0x375A000F)); // EBDK 3 (001 0008 0095)


//static const TUint16 KUnplugUUID = 0x1103;
//static const TUint16 KUnplugUUID = 3;
//static const TUint16 KUnplugAttribId = 4;

//static const TInt KSDPMaxRequestSize = 100;

//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x0001, 0xec0cdfda));	// EBDK 1
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x0050, 0xcd0002e1));	// PC Card
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x0050, 0xcd003038));	// RS232 card
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x00d0, 0x5a000064));	// Harold
//GLDEF_D TBTDevAddr devAddr(MAKE_TINT64(0x00d0, 0x5a000064));	// Harold




void LoadLDD_PDD()
	{
	TInt r;
#ifdef __EPOC32__
	r=StartC32();
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		test.Printf(_L("Failed %d!\n\r"),r);
		test(r==KErrNone);
		}
	else
		test.Printf(_L("Started C32\n"));
#endif
	test.Printf(_L("Loading PDD\n"));
	r=User::LoadPhysicalDevice(PDD_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		test.Printf(_L("Failed %d!\n\r"),r);
		test(r==KErrNone);
		}
	else 
		test.Printf(_L("Loaded PDD\n"));
	test.Printf(_L("Loading LDD\n"));
	r=User::LoadLogicalDevice(LDD_NAME);
	if (r!=KErrNone && r!=KErrAlreadyExists)
		{
		test.Printf(_L("Failed %d!\n\r"),r);
		test(r==KErrNone);
		}
	else
		test.Printf(_L("Loaded LDD\n"));
	}

// plagiarised from tsdp.cpp
// actually Mel can't cast from Shortest form (TPtrC8) to a TDesC8 without this call
void HexDes(const TDesC8& aDes)
	{
	for (TInt i = 0; i < aDes.Length(); ++i)
		test.Printf(_L("%02x"), aDes[i]);
	}

void HexDump(const TDesC8& aDes)
	{
	for (TInt i = 0; i < aDes.Length(); ++i)
		{
		if (i%8 == 0)
			test.Printf(_L("0x%04x:   "), i);
		test.Printf(_L("0x%02x "), aDes[i]);
		if (i%8 == 7 || i == aDes.Length() - 1)
			test.Printf(_L("\n"));
		}
	}

class CConsoleBuilder : public CBase, public MSdpElementBuilder
	{
public:
	CConsoleBuilder(CConsoleBase& aConsole) :iConsole(aConsole) {};
	MSdpElementBuilder* BuildUnknown(TUint8 aType, TUint8 aSizeDesc, const TDesC8& aData)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("Got Unknown: <type %d, sizedesc %d, length %d>\n"), aType, aSizeDesc, aData.Length());
		HexDump(aData);
		return this;
		};
	MSdpElementBuilder* BuildNilL()
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("Got Nil: <>\n"));
		return this;
		};
	MSdpElementBuilder* BuildUintL(TUint aUint)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("Got Uint: <%d>\n"), aUint);
		return this;
		};
	MSdpElementBuilder* BuildUintL(const TDesC8& aLongUint)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("Got Long Uint: <Length %d>\n"), aLongUint.Length());
		HexDump(aLongUint);
		return this;
		};
	MSdpElementBuilder* BuildIntL(TUint aInt)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("Got Int: <%d>\n"), aInt);
		return this;
		};
	MSdpElementBuilder* BuildIntL(const TDesC8& aLongInt)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("Got Long Int: <Length %d>\n"), aLongInt.Length());
		HexDump(aLongInt);
		return this;
		};
	MSdpElementBuilder* BuildUUIDL(const TUUID& aUUID)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("Got UUID: <length %d>\n"), aUUID.ShortestForm().Length());
		HexDump(aUUID.ShortestForm());
		return this;
		};
	MSdpElementBuilder* BuildBooleanL(TBool aBool)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("Got Boolean: <%d>\n"), aBool);
		return this;
		};
	MSdpElementBuilder* BuildStringL(const TDesC8& aString)
		{
		TBuf<512> ustr;
		ustr.Copy(aString.Left(Min(aString.Length(), ustr.MaxLength())));
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("Got String: <length %d, Contents %S>\n"), ustr.Length(), &ustr);
		return this;
		};
	MSdpElementBuilder* BuildDESL()
		{
		return StartListL();
		};

	MSdpElementBuilder* BuildDEAL()
		{
		return StartListL();
		};

	MSdpElementBuilder* StartListL(/*prob want list type here*/)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("<START LIST>\n"));
		iIndent += 2;
		return this;
		};
	MSdpElementBuilder* EndListL()
		{
		if (iIndent > 0)
			{
			iIndent-=2;
			iConsole.SetPos(iIndent);
			iConsole.Printf(_L("<END LIST>\n"));
			}
		else
			{
			iConsole.Printf(_L("WARNING: UNMATCHED END LIST!"));
			}
		return this;
		};
	MSdpElementBuilder* BuildURLL(const TDesC8& aString)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("Got URL: <%S>\n"), &aString);
		return this;
		};

private:
	CConsoleBase&	iConsole;
	TInt			iIndent;
	};



TInt ParseL(TDesC8& aDes)
{
	// Parse attribute list
	CConsoleBuilder* builder = new(ELeave) CConsoleBuilder(*test.Console());
	CElementParser* parser = CElementParser::NewL(builder);

	test.Next(_L("Parsing simple attribute list"));
	TUint rem=0;
	TRAPD(err,rem=parser->ParseElementsL(aDes.Mid(2)));
	if (err)
		test.Printf(_L("Parser left with error %d\n"), err);
	else
		test.Printf(_L("Parser returned %d\n"), rem);
	test.Getch();
	return rem;
}



class CAttrPrintVisitor : public CBase, public MAttributeVisitor
	{
public:
	CAttrPrintVisitor(CConsoleBase& aConsole) : iConsole(aConsole) {iIndent=0;}
	~CAttrPrintVisitor() {/*iConsole.Getch();*/}
    void VisitAttributeL(CSdpAttr &aAttribute)
		{
		Indent();
		test.Printf(_L("\nID:0x%x:"), aAttribute.AttributeID());
		}
	void VisitAttributeValueL(CSdpAttrValue & aValue, TSdpElementType aType)
		{
// FIXME need a new object like the match list that will just store the object in
// FIXME an array, or perhaps just print it ? Is it a visitor ?
//		CSdpSearchPattern* encAttrList = new (ELeave) CSdpSearchPattern;
//		CleanupStack::PushL(encAttrList);

//		CElementParser* parser = CElementParser::NewL(encAttrList);
//		CleanupStack::PushL(parser);

//		TInt rem;

		TBuf16<0x64> iString;
//		TInt iFound;
		switch (aType)
			{
			case ETypeString:
				{
				TInt len = aValue.Des().Length();
				if(len<0x60)
					//cannot cope with strings longer than 100 bytes!
					{
					iString.Copy(aValue.Des());
					test.Printf(_L("\"%S\""),&iString);
					}
				else
					{
					test.Printf(_L("String attribute too long for CONSOLE: Length %d"),len);
					}
				break;
				}
			case ETypeDES:
				test.Printf(_L(" DES"));
				break;
			case ETypeUint:
				test.Printf(_L(" UInt:0x%x"), aValue.Uint());
				break;
			case ETypeUUID:
				test.Printf(_L(" UUID:0x"));
				HexDes(aValue.UUID().ShortestForm());
				break;
			case ETypeEncoded:
				HexDes(aValue.Des());  // simplest
//parse out the elements in this encoded attribute
//				rem = parser->ParseElementsL(aValue.Des());
//				CleanupStack::PopAndDestroy(/*parser*/);
//				for (iFound=0 ; iFound++ ; (iFound < (encAttrList->Length())))
//				{
//					VisitAttributeValueL(encAttrList->At(iFound).Value, encAttrList->At(iFound).Type);
//				}
//				CleanupStack::PopAndDestroy(/*encAttrList*/);
				break;
			default:
				test.Printf(_L("type %d\n"), aType);
			}
		}
	void StartListL(CSdpAttrValueList &/*aList*/)
		{
		++iIndent;
		test.Printf(_L("{"));
		}
    void EndListL()
		{
		if(iIndent<=0)
			{
			test.Printf(_L("\nERROR! Unmatched EndList!\n"));
			__DEBUGGER();
			}
		test.Printf(_L("}"));
		--iIndent;
		}
private:
	void Indent() {/*test.SetPos(iIndent)*/;}
	CConsoleBase &iConsole;
	TInt iIndent;
	};

void PrintDb(CSdpDatabase& aDb, CConsoleBase& aConsole)
	{
	aConsole.Printf(_L("Printing Database...\n"));

	for(TServRecordIter recIter(aDb.RecordIter()); recIter; recIter++)
		{// Iterate thru records in Db
		aConsole.Printf(_L("\n...Printing Record 0x%x\n"), (*recIter).Handle());
		for(TServAttrIter attrIter((*recIter).AttributeIter()); attrIter; attrIter++)
			{// Iterate thru attributes in record
			CAttrPrintVisitor* theVisitor = new CAttrPrintVisitor(aConsole);
			(*attrIter).AcceptVisitorL(*theVisitor);
			delete theVisitor;
			}
		}
	}

class CAttrFlogVisitor : public CBase, public MAttributeVisitor
	{
public:
	CAttrFlogVisitor(){}
#ifdef _DEBUG
    void VisitAttributeL(CSdpAttr& aAttribute)
		{
		Indent();
		FTRACE(FPrint(_L("Attribute ID: 0x%x\n"), aAttribute.AttributeID()));
		}
#else
    void VisitAttributeL(CSdpAttr&)
		{
		Indent();
		}
#endif // _DEBUG
	void VisitAttributeValueL(CSdpAttrValue & aValue, TSdpElementType aType)
		{
		TBuf16<64> iString;
		switch (aType)
			{
			case ETypeString:
				iString.Copy(aValue.Des());
				FTRACE(FPrint(_L("\"%S\""),&iString));
				break;
			case ETypeDES:
				FTRACE(FPrint(_L(" DES")));
				break;
			case ETypeUint:
				FTRACE(FPrint(_L(" UInt:0x%x"), aValue.Uint()));
				break;
			case ETypeUUID:
				FTRACE(FPrint(_L(" UUID:0x")));
				HexDes(aValue.UUID().ShortestForm());
				break;
			case ETypeEncoded:
				HexDes(aValue.Des());  // simplest
				break;
			default:
				FTRACE(FPrint(_L("type %d\n"), aType));
			}
		}
    void StartListL(CSdpAttrValueList &/*aList*/)
		{
		++iIndent;
		Indent();
		FTRACE(FPrint(_L("{\n")));
		}
    void EndListL()
		{
		if(iIndent<=0)
			{
			FTRACE(FPrint(_L("\nERROR! Unmatched EndList!\n")));
			__DEBUGGER();
			}
		Indent();
		FTRACE(FPrint(_L("}\n")));
		--iIndent;
		}
private:
	void HexDes(const TDesC8& aDes)
		{
		for (TInt i = 0; i < aDes.Length(); ++i) 
			{
			FTRACE(FPrint(_L("%02x"), aDes[i]));
			}
		}

	void Indent() 
		{
		for(TInt i=0; i<iIndent;++i)
			{
			FTRACE(FPrint(_L("  ")));
			}
		}
	TInt iIndent;
	};


void FlogDb(CSdpDatabase& aDb)
	{
	FTRACE(FPrint(_L("\n\nPrinting Database...\n\n")));

	for(TServRecordIter recIter(aDb.RecordIter()); recIter; recIter++)
		{// Iterate thru records in Db
		FTRACE(FPrint(_L("\n...Printing Record 0x%x\n"), (*recIter).Handle()));
		for(TServAttrIter attrIter((*recIter).AttributeIter()); attrIter; attrIter++)
			{// Iterate thru attributes in record
			CAttrFlogVisitor* theVisitor = new CAttrFlogVisitor();
			(*attrIter).AcceptVisitorL(*theVisitor);
			delete theVisitor;
			}
		}
	FTRACE(FPrint(_L("\n\nEnd Printing Database...\n\n\n\n")));
	}



void Test1()
/**
	RSdpAgent Test.
**/
	{
	test.Start(_L("Testing RSdpAgent used with BT NetDb connection"));	// SOCKSERV CONNECTION
    RSocketServ ss;
	TInt ret=ss.Connect();
    test(ret==KErrNone || ret==KErrAlreadyExists);
	 

	TUint32 uuid = 0x1100;

	do
		{
		RSdpAgent sa;
		TInt bufSize;
		TUint16 maxAttrByteCount = 0x200;
		TRequestStatus stat;
		TSDPConnectQuery connQ;
		TBuf8<512> result;
		TBuf8<255> request;
		TInt len;
		TBuf8<2> contState(0);
		//BigEndian::Put16(&contState[0], 0);

	    test.Next(_L("Opening SDP Net Database"));
			
		ret=sa.Open(ss);
		if (ret==KErrNone)
			{
			test.Printf(_L("Created netdb OK\n"));
			}
		else// if(ret<KErrNone)
			{
			test.Printf(_L("Failed to open netdb, return = %d\n"),ret); 
			goto end;
			}

		// Do a version inquiry...


		FOREVER
			{
			test.Printf(_L("\n==================\nPress x to exit\ns to do a service search, a to get server version list\nS to search for a used service record, A to do search and attribs\nN for crazy newing tests\n==================\n"));
			TChar c = test.Getch(); 
			switch (c)
				{
			case 'x':
				goto end;
			case 'S':
			case 'A':
				{
				sa.Connect(devAddr, stat);
				User::WaitForRequest(stat);
				if (stat!=KErrNone)
					{
					test.Printf(_L("Connect query failed with error: %d\r\n"), stat.Int());
					goto end;
					}
				TSDPServiceSearchKey key;
				for (uuid = 0; uuid < 0x300; ++uuid)
					{
					if (uuid%8 == 0)
						test.Printf(_L("Using UUID 0x%04x\r"),uuid);
					CSdpAttrValueDES* list = CSdpAttrValueDES::NewDESL(NULL);
					MSdpElementBuilder* bldr = list;
					CleanupStack::PushL(list);
					bldr
						->StartListL()
							->BuildUUIDL(uuid)
						->EndListL();

					sa.SdpSearchRequestL(bufSize, *list, 0x20, contState, stat);
					User::WaitForRequest(stat);
					CleanupStack::PopAndDestroy(); //list
					if (stat!=KErrNone)
						{
						test.Printf(_L("Failed with %d on search query\r\n"), stat.Int());
						goto end;
						}
					HBufC8* bufS = HBufC8::NewMaxL(bufSize);
					CleanupStack::PushL(bufS);
					TPtr8 ptrS = bufS->Des();

					sa.RetrieveResult(ptrS);
					if (ptrS.Length() < 5)
						{
						CleanupStack::PopAndDestroy(); //bufS
						continue;
						}
					len=BigEndian::Get16(&ptrS[2]);
					if (len != 0)
						{
						if (len*4 > ptrS.Length() - 5)
							{
							test.Printf(_L("Bad length field %d on records length %d\r\n"),
								len*4, ptrS.Length());
							CleanupStack::PopAndDestroy(); //bufS
							goto end;
							}
						test.Printf(_L("\nFound %d Service records for UUID 0x%04x\n"), len, uuid);
						HexDump(ptrS.Mid(4, len*4));
						if (c == 'A')
							{
							for (TInt rec=0;rec<len;++rec)
								{
								test.Printf(_L("Getting all Attributes for record %d..."), rec);
								CSdpAttrIdMatchList* matchList = CSdpAttrIdMatchList::NewL();
								CleanupStack::PushL(matchList);
								matchList->AddL(TAttrRange(0x0000, 0xFFFF));
								sa.SdpAttributeRequestL(bufSize, 
														BigEndian::Get32(&ptrS[4+(rec*4)]),
														maxAttrByteCount,
														*matchList, 
														contState, 
														stat);
								User::WaitForRequest(stat);
								CleanupStack::PopAndDestroy(); //matchList
								if (stat!=KErrNone)
									{
									test.Printf(_L("Failed with %d on attribute query\r\n"), stat.Int());
									CleanupStack::PopAndDestroy(); //bufS
									goto end;
									}
								HBufC8* bufA = HBufC8::NewMaxL(bufSize);
								CleanupStack::PushL(bufA);
								TPtr8 ptrA = bufA->Des();

								sa.RetrieveResult(ptrA);
								test.Printf(_L("... got attributes for record %d, Here's the result....\n"), rec);
								// Parse aatribute list
								CConsoleBuilder* builder = new(ELeave) CConsoleBuilder(*test.Console());
								CleanupStack::PushL(builder);
								CElementParser* parser = CElementParser::NewL(builder);
								CleanupStack::PushL(parser);

								TUint rem=0;
								TRAPD(err,rem=parser->ParseElementsL(ptrA.Mid(2)));
								if (err)
									test.Printf(_L("Parser left with error %d - press any key\n"), err);
								else
									test.Printf(_L("Parser returned %d - press any key\n"), rem);
								test.Getch();
								CleanupStack::PopAndDestroy(3); //parser, builder, bufA
								}
							}
						}
					CleanupStack::PopAndDestroy(); //bufS
					}
				}
				continue;
			case 'N':
				{
				test.Next(_L("Crazy Newing test. Warning! Will OOM eventually..."));
				for (TInt i = 0; i < 256; ++i)
					{
					test.Printf(_L("Firing off parallel query %d\r"),i);
					RNetDatabase* n = new RNetDatabase;
					while((ret=n->Open(ss,KBTAddrFamily,KSDP)) == KErrServerBusy)
						{
						test.Printf(_L("\nProblem getting in contact with esock. Will try again\n"));
						User::After(2);
						};
					test(ret == KErrNone);
					TRequestStatus *r = new TRequestStatus;
					connQ.iQueryType = KSDPConnectQuery;
					connQ.iAddr = devAddr;
					result.SetMax();
					while((ret=n->Query(TSDPConnectBuf(connQ), result)) == KErrServerBusy)
						{
						test.Printf(_L("\nProblem getting in contact with esock. Will try again\n"));
						User::After(2);
						};
					test(ret == KErrNone);
					TSDPAttributeKey key;
					key.iQueryType = KSDPAttributeQuery;
					key.iServiceRecordHandle = 0;
					key.iMaxLength = 100;
					key.iRange = ETrue;
					key.iAttribute = 0x0000FFFF;
					key.iStateLength = 0;
					result.SetMax();
					n->Query(TSDPAttributeKeyBuf(key), result, *r);
					continue;
					}
				}
			case 's':
				{
				test.Next(_L("Doing a service Search Query..."));
				test.Printf(_L("Using UUID %04x\n"),uuid);
				sa.Connect(devAddr, stat);
				User::WaitForRequest(stat);
				if (stat!=KErrNone)
					{
					test.Printf(_L("Connect query failed with error: %d\r\n"), stat.Int());
					goto end;
					}

				
				CSdpAttrValueDES* list = CSdpAttrValueDES::NewDESL(NULL);
				MSdpElementBuilder* bldr= list;
				CleanupStack::PushL(list);

				bldr
					->StartListL()
						->BuildUUIDL(uuid++)
						//->BuildUUIDL(TUUID(TUint32(0x11112222), TUint32(0x33334444), 
						//					  TUint32(0x55556666), TUint32(0x77778888)))
						//->BuildUUIDL(TUUID(TUint32(0x40000)))
					->EndListL();
				if(uuid>=0x1105||uuid<0x1100)
					uuid = 0x1100;
				sa.SdpSearchRequestL(bufSize, *list, 20, contState, stat);
				User::WaitForRequest(stat);
				if (stat!=KErrNone)
					{
					test.Printf(_L("Failed with %d on search query\r\n"), stat.Int());
					CleanupStack::PopAndDestroy(); //list
					goto end;
					}
				HBufC8* buf = HBufC8::NewMaxLC(bufSize);
				TPtr8 ptr = buf->Des();
				ptr.SetMax();
				sa.RetrieveResult(ptr);
				test.Printf(_L("Successful query!!\r\nHere's the result....\n"));
				HexDump(ptr);
				CleanupStack::PopAndDestroy(2); //list, buf
				}
				continue;
			case 'a':
				{
				test.Next(_L("Doing a service Attribute Query..."));
				sa.Connect(devAddr, stat);
				User::WaitForRequest(stat);
				if (stat!=KErrNone)
					{
					test.Printf(_L("Connect query failed with error: %d\r\n"), stat.Int());
					goto end;
					}
				CSdpAttrIdMatchList* matchList = CSdpAttrIdMatchList::NewL();
				CleanupStack::PushL(matchList);
				matchList->AddL(TAttrRange(0x0000, 0xFFFF));
				sa.SdpAttributeRequestL(bufSize, 
										0,
										maxAttrByteCount,
										*matchList, 
										contState, 
										stat);
				User::WaitForRequest(stat);
				if (stat!=KErrNone)
					{
					test.Printf(_L("Failed with %d on attribute query\r\n"), stat.Int());
					CleanupStack::PopAndDestroy(); //matchList
					goto end;
					}
				HBufC8* buf = HBufC8::NewMaxLC(bufSize);
				TPtr8 ptr = buf->Des();
				ptr.SetMax();
				sa.RetrieveResult(ptr);
				test.Printf(_L("Successful query!!\r\nHere's the result....\n"));
				HexDump(ptr);
				CleanupStack::PopAndDestroy(2); //matchList, buf
				}
				continue;
			default:
				continue;
				}
			}

end:
		test.Next(_L("\r\nClosing netdb... "));			// CLOSING NETDB
		test.Getch();
		sa.Close();
		test.Printf(_L("\r\nPress c to reconnect, or x to quit"));
		}
	while(test.Getch() == 'c');
	ss.Close();									// CLOSING SOCKSERV 
	test.End();			
	}


class CSdpResultReceiver : public CBase, public MSdpAgentNotifier
	{
public:
	virtual ~CSdpResultReceiver();
	void SetAgent(CSdpAgent* aAgent)
		{
		iAgent = aAgent;
		};
	virtual void HandleTimeout(){};
	virtual void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount) = 0;
	virtual void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttr) = 0;
	virtual void AttributeRequestComplete(TSdpServRecordHandle, TInt aError) = 0;
protected:
	void ConstructL();
	CSdpResultReceiver(CConsoleBase& aConsole) 
	:iConsole(aConsole), iBuilder(0), iAgent(0)
		{
		iCounter = 0;
		};
protected:
	TInt				iCounter;
	CConsoleBase&		iConsole;
	TInt				iIndent;
	CConsoleBuilder*	iBuilder;
	CSdpAgent*			iAgent;
	};

CSdpResultReceiver::~CSdpResultReceiver()
	{
	delete iBuilder;
	}

void CSdpResultReceiver::ConstructL()
	{
	iBuilder = new(ELeave)CConsoleBuilder(iConsole);
	}

/**
	Allow a timeout for SDP agent tests
**/
class CTestTimeout : public CTimer
	{
public:
	static CTestTimeout* NewLC( CSdpResultReceiver& aParent);
	static CTestTimeout* NewL( CSdpResultReceiver& aParent);
	~CTestTimeout();
	void IssueRequest(TInt aConnectionTime);
	void RunL();
	void DoCancel();
private:
	void ConstructL( );
	CTestTimeout( CSdpResultReceiver& aParent);
private:
	CSdpResultReceiver& iParent;
	
	};

CTestTimeout* CTestTimeout::NewL( CSdpResultReceiver& aParent)
	{
	CTestTimeout* self = CTestTimeout::NewLC(aParent);
	CleanupStack::Pop();
	return self;
	}

CTestTimeout* CTestTimeout::NewLC( CSdpResultReceiver& aParent)
	{
	CTestTimeout* self = new(ELeave) CTestTimeout( aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestTimeout::CTestTimeout( CSdpResultReceiver& aParent )
	: CTimer(EPriorityStandard),
	iParent(aParent)
	{
	}

CTestTimeout::~CTestTimeout()
	{
	Cancel();
	}

void CTestTimeout::IssueRequest(TInt aConnectionTime)
	{
	After(aConnectionTime*1000000);
	}


void CTestTimeout::RunL()
	{
	iParent.HandleTimeout();
	}

void CTestTimeout::DoCancel()
	{
	CTimer::DoCancel();
	}

void CTestTimeout::ConstructL( )
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}




class CSdpResultReceiverTest2 : public CSdpResultReceiver
	{
public:
	static CSdpResultReceiverTest2* NewL(CConsoleBase& aConsole);
	virtual void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount)
		{
		iCounter++;
		if(!iAgent || iCounter > aTotalRecordsCount || aError == KErrEof)
			{
			iCounter = 0;
			CActiveScheduler::Stop();
			return;
			}
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nNextRecordRequestComplete\n"));
		iConsole.Printf(_L("Got Error: <%d>\n"), aError);
		iConsole.Printf(_L("Got Total Records Count: <%d>\n"), aTotalRecordsCount);
		iConsole.Printf(_L("Got Handle: <0x%x>\n"), aHandle);
		if(aError)
			{
			iAgent->NextRecordRequestL();
			}
		else
			{
			CSdpAttrIdMatchList* matchList = CSdpAttrIdMatchList::NewL();
			CleanupStack::PushL(matchList);
			matchList->AddL(TAttrRange(0x0000, 0xFFFF)); //getemall
			iAgent->AttributeRequestL(aHandle, *matchList);
			CleanupStack::PopAndDestroy(); //matchList - AttributeRequestL makes a copy
			}
		iConsole.Printf(_L("PRESS ANY KEY\n"));
		test.Getch();
		};

	virtual void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nAttributeRequestResult\n"));
		iConsole.Printf(_L("Got Handle: <0x%x>\n"), aHandle);
		iConsole.Printf(_L("Got Attribute: ID is: <0x%04x>\n"), aAttrID);
		delete aAttrValue; //ownership is passed here
		iConsole.Printf(_L("PRESS ANY KEY\n"));
		test.Getch();
		if(!iAgent)
			{
			CActiveScheduler::Stop();
			}
		else
			{
			}
		};

	virtual void AttributeRequestComplete(TSdpServRecordHandle, TInt aError)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nAttributeRequestComplete\n"));
		iConsole.Printf(_L("Got Error: <%d>\n"), aError);
		iConsole.Printf(_L("PRESS ANY KEY\n"));
		test.Getch();
		if(!iAgent||aError)
			{
			CActiveScheduler::Stop();
			return;
			}
		iAgent->NextRecordRequestL();
		};
private:
	CSdpResultReceiverTest2(CConsoleBase& aConsole) :CSdpResultReceiver(aConsole){};
	};

CSdpResultReceiverTest2* CSdpResultReceiverTest2::NewL(CConsoleBase& aConsole)
	{
	CSdpResultReceiverTest2* self = new(ELeave) CSdpResultReceiverTest2(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}



void Test2()
/**
	Test2()
**/
	{
	test.Start(_L("Test 2 -- Published Agent API"));
	CSdpResultReceiverTest2* rcvr = CSdpResultReceiverTest2::NewL(*test.Console());
	CleanupStack::PushL(rcvr);
	CSdpAgent* agent = CSdpAgent::NewLC(*rcvr, devAddr);
	rcvr->SetAgent(agent);
	CSdpSearchPattern* list = CSdpSearchPattern::NewL();
	CleanupStack::PushL(list);
	list->AddL(0x1102);
	list->AddL(0x0100);
	agent->SetRecordFilterL(*list);
	agent->NextRecordRequestL();
	CActiveScheduler::Start();
	test.Next(_L("\n\n\nEnd of test 2\n\n\n"));
	CleanupStack::PopAndDestroy(3); //list, agent, rcvr

	test.End();
	}

class CSdpResultReceiverTest3 : public CSdpResultReceiver
	{
public:
	static CSdpResultReceiverTest3* NewL(CConsoleBase& aConsole, CSdpSearchPattern& aList);

	virtual void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount)
		{
		iCounter++;
		if(!iAgent||iCounter>aTotalRecordsCount||aError)
			{
			iCounter = 0;
			CActiveScheduler::Stop();
			return;
			}
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nNextRecordRequestComplete\n"));
		iConsole.Printf(_L("Got Error: <%d>\n"), aError);
		iConsole.Printf(_L("Got Total Records Count: <%d>\n"), aTotalRecordsCount);
		iConsole.Printf(_L("Got Handle: <0x%x>\n"), aHandle);
		iConsole.Printf(_L("PRESS ANY KEY (***ESC*** TO STOP)\n"));
		TInt c = test.Getch();
		if(c==27)
			{
			iAgent->Cancel();
			CActiveScheduler::Stop();
			return;
			}
		else
			{
			CSdpAttrIdMatchList* matchList = CSdpAttrIdMatchList::NewL();
			CleanupStack::PushL(matchList);
			matchList->AddL(TAttrRange(0x0000, 0xFFFF)); //getemall
			iAgent->AttributeRequestL(aHandle, *matchList);
			CleanupStack::PopAndDestroy(); //matchList - AttributeRequestL makes a copy
			}

		};

	virtual void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nAttributeRequestResult\n"));
		iConsole.Printf(_L("Got Handle: <0x%x>\n"), aHandle);
		iConsole.Printf(_L("Got Attribute: ID is: <0x%04x>\n"), aAttrID);
		CAttrPrintVisitor* theVisitor = new CAttrPrintVisitor(iConsole);
		//theVisitor->VisitAttributeValueL(*aAttrValue, aAttrValue->Type());
		aAttrValue->AcceptVisitorL(*theVisitor);
		delete theVisitor;
		iConsole.Printf(_L("\nPRESS ANY KEY\n"));
		test.Getch();
		delete aAttrValue; //ownership is passed here
		//if(iCounter>1)
		//	User::Leave(KErrCompletion);
		if(!iAgent)
			{
			CActiveScheduler::Stop();
			return;
			}
		else
			{
			}
		};

	virtual void AttributeRequestComplete(TSdpServRecordHandle /*aHandle*/, TInt aError)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nAttributeRequestComplete\n"));
		iConsole.Printf(_L("Got Error: <%d>\n"), aError);
		iConsole.Printf(_L("PRESS ANY KEY (***ESC*** TO STOP)\n"));
		TInt c = test.Getch();
		if(!iAgent||aError)
			{
			CActiveScheduler::Stop();
			return;
			}
		if(c==27)
			{
			iAgent->Cancel();
			CActiveScheduler::Stop();
			return;
			}
		else
			{
			iAgent->NextRecordRequestL();
			}
		};
	~CSdpResultReceiverTest3()
		{
		};

private:
	CSdpResultReceiverTest3(CConsoleBase& aConsole, CSdpSearchPattern& aList) 
		:CSdpResultReceiver(aConsole), iList(aList)
		{
		};
	CSdpSearchPattern& iList;
	};

CSdpResultReceiverTest3* CSdpResultReceiverTest3::NewL(CConsoleBase& aConsole, CSdpSearchPattern& aList)
	{
	CSdpResultReceiverTest3* self = new(ELeave) CSdpResultReceiverTest3(aConsole, aList);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}




void Test3()
/**
	Test3()
**/
	{
	test.Start(_L("Test 3 -- Published Agent API"));
	CSdpSearchPattern* list = CSdpSearchPattern::NewL();
	CleanupStack::PushL(list);
	list->AddL(0x0100);
	CSdpResultReceiverTest3* rcvr = CSdpResultReceiverTest3::NewL(*test.Console(), *list);
	CleanupStack::PushL(rcvr);
	CSdpAgent* agent = CSdpAgent::NewLC(*rcvr, devAddr);
	rcvr->SetAgent(agent);
	agent->SetRecordFilterL(*list);
	agent->NextRecordRequestL();
	CActiveScheduler::Start();
	test.Next(_L("\n\n\nEnd of test 3\n\n\n"));
	CleanupStack::PopAndDestroy(3); //list, agent, rcvr
	test.End();
	}

class CSdpResultReceiverTest4 : public CSdpResultReceiver
	{
public:
	static CSdpResultReceiverTest4* NewL(CConsoleBase& aConsole);
	virtual void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount)
		{
		iCounter++;
		if(!iAgent||iCounter>aTotalRecordsCount||aError)
			{
			iCounter = 0;
			CActiveScheduler::Stop();
			return;
			}
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nNextRecordRequestComplete\n"));
		iConsole.Printf(_L("Got Error: <%d>\n"), aError);
		iConsole.Printf(_L("Got Total Records Count: <%d>\n"), aTotalRecordsCount);
		iConsole.Printf(_L("Got Handle: <0x%x>\n"), aHandle);
		iConsole.Printf(_L("PRESS ANY KEY\n"));
		test.Getch();
		iAgent->NextRecordRequestL();
		};

	virtual void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nAttributeRequestResult\n"));
		iConsole.Printf(_L("Got Handle: <0x%x>\n"), aHandle);
		iConsole.Printf(_L("Got Attribute: ID is: <0x%04x>\n"), aAttrID);
		delete aAttrValue; //ownership is passed here
		iConsole.Printf(_L("PRESS ANY KEY\n"));
		test.Getch();
		if(!iAgent)
			{
			CActiveScheduler::Stop();
			}
		else
			{
			}

		};

	virtual void AttributeRequestComplete(TSdpServRecordHandle, TInt aError)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nAttributeRequestComplete\n"));
		iConsole.Printf(_L("Got Error: <%d>\n"), aError);
		iConsole.Printf(_L("PRESS ANY KEY\n"));
		test.Getch();
		if(!iAgent||aError)
			{
			CActiveScheduler::Stop();
			return;
			}
		iAgent->NextRecordRequestL();
		};
private:
	CSdpResultReceiverTest4(CConsoleBase& aConsole) :CSdpResultReceiver(aConsole){};
	};

CSdpResultReceiverTest4* CSdpResultReceiverTest4::NewL(CConsoleBase& aConsole)
	{
	CSdpResultReceiverTest4* self = new(ELeave) CSdpResultReceiverTest4(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}



void Test4()
/**
	Test4()
**/
	{
	test.Start(_L("Test 4 -- Published Agent API"));
	CSdpResultReceiverTest4* rcvr = CSdpResultReceiverTest4::NewL(*test.Console());
	CleanupStack::PushL(rcvr);
	CSdpAgent* agent = CSdpAgent::NewLC(*rcvr, devAddr);
	rcvr->SetAgent(agent);
	CSdpSearchPattern* list = CSdpSearchPattern::NewL();
	CleanupStack::PushL(list);
	//Use serial port UUID
	// - This is used in 165+ service records in our 
	//   prequal test SDP database.
	list->AddL(0x1101); 
	agent->SetRecordFilterL(*list);
	agent->NextRecordRequestL();
	CActiveScheduler::Start();
	test.Next(_L("\n\n\nEnd of test 4\n\n\n"));
	CleanupStack::PopAndDestroy(3); //list, agent, rcvr

	test.End();
	}


class CSdpResultReceiverTest5 : public CSdpResultReceiver
	{
public:
	static CSdpResultReceiverTest5* NewL(CConsoleBase& aConsole);
	virtual void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount)
		{
		iCounter++;
		if(!iAgent || iCounter > aTotalRecordsCount || aError == KErrEof)
			{
			iCounter = 0;
			CActiveScheduler::Stop();
			return;
			}
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nNextRecordRequestComplete\n"));
		iConsole.Printf(_L("Got Error: <%d>\n"), aError);
		iConsole.Printf(_L("Got Total Records Count: <%d>\n"), aTotalRecordsCount);
		iConsole.Printf(_L("Got Handle: <0x%x>\n"), aHandle);
		if(aError)
			{
			iAgent->NextRecordRequestL();
			}
		else
			{
			CSdpAttrIdMatchList* matchList = CSdpAttrIdMatchList::NewL();
			CleanupStack::PushL(matchList);
			matchList->AddL(TAttrRange(0x0000));
			iAgent->AttributeRequestL(aHandle, *matchList);
			CleanupStack::PopAndDestroy(); //matchList - AttributeRequestL makes a copy
			}
		iConsole.Printf(_L("PRESS ANY KEY\n"));
		test.Getch();
		};

	virtual void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nAttributeRequestResult\n"));
		iConsole.Printf(_L("Got Handle: <0x%x>\n"), aHandle);
		iConsole.Printf(_L("Got Attribute: ID is: <0x%04x>\n"), aAttrID);
		delete aAttrValue; //ownership is passed here
		iConsole.Printf(_L("PRESS ANY KEY\n"));
		test.Getch();
		if(!iAgent)
			{
			CActiveScheduler::Stop();
			}
		else
			{
			}
		};

	virtual void AttributeRequestComplete(TSdpServRecordHandle aHandle, TInt aError)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nAttributeRequestComplete\n"));
		iConsole.Printf(_L("Got Error: <%d>\n"), aError);
		iConsole.Printf(_L("PRESS ANY KEY\n"));
		test.Getch();
		if(!iAgent||aError||iARCounter>=4)
			{
			iARCounter = 0;
			CActiveScheduler::Stop();
			return;
			}
		CSdpAttrIdMatchList* matchList = CSdpAttrIdMatchList::NewL();
		CleanupStack::PushL(matchList);
		switch(iARCounter)
			{
		case 0:
			matchList->AddL(TAttrRange(0x01, 0x02));
			break;
		case 1:
			matchList->AddL(TAttrRange(0x03, 0x04));
			break;
		case 2:
			matchList->AddL(TAttrRange(0x05));
			break;
		case 3:
			matchList->AddL(TAttrRange(0x06, 0x07));
			matchList->AddL(TAttrRange(0x0201));
			break;
		default:
			matchList->AddL(TAttrRange(0x0000, 0xffff)); //getemall
			break;
			}
		iAgent->AttributeRequestL(aHandle, *matchList);
		CleanupStack::PopAndDestroy(); //matchList - AttributeRequestL makes a copy
		iARCounter++;
		};
private:
	CSdpResultReceiverTest5(CConsoleBase& aConsole) :CSdpResultReceiver(aConsole), iARCounter(0){};
	TUint iARCounter;
	};

CSdpResultReceiverTest5* CSdpResultReceiverTest5::NewL(CConsoleBase& aConsole)
	{
	CSdpResultReceiverTest5* self = new(ELeave) CSdpResultReceiverTest5(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


void Test5()
/**
	Test5()
**/
	{
	test.Start(_L("Test 2 -- Published Agent API"));
	CSdpResultReceiverTest5* rcvr = CSdpResultReceiverTest5::NewL(*test.Console());
	CleanupStack::PushL(rcvr);
	CSdpAgent* agent = CSdpAgent::NewLC(*rcvr, devAddr);
	rcvr->SetAgent(agent);
	CSdpSearchPattern* list = CSdpSearchPattern::NewL();
	CleanupStack::PushL(list);
	list->AddL(0x0100);
	agent->SetRecordFilterL(*list);
	agent->NextRecordRequestL();
	CActiveScheduler::Start();
	test.Next(_L("\n\n\nEnd of test 5\n\n\n"));
	CleanupStack::PopAndDestroy(3); //list, agent, rcvr

	test.End();
	}


class CSdpResultReceiverTest6 : public CSdpResultReceiver
	{
public:
	static CSdpResultReceiverTest6* NewL(CConsoleBase& aConsole);
	~CSdpResultReceiverTest6();
	virtual void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount)
		{
		iCounter++;
		if(!iAgent || iCounter > aTotalRecordsCount || aError == KErrEof)
			{
			iCounter = 0;
			CActiveScheduler::Stop();
			return;
			}
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nNextRecordRequestComplete\n"));
		iConsole.Printf(_L("Got Error: <%d>\n"), aError);
		iConsole.Printf(_L("Got Total Records Count: <%d>\n"), aTotalRecordsCount);
		iConsole.Printf(_L("Got Handle: <0x%x>\n"), aHandle);
		if(aError)
			{
			iAgent->NextRecordRequestL();
			}
		else
			{
			CSdpAttrIdMatchList* matchList = CSdpAttrIdMatchList::NewL();
			CleanupStack::PushL(matchList);
			matchList->AddL(TAttrRange(0x0000, 0x0007)); //get up to id 6
			iAgent->AttributeRequestL(iBuilder, aHandle, *matchList);
			CleanupStack::PopAndDestroy(); //matchList - AttributeRequestL makes a copy
			}
		iConsole.Printf(_L("PRESS ANY KEY\n"));
		test.Getch();
		};

	virtual void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nAttributeRequestResult\n"));
		iConsole.Printf(_L("Got Handle: <0x%x>\n"), aHandle);
		iConsole.Printf(_L("Got Attribute: ID is: <0x%04x>\n"), aAttrID);
		delete aAttrValue; //ownership is passed here
		iConsole.Printf(_L("PRESS ANY KEY\n"));
		test.Getch();
		if(!iAgent)
			{
			CActiveScheduler::Stop();
			}
		else
			{
			}
		};

	virtual void AttributeRequestComplete(TSdpServRecordHandle, TInt aError)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nAttributeRequestComplete\n"));
		iConsole.Printf(_L("Got Error: <%d>\n"), aError);
		iConsole.Printf(_L("PRESS ANY KEY\n"));
		test.Getch();
		if(!iAgent||aError)
			{
			CActiveScheduler::Stop();
			return;
			}
		iAgent->NextRecordRequestL();
		};
private:
	CSdpResultReceiverTest6(CConsoleBase& aConsole) :CSdpResultReceiver(aConsole){};
	void ConstructL();
private:
	CConsoleBuilder* iBuilder;
	};

CSdpResultReceiverTest6* CSdpResultReceiverTest6::NewL(CConsoleBase& aConsole)
	{
	CSdpResultReceiverTest6* self = new(ELeave) CSdpResultReceiverTest6(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSdpResultReceiverTest6::~CSdpResultReceiverTest6()
	{
	delete iBuilder;
	}

void CSdpResultReceiverTest6::ConstructL()
	{
	iBuilder = new(ELeave) CConsoleBuilder(iConsole);
	}

void Test6()
/**
	Test6()
**/
	{
	test.Start(_L("Test 6 -- Published Agent API"));
	CSdpResultReceiverTest6* rcvr = CSdpResultReceiverTest6::NewL(*test.Console());
	CleanupStack::PushL(rcvr);
	CSdpAgent* agent = CSdpAgent::NewLC(*rcvr, devAddr);
	rcvr->SetAgent(agent);
	CSdpSearchPattern* list = CSdpSearchPattern::NewL();
	CleanupStack::PushL(list);
	list->AddL(0x0100);
	agent->SetRecordFilterL(*list);
	agent->NextRecordRequestL();
	CActiveScheduler::Start();
	test.Next(_L("\n\n\nEnd of test 6\n\n\n"));
	CleanupStack::PopAndDestroy(3); //list, agent, rcvr

	test.End();
	}

void Test7()
/**
	Test7()
**/
	{
	test.Start(_L("Test 7 -- Published Agent API"));
	CSdpVerificationTests::RunTestL(*test.Console());
	test.Next(_L("\n\n\nEnd of test 7\n\n\n"));

	test.End();
	}

class CSdpResultReceiverTest8 : public CSdpResultReceiver
	{
public:
	static CSdpResultReceiverTest8* NewL(CConsoleBase& aConsole, CSdpSearchPattern& aList);
	void HandleTimeout()
		{
		iCounter = 0;
		CActiveScheduler::Stop();
		return;
		};

	virtual void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount)
		{
		iCounter++;
		if(!iAgent||iCounter>aTotalRecordsCount||aError)
			{
			iCounter = 0;
			CActiveScheduler::Stop();
			return;
			}
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nNextRecordRequestComplete\n"));
		iConsole.Printf(_L("Got Error: <%d>\n"), aError);
		iConsole.Printf(_L("Got Total Records Count: <%d>\n"), aTotalRecordsCount);
		iConsole.Printf(_L("Got Handle: <0x%x>\n"), aHandle);
		iConsole.Printf(_L("PRESS ANY KEY\n"));
		test.Getch();
		iAgent->NextRecordRequestL();
		};


	virtual void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nAttributeRequestResult\n"));
		iConsole.Printf(_L("Got Handle: <0x%x>\n"), aHandle);
		iConsole.Printf(_L("Got Attribute: ID is: <0x%04x>\n"), aAttrID);
		CAttrPrintVisitor* theVisitor = new CAttrPrintVisitor(iConsole);
		//theVisitor->VisitAttributeValueL(*aAttrValue, aAttrValue->Type());
		aAttrValue->AcceptVisitorL(*theVisitor);
		delete theVisitor;
		iConsole.Printf(_L("\nPRESS ANY KEY\n"));
		test.Getch();
		delete aAttrValue; //ownership is passed here
		//if(iCounter>1)
		//	User::Leave(KErrCompletion);
		if(!iAgent)
			{
			CActiveScheduler::Stop();
			return;
			}
		else
			{
			}
		};

	virtual void AttributeRequestComplete(TSdpServRecordHandle, TInt aError)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nAttributeRequestComplete\n"));
		iConsole.Printf(_L("Got Error: <%d>\n"), aError);
		iConsole.Printf(_L("PRESS ANY KEY (***ESC*** TO STOP)\n"));
		TInt c = test.Getch();
		if(!iAgent||aError)
			{
			CActiveScheduler::Stop();
			return;
			}
		if(c==27)
			{
			iAgent->Cancel();
			CActiveScheduler::Stop();
			return;
			}
		iAgent->NextRecordRequestL();
		};
	~CSdpResultReceiverTest8()
		{
		iTimeout->Cancel();
		delete iTimeout;
		};

private:
	void ConstructL();
private:
	CSdpResultReceiverTest8(CConsoleBase& aConsole, CSdpSearchPattern& aList) 
		:CSdpResultReceiver(aConsole), iTimeout(0), iList(aList)
		{
		iTimeoutCounter = 0;
		iDelay = 1;
		};
    TInt iTimeoutCounter;
	CTestTimeout* iTimeout;
	TInt iDelay;
	TInt iRenew;
	CSdpSearchPattern& iList;
	};

CSdpResultReceiverTest8* CSdpResultReceiverTest8::NewL(CConsoleBase& aConsole, CSdpSearchPattern& aList)
	{
	CSdpResultReceiverTest8* self = new(ELeave) CSdpResultReceiverTest8(aConsole, aList);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CSdpResultReceiverTest8::ConstructL()
	{
	iTimeout = CTestTimeout::NewL(*this);
	iTimeout->IssueRequest(iDelay);
	}



void Test8()
/**
	Test8()
**/
	{
	test.Start(_L("Test 8 -- Published Agent API"));
	CSdpSearchPattern* list = CSdpSearchPattern::NewL();
	CleanupStack::PushL(list);
	list->AddL(0x0100);
	CSdpResultReceiverTest8* rcvr = CSdpResultReceiverTest8::NewL(*test.Console(), *list);
	CleanupStack::PushL(rcvr);
	CSdpAgent* agent = CSdpAgent::NewLC(*rcvr, devAddr);
	rcvr->SetAgent(agent);
	agent->SetRecordFilterL(*list);
	agent->NextRecordRequestL();
	CActiveScheduler::Start();
	test.Next(_L("\n\n\nEnd of test 8\n\n\n"));
	CleanupStack::PopAndDestroy(3); //list, agent, rcvr
	test.End();
	}

class CSdpResultReceiverTest9 : public CSdpResultReceiver
	{
public:
	static CSdpResultReceiverTest9* NewL(CConsoleBase& aConsole, CSdpSearchPattern& aList);
	void HandleTimeout()
		{
		iAgent->Cancel();
		iConsole.Printf(_L("Type in an interrupt time delay, and THEN press..."));
		iConsole.Printf(_L("\n...return to restart"));
		iConsole.Printf(_L("\n...'d' to await next time out"));
		iConsole.Printf(_L("\n...'n' to restart and not do a further timeout"));
		iConsole.Printf(_L("\n...ESC to stop\n"));
		iDelay = 0;
		TInt c = test.Getch();
		TBool stop = EFalse;
		while(!stop)
			{
			switch(c)
				{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				iConsole.Printf(_L("%c"), c);
				iDelay*=10;
				iDelay+=(c - '0');
				c = test.Getch();
				break;
			case '\b':
				iDelay/=10;
				iConsole.Printf(_L("\b"));
				c = test.Getch();
				break;
			case 13:
				iAgent->SetRecordFilterL(iList);
				iAgent->NextRecordRequestL();
				iTimeout->IssueRequest(iDelay);
				stop = ETrue;
				break;
			case 'd':
			case 'D':
				iTimeout->IssueRequest(iDelay);
				stop = ETrue;
				break;
			case 'n':
			case 'N':
				iAgent->SetRecordFilterL(iList);
				iAgent->NextRecordRequestL();
				stop = ETrue;
				break;
			case 27:
				CActiveScheduler::Stop();
				stop = ETrue;
				break;
			default:
				c = test.Getch();
				break;
				}
			}
		iConsole.Printf(_L("\n"));
		};
	virtual void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount)
		{
		iCounter++;
		if(iCounter==2)
			{
			User::Leave(KErrGeneral);
			}
		if(!iAgent||aError==KErrEof)
			{
			iCounter = 0;
			CActiveScheduler::Stop();
			return;
			}
		if(iCounter>aTotalRecordsCount)
			{
			iCounter = 0;
			}
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nNextRecordRequestComplete\n"));
		iConsole.Printf(_L("Got Error: <%d>\n"), aError);
		iConsole.Printf(_L("Got Total Records Count: <%d>\n"), aTotalRecordsCount);
		iConsole.Printf(_L("Got Handle: <0x%x>\n"), aHandle);
		iConsole.Printf(_L("PRESS ANY KEY (***ESC*** TO STOP)\n"));
		TInt c = test.Getch();
		if(c==27)
			{
			iAgent->Cancel();
			CActiveScheduler::Stop();
			return;
			}
		if(aError!=KErrNone)
			{
			iAgent->SetRecordFilterL(iList);
			iAgent->NextRecordRequestL();
			}
		else
			{
			CSdpAttrIdMatchList* matchList = CSdpAttrIdMatchList::NewL();
			CleanupStack::PushL(matchList);
			matchList->AddL(TAttrRange(0x0000, 0xFFFF)); //getemall
			iAgent->AttributeRequestL(aHandle, *matchList);
			CleanupStack::PopAndDestroy(); //matchList - AttributeRequestL makes a copy
			}

		};

	virtual void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nAttributeRequestResult\n"));
		iConsole.Printf(_L("Got Handle: <0x%x>\n"), aHandle);
		iConsole.Printf(_L("Got Attribute: ID is: <0x%04x>\n"), aAttrID);
		CAttrPrintVisitor* theVisitor = new CAttrPrintVisitor(iConsole);
		//theVisitor->VisitAttributeValueL(*aAttrValue, aAttrValue->Type());
		aAttrValue->AcceptVisitorL(*theVisitor);
		delete theVisitor;
		iConsole.Printf(_L("\nPRESS ANY KEY\n"));
		test.Getch();
		delete aAttrValue; //ownership is passed here
		//if(iCounter>1)
		//	User::Leave(KErrCompletion);
		if(!iAgent)
			{
			CActiveScheduler::Stop();
			return;
			}
		else
			{
			}
		};

	virtual void AttributeRequestComplete(TSdpServRecordHandle aHandle, TInt aError)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nAttributeRequestComplete\n"));
		iConsole.Printf(_L("Got Error: <%d>\n"), aError);
		iConsole.Printf(_L("PRESS ANY KEY (***ESC*** TO STOP)\n"));
		TInt c = test.Getch();
		if(!iAgent||aError==KErrEof)
			{
			CActiveScheduler::Stop();
			return;
			}
		if(c==27)
			{
			iAgent->Cancel();
			CActiveScheduler::Stop();
			return;
			}
		if(aError)
		//try again
			{
			CSdpAttrIdMatchList* matchList = CSdpAttrIdMatchList::NewL();
			CleanupStack::PushL(matchList);
			matchList->AddL(TAttrRange(0x0000, 0xFFFF)); //getemall
			iAgent->AttributeRequestL(aHandle, *matchList);
			CleanupStack::PopAndDestroy(); //matchList - AttributeRequestL makes a copy
			}
		else
			{
			iAgent->NextRecordRequestL();
			}
		};
	~CSdpResultReceiverTest9()
		{
		iTimeout->Cancel();
		delete iTimeout;
		};

private:
	void ConstructL();
private:
	CSdpResultReceiverTest9(CConsoleBase& aConsole, CSdpSearchPattern& aList) 
		:CSdpResultReceiver(aConsole), iTimeout(0), iList(aList)
		{
		iTimeoutCounter = 0;
		iDelay = 5;
		};
    TInt iTimeoutCounter;
	CTestTimeout* iTimeout;
	TInt iDelay;
	TInt iRenew;
	CSdpSearchPattern& iList;
	};

CSdpResultReceiverTest9* CSdpResultReceiverTest9::NewL(CConsoleBase& aConsole, CSdpSearchPattern& aList)
	{
	CSdpResultReceiverTest9* self = new(ELeave) CSdpResultReceiverTest9(aConsole, aList);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CSdpResultReceiverTest9::ConstructL()
	{
	iTimeout = CTestTimeout::NewL(*this);
	iTimeout->IssueRequest(iDelay);
	}



void Test9()
/**
	Test9()
**/
	{
	test.Start(_L("Test 9 -- Published Agent API"));
	CSdpSearchPattern* list = CSdpSearchPattern::NewL();
	CleanupStack::PushL(list);
	list->AddL(0x0100);
	CSdpResultReceiverTest9* rcvr = CSdpResultReceiverTest9::NewL(*test.Console(), *list);
	CleanupStack::PushL(rcvr);
	CSdpAgent* agent = CSdpAgent::NewLC(*rcvr, devAddr);
	rcvr->SetAgent(agent);
	agent->SetRecordFilterL(*list);
	agent->NextRecordRequestL();
	CActiveScheduler::Start();
	test.Next(_L("\n\n\nEnd of test 9\n\n\n"));
	CleanupStack::PopAndDestroy(3); //list, agent, rcvr
	test.End();
	}



class CSdpResultReceiverTest0 : public CSdpResultReceiver
	{
public:
	static CSdpResultReceiverTest0* NewL(CConsoleBase& aConsole);
	virtual void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount)
		{
		iCounter++;
		if(iCounter==2)
			{
			User::Leave(KErrGeneral);
			}
		if(!iAgent||(iCounter>aTotalRecordsCount && aError==KErrNone))
			{
			iCounter = 0;
			CActiveScheduler::Stop();
			return;
			}
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nNextRecordRequestComplete\n"));
		iConsole.Printf(_L("Got Error: <%d>\n"), aError);
		iConsole.Printf(_L("Got Total Records Count: <%d>\n"), aTotalRecordsCount);
		iConsole.Printf(_L("Got Handle: <0x%x>\n"), aHandle);
		iConsole.Printf(_L("PRESS ANY KEY\n"));
		test.Getch();
		iAgent->NextRecordRequestL();
		};

	virtual void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nAttributeRequestResult\n"));
		iConsole.Printf(_L("Got Handle: <0x%x>\n"), aHandle);
		iConsole.Printf(_L("Got Attribute: ID is: <0x%04x>\n"), aAttrID);
		delete aAttrValue; //ownership is passed here
		iConsole.Printf(_L("PRESS ANY KEY\n"));
		test.Getch();
		if(!iAgent)
			{
			CActiveScheduler::Stop();
			}
		else
			{
			}

		};

	virtual void AttributeRequestComplete(TSdpServRecordHandle, TInt aError)
		{
		iConsole.SetPos(iIndent);
		iConsole.Printf(_L("\n\nAttributeRequestComplete\n"));
		iConsole.Printf(_L("Got Error: <%d>\n"), aError);
		iConsole.Printf(_L("PRESS ANY KEY\n"));
		test.Getch();
		if(!iAgent||aError)
			{
			CActiveScheduler::Stop();
			return;
			}
		iAgent->NextRecordRequestL();
		};
private:
	CSdpResultReceiverTest0(CConsoleBase& aConsole) :CSdpResultReceiver(aConsole){};
	};

CSdpResultReceiverTest0* CSdpResultReceiverTest0::NewL(CConsoleBase& aConsole)
	{
	CSdpResultReceiverTest0* self = new(ELeave) CSdpResultReceiverTest0(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}



void Test0()
/**
	Test0()
**/
	{
	test.Start(_L("Test 0 -- Published Agent API"));
	CSdpResultReceiverTest0* rcvr = CSdpResultReceiverTest0::NewL(*test.Console());
	CleanupStack::PushL(rcvr);
	CSdpAgent* agent = CSdpAgent::NewLC(*rcvr, devAddr);
	rcvr->SetAgent(agent);
	CSdpSearchPattern* list = CSdpSearchPattern::NewL();
	CleanupStack::PushL(list);
	list->AddL(0x0100);
	agent->SetRecordFilterL(*list);
	agent->NextRecordRequestL();
	CActiveScheduler::Start();
	test.Next(_L("\n\n\nEnd of test 0\n\n\n"));
	CleanupStack::PopAndDestroy(3); //list, agent, rcvr

	test.End();
	}

void GetDeviceAddressL()
	{
	RNotifier notifier;
    notifier.Connect();
    TBTDeviceSelectionParamsPckg pckg;
    TBTDeviceResponseParamsPckg resPckg;
    //TBTDevAddrPckg resPckg;
    TRequestStatus stat;
    notifier.StartNotifierAndGetResponse(stat, KDeviceSelectionNotifierUid, pckg, resPckg);
    User::WaitForRequest(stat);
    notifier.CancelNotifier(KDeviceSelectionNotifierUid);
    notifier.Close();
    User::LeaveIfError(stat.Int());
	devAddr = resPckg().BDAddr();

	TBTDeviceName dname = resPckg().DeviceName();
	TBTDeviceClass dclass = resPckg().DeviceClass();
	return;
	}

void RunTest(TChar aUserSelection)
	{
	__UHEAP_MARK;

	switch (aUserSelection)
		{
	case 'A':
	case 'a':
		TRAPD(errA, GetDeviceAddressL());
		test.Printf(_L("Test %c finished with result %d\n"), (TUint) aUserSelection, errA);
		break;
	case '1':
		TRAPD(err1, Test1());
		test.Printf(_L("Test %c finished with result %d\n"), (TUint) aUserSelection, err1);
		break;
	case '2':
		TRAPD(err2, Test2());
		test.Printf(_L("Test %c finished with result %d\n"), (TUint) aUserSelection, err2);
		break;
	case '3':
		TRAPD(err3, Test3());
		test.Printf(_L("Test %c finished with result %d\n"), (TUint) aUserSelection, err3);
		break;
	case '4':
		TRAPD(err4, Test4());
		test.Printf(_L("Test %c finished with result %d\n"), (TUint) aUserSelection, err4);
		break;
	case '5':
		TRAPD(err5, Test5());
		test.Printf(_L("Test %c finished with result %d\n"), (TUint) aUserSelection, err5);
		break;
	case '6':
		TRAPD(err6, Test6());
		test.Printf(_L("Test %c finished with result %d\n"), (TUint) aUserSelection, err6);
		break;
	case '7':
		TRAPD(err7, Test7());
		test.Printf(_L("Test %c finished with result %d\n"), (TUint) aUserSelection, err7);
		break;
	case '8':
		TRAPD(err8, Test8());
		test.Printf(_L("Test %c finished with result %d\n"), (TUint) aUserSelection, err8);
		break;
	case '9':
		TRAPD(err9, Test9());
		test.Printf(_L("Test %c finished with result %d\n"), (TUint) aUserSelection, err9);
		break;
	case '0':
		TRAPD(err0, Test0());
		test.Printf(_L("Test %c finished with result %d\n"), (TUint) aUserSelection, err0);
		break;
	default:
		test.Printf(_L("Unknown command\r\n"));
		break;
		};		
	__UHEAP_MARKEND;	
	}
	
TInt E32Main()
	{

	CTrapCleanup* cleanup=CTrapCleanup::New(); // get clean-up stack

//	TInt ret;
	test.SetLogged(EFalse);
	test.Title();
	test.Start(_L("Loading SDP Protocol Module"));
// For some reason, you have to do the following to
// ensure that the file server behaves properly.
	RFs fs;
	(void)fs.Connect();
	fs.Close();

	LoadLDD_PDD();	// Loading the LDD/PDD pair



	//Shove it all in a TRAP harness to allow cleanup to work...
	TRAPD(
		err,
		CActiveScheduler *scheduler = new (ELeave) CActiveScheduler;
		CleanupStack::PushL(scheduler);
		CActiveScheduler::Install(scheduler); 
		FOREVER
			{
			test.Printf(_L("Choose the test to run...\r\n"));
			test.Printf(_L("************************\r\n"));
			test.Printf(_L("1. Open, connect\r\n"));
			test.Printf(_L("2. CSdpAgent API - UUIDs restrict to one record\r\n"));
			test.Printf(_L("3. CSdpAgent API - UUIDs allow 3 records\r\n"));
			test.Printf(_L("4. CSdpAgent API - Just do record requests\r\n"));
			test.Printf(_L("5. CSdpAgent API - Do one record request + 5 attr requests\r\n"));
			test.Printf(_L("6. CSdpAgent API - Provide own builder\r\n"));
			test.Printf(_L("7. CSdpAgent API - Attempt in-house SDP prequal\r\n"));
			test.Printf(_L("8. CSdpAgent API - Interrupted Request\r\n"));
			test.Printf(_L("9. CSdpAgent API - UUIDs allow 3 records - with Cancel Facility and Forced Error\r\n"));
			test.Printf(_L("0. CSdpAgent API - Just do record requests - with Forced Error\r\n"));
			test.Printf(_L("************************\r\n\r\n"));
			test.Printf(_L("A. Alter Remote Device Address From Default Value\r\n(NB This action needs a compiled TEXT NOTIFIER)\r\n\r\n"));
			test.Printf(_L("press Escape to quit\r\n\r\n"));

			
			TChar c = test.Getch();
			if (c == EKeyEscape)
				{
				goto done;
				}
			else
				{
				RunTest(c);
				}
			}
		done:
		CleanupStack::PopAndDestroy(/*scheduler*/);
	); //end of TRAPD!!!!!!!!!!
	if (err)
		{
		test.Printf(_L("Failed to allocate a CActiveScheduler. err: %d!\n"), err);
		}
	
	test.Printf(_L("Finished! Press any key...\r\n"));
	test.Getch();
	test.End();

	delete cleanup; // destroy clean-up stack
	return KErrNone;
	}
