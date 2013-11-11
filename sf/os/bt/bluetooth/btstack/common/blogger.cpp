// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "blogger.h"

#ifndef BLOGGING__
EXPORT_C TAny* CBlogger::GetFloggerInfo()
	{
	return 0;
	}
#endif

#ifdef BLOGGING__
#include "rfcommmuxer.h"
#include "rfcommframe.h"


CBlogger::~CBlogger()
	{
	delete ifilename;
	ifile.Close ();
	ifileserv.Close ();
	}

CBlogger::CBlogger() : 
	iOutputWindowSize(KBlogOutputWindowDefault),
	iVerbosityLevel(KBlogVerbosityAllLevels), 
	iLayersToLogMask(KBlogAllLayers),
	iLoggingMask(KBlogTypeLogAllMask)
	{
	}

CBlogger* CBlogger::NewL()
	{
	CBlogger* self = new (ELeave)CBlogger();
	self->ConstructL();
	return self;
	}

void CBlogger::ConstructL()
	{
	User::LeaveIfError(ifileserv.Connect());
	MakefilenameL(_L("Blog.txt"));
	iFileExists = EFalse;
	}

TInt CBlogger::LoggingMask(TInt aLayer, TInt aVerbosity) const
	{
	if(aLayer&iLayersToLogMask)
		{
		if(aVerbosity<=iVerbosityLevel)
			{
			return iLoggingMask;
			}
		}
	//else
	return 0;
	}

CBloggette* CBlogger::GetLayerBloggette(TInt aLayer)
	{
	switch(aLayer)
		{
		case KBlogLayerLinkLayer :
			{
			return iLLBloggette;
			}
		case KBlogLayerL2CAP :
			{
			return iL2Bloggette;
			}
		case KBlogLayerRFComm :
			{
			return iRFBloggette;
			}
		default:
			{
			Panic(EBloggerLayerRequestedOutOfRange);
			return NULL; //for compiler to shut up
			}
		};
	}

void CBlogger::Register(CBloggette* aLayerBloggette)
/** 
	only one per layer please
*/
	{
	TUint layer = aLayerBloggette->Layer();
	switch(layer)
		{
		case KBlogLayerLinkLayer:
			{
			__ASSERT_DEBUG(iLLBloggette==NULL,Panic(EBloggerLayerBloggetteAlreadyRegistered));
			iLLBloggette=aLayerBloggette;
			break;
			}
		case KBlogLayerL2CAP :
			{
			__ASSERT_DEBUG(iL2Bloggette==NULL,Panic(EBloggerLayerBloggetteAlreadyRegistered));
			iL2Bloggette=aLayerBloggette;
			break;
			}
		case KBlogLayerRFComm :
			{
			__ASSERT_DEBUG(iRFBloggette==NULL,Panic(EBloggerLayerBloggetteAlreadyRegistered));
			iRFBloggette=aLayerBloggette;
			break;
			}
		default:
			{
			Panic(EBloggerLayerRegistrationOutOfRange);
			}
		};
	}


TInt CBlogger::UpdateLoggingSettings(TUint aOptionType,const TDesC8& aOption)
	{
#define CHECK_OPT_IS_(type) {if (aOption.Size() != sizeof(type)) \
										return KErrArgument;}
	switch(aOptionType)
		{
		case KBloggerSetLayers:
			{
			CHECK_OPT_IS_(TInt);
			iLayersToLogMask= *(TUint*)aOption.Ptr();
			break;
			}
		case KBloggerSetVerbosity:
			{
			CHECK_OPT_IS_(TInt);
			TInt verbosity=*(TUint*)aOption.Ptr();
			if((verbosity<KBlogVerbosityOff)||(verbosity>KBlogVerbosityAllLevels))
				{
				return KErrArgument;
				}
			iVerbosityLevel= verbosity;
			break;
			}
		case KBloggerSetLoggingMask:
			{
			CHECK_OPT_IS_(TInt);
			iLoggingMask= *(TInt*)aOption.Ptr();
			break;
			}
		case KBloggerSetMedium:
			{
			CHECK_OPT_IS_(TInt);
			TInt mediumType=*(TUint*)aOption.Ptr();
			if((mediumType!=KBlogOutputTypeIsFlogger)||(mediumType!=KBlogOutputTypeIsRDebug))
				{//otherwise we will panic when we try to output
				return KErrArgument;
				}
			iMediumType= mediumType;
			break;
			}
		case KBloggerSetTestCaseName:
			{
			TInt length=Min(KRDBMaxLit,aOption.Length());
			iTestCaseLit.Copy(((TDesC16*)aOption.Ptr())->Left(length));
			BlogTestCase(iTestCaseLit);
			break;
			}
		case KBloggerSetOutputWindow:
			{
			CHECK_OPT_IS_(TInt);
			TInt windowSize=*(TUint*)aOption.Ptr();
			if((windowSize<1)||(windowSize>KRDBMaxLit))
				{
				return KErrArgument;
				}
			iOutputWindowSize=windowSize;			
			break;
			}
		case KBloggerSetTestCaseResult:
			{
			CHECK_OPT_IS_(TInt);
			TInt tcResult=*(TUint*)aOption.Ptr();
			if(tcResult>KBlogTestCaseResultUpperNo)
				{
				return KErrArgument;	
				}
			BlogTestCaseResult(tcResult);
			break;
			}
		case KBloggerSetTesterMessage:
			{
			BlogTesterMessage(*(TDesC16*)aOption.Ptr());						
			break;
			}
		case KBloggerSetAllOptions:
			{
			CHECK_OPT_IS_(TBlogSettings); // we care about the data pckgd not he pckg itself
			TBlogSettingsPckg theSettings;
			theSettings.Copy(aOption);

			TInt verbosity=(theSettings()).iVerbosity;
			if((verbosity<KBlogVerbosityOff)||(verbosity>KBlogVerbosityAllLevels))
				{
				return KErrArgument;
				}

			TInt mediumType=(theSettings()).iMediumToLog;
			if((mediumType!=KBlogOutputTypeIsFlogger)&&(mediumType!=KBlogOutputTypeIsRDebug))
				{//otherwise we will panic when we try to output
				return KErrArgument;
				}

			TInt windowSize=(theSettings()).iOutputWindowSize;
			if((windowSize<1)||(windowSize>KRDBMaxLit))
				{
				return KErrArgument;
				}

			iLayersToLogMask  =(theSettings()).iLayersToLog;
			iVerbosityLevel   =verbosity;
			iLoggingMask      =(theSettings()).iLoggingMask;
			iMediumType       =mediumType;
			iOutputWindowSize =windowSize;
			iTestCaseLit      =(theSettings()).iTestCaseLit;
			
			if (iTestCaseLit!=KNullDesC)
				BlogTestCase(iTestCaseLit);

			break;
			}
		default:
			{
			return KErrArgument;
			}
		};
	return KErrNone;
	}


void CBlogger::Panic(TBloggerPanic aPanic)
	{
	User::Panic(_L("BLogger Panic"),aPanic);
	}

void CBlogger::Blog(const TDesC& aMessage,TInt aFrameWindow)
/**
	Formats the message with a date and time preamble and blogs it.
	But a limitation is that although it breaks the message in parts of
	aFrameWindow size characters, it cannot accept amessage which is longer 
	than KBlogMessageMaxLitLength
*/
	{
	__ASSERT_DEBUG((KBlogMessageMaxLitLength-KBlogDateTimeLitLength)>=aMessage.Length(),Panic(EBloggerMessageLengthTooLong));
	TBuf<KBlogMessageMaxLitLength> logMessage;
	DoDateTimeFormat(logMessage);
	logMessage.Append(aMessage);
	DoBlog(logMessage,aFrameWindow);
	}

void CBlogger::Blog(const TDesC& aMessage)
/**
	Formats the message with a date and time preamble and blogs it.
	But a limitation is that it cannot accept a message which is longer 
	than KBlogMessageMaxLitLength.
*/
	{	
	Blog(aMessage,iOutputWindowSize);
	}


void CBlogger::BlogTestCase(TDesC& aTestCaseName)
	{
	// output a preamble indicating new test case (or a repeat of the previous one:-)
	// include time date and a Lit for the preamble
	__ASSERT_DEBUG(KBlogTestCaseMaxLitLength>=KBlogDateTimeLitLength,Panic(EBloggerDescriptorLengthWasShorter));
	TBuf<KBlogTestCaseMaxLitLength> testCaseString;
	DoDateTimeFormat(testCaseString); // no need for ret val check here	
	testCaseString.Append(KBlogTestCaseLit);
	testCaseString.Append(aTestCaseName);
	DoBlog(testCaseString,iOutputWindowSize);
	}

void CBlogger::BlogTesterMessage(TDesC& aTesterMessage)
/**
	A Test App can output a string in the Blogger logs through this method.
	To do this the test app should issue a SetOpt of type KBloggerSetTesterMessage
	and pass a packaged TDesC16.
*/
	{
	TBuf<KBlogTesterMessageInfoMaxLitLength> tstInfoString;
	DoDateTimeFormat(tstInfoString);
	tstInfoString.Append(KBlogTesterMessageInfoLit);
	tstInfoString.Append(aTesterMessage);
	DoBlog(tstInfoString,iOutputWindowSize);
//	DoBlog(aTesterMessage,iOutputWindowSize);
	}

void CBlogger::BlogTestCaseResult(TInt aTestCaseResult)
	{
	if(!(iTestCaseLit.Length())) //in the case the TC Lit was not setup
		{
		return;
		}

	TBuf<KBlogTestCaseResultMaxLitLength> tcMessage;
	__ASSERT_DEBUG(KBlogTestCaseResultMaxLitLength>KBlogDateTimeLitLength,Panic(EBloggerDescriptorLengthWasShorter));	
	DoDateTimeFormat(tcMessage);
	
	switch(aTestCaseResult)
		{
		case KBlogTestCaseResultPassed:
			{
			tcMessage.Append(KBlogTestCaseResultLitPass);
			break;
			}
		case KBlogTestCaseResultFailed:
			{
			tcMessage.Append(KBlogTestCaseResultLitFailed);
			break;
			}
		case KBlogTestCaseResultInconclusive:
			{
			tcMessage.Append(KBlogTestCaseResultLitInconclusive);
			break;
			}
		case KBlogTestCaseResultSkipped:
			{
			tcMessage.Append(KBlogTestCaseResultLitSkipped);
			break;
			}
		default:
			{
			// check your bounds checking in the handler
			Panic(EBloggerTestCaseResultIsBogus); 
			}
		};
	
	tcMessage.Append(iTestCaseLit);
	DoBlog(tcMessage,iOutputWindowSize);
	}

void CBlogger::DoBlog(const TDesC& aMessage,TInt aFrameWindow)
	{	
	TInt msgLength=aMessage.Length();
	if(!msgLength)
		{return;}

	TInt min=Min(KRDBMaxLit,aMessage.Length());
	TInt len=Min(min,aFrameWindow);
		
	TInt frames=msgLength/len; 
	TInt lastFrameSize=msgLength%len;// i.e the remainder
	
	TInt position=0;
	for(TInt f=0;f<frames;f++,position=f*len)
		{
		DoOutputToResourceL(aMessage.Mid(position,len)); //Qualified
		}
	// log last frame
	if(lastFrameSize)
		{		
		DoOutputToResourceL(aMessage.Mid(position));  //Qualified
		}
	}

TInt CBlogger::DoDateTimeFormat(TDes& aDesToPrepend)
	{

	//FIXME:if the options are for time critical logging then spare the date and time 
	
	if(aDesToPrepend.MaxLength()<KBlogDateTimeLitLength)
		{
		return KErrArgument;
		}
	TBuf<KBlogDateTimeLitLength> timeLit; // abit more bytes than needed
	TTime time;
	time.HomeTime();
	TRAPD(errD,time.FormatL(aDesToPrepend,KBlogDateFormat));
	if(errD)
		{
		//since we OOMed
		aDesToPrepend.Append(KBlogBlankDate);
		}
	TRAPD(errT,time.FormatL(timeLit,KBlogTimeFormat));
	if (errT)
		{
		timeLit.Copy(KBlogBlankTime);
		}

	aDesToPrepend.Append(timeLit);

	// although OOM may have happened, but the point is to get the log out quickly
	return KErrNone; 
	}


void CBlogger::HexDump(const TDesC& aDescription,const TDesC8& aHexFrame, TInt aFrameWindow)
/**
	It will trim the sDescription to fit in the aFrameWindow size.
	It also log the HexFrame in chunks of size=aFrameWindow.
*/
	{
	//255 is the max RDebug::Print size

	DoBlog(aDescription,aFrameWindow);

	TInt bufferSize=aHexFrame.Length();
	if(!bufferSize)
		{return;}

	TInt frames=bufferSize/(aFrameWindow/5); // 5 is the size of the formating lit 
	TInt lastFrameSize=(bufferSize*5)%aFrameWindow;// i.e the remainder
	
	TInt i=0;
	TBuf<KRDBMaxLit> outBuf;

	for ( TInt f=0;f<frames;f++)
		{
		outBuf.Zero();
		for ( i = 0; i < aFrameWindow/5; ++i)
			{// 5 is the size of the formating lit :-)
			outBuf.AppendFormat(_L("0x%02x "),aHexFrame[(f*aFrameWindow/5)+i]);			
			}
		DoOutputToResourceL(outBuf); //Qualified
		}

	// log last frame
	if(lastFrameSize)
		{
		outBuf.Zero();
		for(i=(frames*aFrameWindow/5);i<bufferSize;i++)
			{
			outBuf.AppendFormat(_L("0x%02x "),aHexFrame[i]);
			}
		DoOutputToResourceL(outBuf); //Qualified
		}
	}
void CBlogger::CreateBlogFileL()
	{
	//User::LeaveIfError(ifileserv.Connect());
	User::LeaveIfError(ifile.Replace(ifileserv, *ifilename, EFileStream|EFileWrite));
	}

void CBlogger::DoFileL()
	{
	/*
	if the file exists and is open then write to it 
	otherwise create and open it
	*/
	TInt ret;
	ret=ifile.Open(ifileserv,*ifilename,EFileStream|EFileWrite);	
	if (ret==KErrNotFound) // file does not exist - create it
		{
		CreateBlogFileL();
		}
	iFileExists = ETrue;
	}

void CBlogger::MakefilenameL(const TDesC &aFileName)
	{
	//Connect to Fileserver
	User::LeaveIfNull(ifilename=aFileName.AllocL());
	TInt err = ifileserv.MkDirAll(*ifilename);
	if ( (err != KErrAlreadyExists) && (err != KErrNone))
		User::Leave(err);
	}

void CBlogger::DoOutputToResourceL(const TDesC& aMessage)
	{
	// It is reasonable to assume we only gonna have 2 types of output resource
	// either COM:0 or File Flogging thus do the sipmlest thing here
	
	//iMediumType=1; //need this to be 1 for output to a file. 

	switch(iMediumType) //yes I like dynamic binding myself but we only gonna have 2 isn't it ?
		{
		case KBlogOutputTypeIsRDebug:
			{
			RDebug::Print(_L("%S"),&aMessage);
			break;
			}
		case KBlogOutputTypeIsFlogger:
			{
			//User::LeaveIfError(ifileserv.Connect());
			//Makefilename(_L("Blog.txt"));
			if(!iFileExists)
				{
				DoFileL();
				}			
			TBuf8<256> buf8;
			buf8.Copy(aMessage);
			User::LeaveIfError(ifile.Write(buf8));
			User::LeaveIfError(ifile.Write(_L8("\r\n")));
			User::LeaveIfError(ifile.Flush());
			break;
			}
		default:
			{
			Panic(EBloggerResourceTypeIsBogus);
			}
		};

	}

void CBlogger::StoreFloggerInfo(TAny* aLogInfo)
/**
	Store the pointer from the HCI's TLS that indicates to which file the floffer should log.
	This is necessary in order to be able (at will) to accomodate both Flogger
	and Blogger functionality in the stack.

	This method will only be used from the LinkMgr when Flogging is enabled.
*/
	{
	iFloggerInfo=aLogInfo;
	}


EXPORT_C TAny* CBlogger::GetFloggerInfo()
	{
	return iFloggerInfo;
	}

void CBlogger::DoBloggerTests()
	{
#ifdef BLOGGING__SELF_TESTS__
	LLQBLOG(1,Log(_L(" >>>>>>>>>>>>>>>>>> This is %d <<<<<<<<<<<<<<<<"),1));
	LLQBLOG(1,Log(_L("123456789a123456789b123456789c123456789d123456789e123456789f123456789g123456789h123456789i")));
	LLQBLOG(1,Log(_L(" a string %S and a number %d"),&_L("Andy"), 1));
	LLQBLOG(1,HexDump(_L("A hex dump"),_L8(" read hex mate?"),10));
	LLQBLOG(1,HexDump(_L("A long hex dump"),_L8("123456789a123456789b123456789c123456789d123456789e123456789f123456789g"),80));
	
	// Testing test case Lit setup 
	TBuf<KRDBMaxLit> tcName(_L("My First Test Case ,NO1"));
	TPckgBuf<TBlogTestCaseLit> tcNamePckg(tcName);
	UpdateLoggingSettings(KBloggerSetTestCaseName,tcNamePckg);
	
	// Testing Test Case Results
	TPckgBuf<TInt> tcResult(KBlogTestCaseResultPassed);
	UpdateLoggingSettings(KBloggerSetTestCaseResult,tcResult);

	//Testing Test app messages
	TBuf<KRDBMaxLit> tstMsg(_L("This is message from the tester saying it is doing a test"));
	TPckgBuf<TBlogTestCaseLit> tstMsgPckg(tstMsg);
	UpdateLoggingSettings(KBloggerSetTesterMessage,tstMsgPckg);
#endif
	}

// Blogettes

CBloggette::~CBloggette()
	{
	}

CBloggette::CBloggette(CBlogger* aProvider, TInt aLayer) : 
	iLayer(aLayer),
	iBlogger(aProvider)
	{
	}

void CBloggette::Log(TRefByValue<const TDesC> aFormatingLit, ...)
/**
	The maximum length of the allowed string is 255.
*/
	{
	VA_LIST tmpArgList;
	VA_START(tmpArgList,aFormatingLit);
	TBuf<KRDBMaxLit> blogBuf; 
	blogBuf.Zero();
	blogBuf.AppendFormatList(aFormatingLit, tmpArgList); //FIXME overflow handler possibly ?
	iBlogger->Blog(blogBuf);
	VA_END(tmpArgList);
	}

  
void CBloggette::HexDump(TRefByValue<const TDesC> aDescription,const TDesC8& aHexFrame, TInt aFrameWindow)
	{
	iBlogger->HexDump(aDescription,aHexFrame,aFrameWindow);
	}

TInt CBloggette::Layer() const
	{
	return iLayer;
	}


// Link Layer Bloggette

CLLBloggette* CLLBloggette::NewL(CBlogger* aProvider, TInt aLayer)
	{
	CLLBloggette* self = new (ELeave)CLLBloggette(aProvider,aLayer);
	self->iBlogger->Register(self);
	return self;
	}

CLLBloggette::CLLBloggette(CBlogger* aProvider, TInt aLayer) 
	: CBloggette(aProvider,aLayer)
	{	
	}

CLLBloggette::~CLLBloggette()
	{
	}

TDesC CLLBloggette::NameDes()
	{
	return _L("LL Dummy Bloggette");
	}

// L2CAP Bloggette

CL2Bloggette* CL2Bloggette::NewL(CBlogger* aProvider, TInt aLayer)
	{
	CL2Bloggette* self = new (ELeave)CL2Bloggette(aProvider,aLayer);
	self->iBlogger->Register(self);
	return self;
	}

CL2Bloggette::CL2Bloggette(CBlogger* aProvider, TInt aLayer) 
	: CBloggette(aProvider,aLayer)
	{	
	}

CL2Bloggette::~CL2Bloggette()
	{
	}

TDesC CL2Bloggette::NameDes()
	{
	return _L("L2CAP Dummy Bloggette");
	}

// RFCOMM Bloggette

CRFBloggette* CRFBloggette::NewL(CBlogger* aProvider, TInt aLayer)
	{
	CRFBloggette* self = new (ELeave)CRFBloggette(aProvider,aLayer);
	self->iBlogger->Register(self);
	return self;
	}

CRFBloggette::CRFBloggette(CBlogger* aProvider, TInt aLayer) 
	: CBloggette(aProvider,aLayer)
	{	
	}

CRFBloggette::~CRFBloggette()
	{
	}

TDesC CRFBloggette::NameDes()
	{
	return _L("RFComm Dummy Bloggette");
	}
	
void CRFBloggette::Timeout(/*CRfcommSAP* aSAP, CRfcommMuxer* aMux,*/ CRfcommFrame* aFrm )
	{
	TUint8 ctrlfield = aFrm->Ctrl();

	switch((ctrlfield&~KPollFinalBitmask))
		{
		case KSABMCtrlField:
			Log(KBlogTimeout);
			Log(KBlogSABM);
			break;
		case KDISCCtrlField:
			Log(KBlogTimeout);
			Log(KBlogDISC);
			break;
		default:
			break;
		};
	}

void CRFBloggette::LogMuxCommand(CRfcommSAP* aSAP, CRfcommMuxer* aMux, TUint8 aCommand)
	{
	TUint8 signals;

	switch(aCommand)
		{
	case KTestType:
			Log(KBlogTestCommand); 
			break;
		case KPNType:
			Log(KBlogPN);//
			break;
		case KRPNType:
			Log(KBlogRPN);//
			break;
		case KFConType:
			//may need length byte value
			Log(KBlogFcOn);//
			break;
		case KFCoffType:
			Log(KBlogFcOff);//
			break;
		case KMSCType:
			signals = aSAP->Signals();//V.24 signals in MSC
			Log(KBlogMSC);//
			break;
		case KNSCType:
			Log(KBlogNSC);//
			break;
		case KRLSType:
			Log(KBlogRLS);//
		};

	}
void CRFBloggette::ExplainOutgoingFrame(CRfcommFrame* aFrm, CRfcommMuxer* aMux )
	{
#ifndef TCI
	//Show what type of frame we have sent and the various parts of the frame which are important.

	TInt frametype = aFrm->Type(); //Is it a Ctrl Frame, Data Frame, CreditDataFrame or Mux Ctrl Frame ?
	TUint8 ctrlfield = aFrm->Ctrl();
	TUint8 ctrl = ctrlfield&~KPollFinalBitmask; //tells whether SABM, DISC, UA, DM
	TUint8 addressfield = aFrm->Address(); //Contains EA, CR, DLCI
//	TUint8 dlci = aMux->DecodeDLCI(addressfield); //extracts dlci
	TBool EA = addressfield & KEABitmask; //Is the EA bit set? 
	TBool CR = addressfield & KCRBitmask; //Is the CR bit set?
	TBool poll = ctrlfield & KPollFinalBitmask; //Is the p/f bit set?
	
	switch(frametype)
		{
		case KCtrlFrameType:
			{
			//CRfcommCtrlFrame* ctrlfrm=static_cast<CRfcommCtrlFrame*>(aFrm);
			//TUint16 ctrlframelength = ctrlfrm->DataLength();
			Log(_L("Tx:"));
			
			if(ctrl==KSABMCtrlField)
				{
				Log(_L("Tx: SABM"));
				}
			if(ctrl==KUACtrlField)
				{
				Log(_L("Tx: UA"));
				}
			if(ctrl==KDMCtrlField)
				{
				Log(_L("Tx: DM"));
				}
			if(ctrl==KDISCCtrlField)
				{
				Log(_L("Tx: DISC"));
				}
			}
			break;
		case KDataFrameType: //CRfCommUIHFrame
			{
			CRfcommUIHFrame* uihfrm=static_cast<CRfcommUIHFrame*>(aFrm);
			TUint16 uihframelength = uihfrm->DataLength();

			if(poll)
				Log(_L("Tx: UIH credit data frame"));
			
			if (uihframelength<=127)
				{
				Log(_L("Tx: UIH simple data frame"));
				}
			else
				{
				Log(_L("Tx: UIH simple data frame"));
				}
			}
			break;
		case KCreditDataFrameType: //CRfcommCreditDataFrame
			{
			CRfcommCreditDataFrame* creditfrm=static_cast<CRfcommCreditDataFrame*>(aFrm);
			TUint8 credits = creditfrm->Credit();
			//BLOG Credits
			TUint16 length = creditfrm->DataLength();
			
			if (length<=127)
				{
				Log(KBlogShortCreditUIH, credits);
				}
			else
				{
				Log(KBlogLongCreditUIH, credits);
				}
			}
			break;
		case KMuxCtrlFrameType: //CRfcommMuxCtrlFrame containing muxer messages/commands
			{
			//#ifndef TCI
			CRfcommMuxCtrlFrame* muxfrm=static_cast<CRfcommMuxCtrlFrame*>(aFrm);
			TUint8 muxdlci = muxfrm->iDLCI;
			CRfcommSAP* sap = aMux->FindSAP(muxdlci); //Find the SAP that is on this dlci for aMux 
			TUint8 command = muxfrm->CommandType(); 
			LogMuxCommand(sap, aMux, command);
			//#endif
			}
			break;
		default: //CRfcommDataFrame
			break;
		};
	Log(KBlogPFSet, poll);
	Log(KBlogEASet, EA);
	Log(KBlogCRSet, CR);

	Log(KBlogFrameType, frametype);
#endif
	}

void CRFBloggette::ExplainIncomingFrame(TUint8 aDlci, TUint8 aCtrl, TBool aPoll)
	{
	Log(_L("Rx: "));
	switch (aCtrl)
		{
		case KSABMCtrlField:
			Log(_L("Rx: SABM"));
			break;
		case KUACtrlField:
			Log(_L("Rx: UA"));
			break;
		case KDMCtrlField:
			Log(KBlogDM);
			break;
		case KDISCCtrlField:
			Log(KBlogDISC);
			break;
		case KUIHCtrlField:
			{
			if(aDlci== KMuxDLCI)
				{
				Log(KBlogUIHCtrlFrame);
				}
			else
				{
				Log(KBlogSimpleUIH);
				}
			}
			break;			
		default:
			//FLOG(_L("Error: RFCOMM: Unexpected frame ctrl field"));
			break;
		};
	Log(KBlogDLCI, aDlci);
	Log(KBlogPFSet, aPoll);
	}

void CRFBloggette::IncomingCtrlMessage(TUint8 aCommand, TInt aLength)
	{
	switch(aCommand)
		{
		case KTestType:
			Log(KBlogTestCommand); 
			break;
		case KPNType:
			Log(KBlogPN);
			break;
		case KRPNType:
			Log(KBlogRPN);
			break;
		case KFConType:
			{
			Log(KBlogFcOn);
			if(aLength!=0)
				{
				Log(KBlogNonZeroLength);
				}
			}
			break;
		case KFCoffType:
			{
			Log(KBlogFcOff);
			if(aLength!=0)
				{
				Log(KBlogNonZeroLength);
				}
			}
			break;
		case KMSCType:
			Log(KBlogMSC);
			break;
		case KNSCType:
			Log(KBlogNSC);
			break;
		case KRLSType:
			Log(KBlogRLS);
		};
	}

#endif
