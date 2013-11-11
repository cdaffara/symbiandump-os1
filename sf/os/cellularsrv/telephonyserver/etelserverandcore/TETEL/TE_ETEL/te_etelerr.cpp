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

#include "TE_EtelBase.h"
#include "te_etelerr.h"

#include <et_clsvr.h>
#include <e32std.h>
#include <etel.h>


// Derivation allows us access to part of protected interface.
class RTelServer1 : public RTelServer
	{
public : 
	TInt SendReceive(TInt aFunction,const TIpcArgs& aArgs) { return RTelServer::SendReceive(aFunction,aArgs); }	
	};

//
// GetTsyName
//


enum TVerdict CTestErrGetTsyOk::doTestStepL()
/** 
Check that RTelServer::GetTsyName and RTelServer::SendReceive( EEtelServerGetTsyName, ... ) 
	return no errors when they use a large enough valid buffer for returning the tsy name. 

@return EPass or EFail 
*/
	{

	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);
		
	TInt indexOfPhone=0;
	TName tsyName;
	ret = server.GetTsyName(indexOfPhone, tsyName);
	  // Check that the original still  works okay.
	TESTCHECK(ret, KErrNone);

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);	
	ServerClose(server);
	return TestStepResult();

	}


enum TVerdict CTestErrGetTsyIllegalBuffer::doTestStepL()
/** 
Check that RTelServer::SendReceive( EEtelServerGetTsyName, ... ) returns an error when a bad descriptor is passed in.

@return EPass or EFail 
*/
	{
	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	// Now lets pass in an illegal data buffer.
	// This is possible if a programmer derives from RTelServer
	// and makes a mistake, or does it on purpose for some perverse reason.

	TInt indexOfPhone=0;
	TInt tsyName;
	TIpcArgs args(indexOfPhone,&tsyName);
	ret = server.SendReceive(EEtelServerGetTsyName,args);

	TESTCHECK(ret, KErrBadDescriptor);

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);	
	ServerClose(server);
	return TestStepResult();
	}


enum TVerdict CTestErrGetTsySmallBuffer::doTestStepL()
/** 
Check that RTelServer::SendReceive( EEtelServerGetTsyName, ... ) returns an error when a descriptor is too small 
 for the tsy name.

@return EPass or EFail 
*/
	{
	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	// Now lets pass in a small data buffer.
	// This is possible if a programmer derives from RTelServer
	// and makes a mistake, or does it on purpose for some perverse reason.

	TInt indexOfPhone=0;
	TBuf<1> tsyName;
	TIpcArgs args(indexOfPhone,&tsyName);
	ret = server.SendReceive(EEtelServerGetTsyName,args);

	TESTCHECK(ret, KErrOverflow);

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);	
	ServerClose(server);
	return TestStepResult();
	}


//
// GetTsyVersionNumber
//

enum TVerdict CTestErrGetTsyVersionNumberOk::doTestStepL()
/** 
Check that RTelServer::GetTsyVersionNumber and RTelServer::SendReceive( EEtelServerGetTsyVersionNo, ... )
	return no errors when there is a large enough valid buffer to hold the version number.

@return EPass or EFail 
*/
	{
	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	// Check that GetTsyVersionNumber still runs okay.
	TInt indexOfPhone=0;
	TName tsyName;
	ret = server.GetTsyName(indexOfPhone, tsyName);	
		// Get a name for us to use.
	TESTCHECK(ret, KErrNone);

	TVersion version;
	ret = server.GetTsyVersionNumber(tsyName, version);
	TESTCHECK(ret, KErrNone);

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);	
	ServerClose(server);
	return TestStepResult();
	}


enum TVerdict CTestErrGetTsyVersionNumberIllegalBuffer::doTestStepL()
/** 
Check that RTelServer::SendReceive( EEtelServerGetTsyVersionNo, ... ) returns an error when a bad buffer is passed in.
@SYMTestPriority High

@return EPass or EFail 
*/
	{
	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	// Now lets pass in an illegal data buffer.
	// This is possible if a programmer derives from RTelServer
	// and makes a mistake, or does it on purpose for some perverse reason.

	TInt indexOfPhone=0;
	TName tsyName;
	ret = server.GetTsyName(indexOfPhone, tsyName);	
		// Get a name for us to use.
	TESTCHECK(ret, KErrNone);

	TInt version=0;

	TIpcArgs args(&tsyName, version);
	ret = server.SendReceive(EEtelServerGetTsyVersionNo,args);
		// Note only possible of error if we inherit and mess around with calls.
		// Otherwise goes through API, which is more safe, but even then we can
		// do some casting.

	TESTCHECK(ret, KErrBadDescriptor);

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);	
	ServerClose(server);
	return TestStepResult();
	}


enum TVerdict CTestErrGetTsyVersionNumberSmallBuffer::doTestStepL()
/** 
Check that RTelServer::SendReceive( EEtelServerGetTsyVersionNo, ... ) returns an error when a descriptor is too small 
 for the version info.

@return EPass or EFail 
*/
	{
	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	// Now lets pass in a small data buffer 
	// This is possible if a programmer derives from RTelServer
	// and makes a mistake, or does it on purpose for some perverse reason.

	TInt indexOfPhone=0;
	TName tsyName;
	ret = server.GetTsyName(indexOfPhone, tsyName);	
		// Get a name for us to use.
	TESTCHECK(ret, KErrNone);

	TBuf8<1> version;

	TPtr8 ptr(REINTERPRET_CAST(TText8*,&version),version.Length(),version.MaxLength() );
	TIpcArgs args(&tsyName, &ptr);
	ret = server.SendReceive(EEtelServerGetTsyVersionNo,args);
		// Note only possible of error if we inherit and mess around with calls.
		// Otherwise goes through API, which is more safe, but even then we can
		// do some casting.

	TESTCHECK(ret, KErrOverflow);

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);	
	ServerClose(server);
	return TestStepResult();
	}


//
// GetPhoneInfo
//

enum TVerdict CTestErrGetPhoneInfoOk::doTestStepL()
/** 
Check that RTelServer::GetPhoneInfo and RTelServer::SendReceive( EEtelServerPhoneInfoByIndex, ... )
 return no errors when there is a large enough valid buffer to hold the phone info.

@return EPass or EFail 
*/
	{
	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	TInt index=0;
	RTelServer::TPhoneInfo phoneInfo;

	ret = server.GetPhoneInfo(index,phoneInfo);
	TESTCHECK(ret, KErrNone);

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);	
	ServerClose(server);
	return TestStepResult();
	}


enum TVerdict CTestErrGetPhoneInfoIllegalBuffer::doTestStepL()
/** 
Check that RTelServer::SendReceive( EEtelServerPhoneInfoByIndex, ... ) returns an error when a bad buffer is passed in. 

@return EPass or EFail 
*/
	{
	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	// Now lets pass in a small data buffer 
	// This is possible if a programmer derives from RTelServer
	// and makes a mistake, or does it on purpose for some perverse reason.
	// Normally goes through server API.

	TInt index=0;
	TInt result=0;

	TIpcArgs args(&result,index);
	ret = server.SendReceive(EEtelServerPhoneInfoByIndex,args);

	TESTCHECK(ret, KErrBadDescriptor);

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);	
	ServerClose(server);
	return TestStepResult();
	}


enum TVerdict CTestErrGetPhoneInfoSmallBuffer::doTestStepL()
/** 
Check that RTelServer::SendReceive( EEtelServerPhoneInfoByIndex, ... ) returns an error when a descriptor is too small 
 for the phone info.

@return EPass or EFail 
*/
	{
	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	// Now lets pass in a small data buffer 
	// This is possible if a programmer derives from RTelServer
	// and makes a mistake, or does it on purpose for some perverse reason.
	// Normally goes through server API.

	TInt index=0;
	TBuf8<1> phoneInfo;

	TPckg< TBuf8<1> > result(phoneInfo);

	TIpcArgs args(&result,index);
	ret = server.SendReceive(EEtelServerPhoneInfoByIndex,args);

	TESTCHECK(ret, KErrOverflow);

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);	
	ServerClose(server);
	return TestStepResult();
	}


//
// IsSupportedByModule
//

enum TVerdict CTestErrIsSupportedByModuleOk::doTestStepL()
/** 
Check that RTelServer::IsSupportedByModuleo and RTelServer::SendReceive( EEtelServerQueryTsyFunctionality, ... )
 return no errors when there is a large enough valid buffer to hold the result.

@return EPass or EFail 
*/
	{
	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	// Check IsSupportedByModule is ok,
	TInt indexOfPhone=0;
	TName tsyName;
	ret = server.GetTsyName(indexOfPhone, tsyName);	
		// Get a name for us to use.
	TESTCHECK(ret, KErrNone);

	TInt mixin = 0;
	TBool result = 0;
	ret = server.IsSupportedByModule(tsyName, mixin, result);

	TESTCHECK(ret, KErrNone);

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);	
	ServerClose(server);
	return TestStepResult();
	}


enum TVerdict CTestErrIsSupportedByModuleRetIllegalBuffer::doTestStepL()
/** 
Check that RTelServer::SendReceive( EEtelServerQueryTsyFunctionality, ... ) returns an error when a bad return buffer is passed in.

@return EPass or EFail 
*/
	{
	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	TInt indexOfPhone=0;
	TName tsyName;
	ret = server.GetTsyName(indexOfPhone, tsyName);	
		// Get a name for us to use.
	TESTCHECK(ret, KErrNone);

	TInt mixin = 0;
	char result = 0;
	TPtr8 ptr1(REINTERPRET_CAST(TText8*,&mixin),sizeof(mixin),sizeof(mixin));
	TIpcArgs args(&tsyName,&ptr1,&result);
	ret = server.SendReceive(EEtelServerQueryTsyFunctionality,args);

	TESTCHECK(ret, KErrBadDescriptor);

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);	
	ServerClose(server);
	return TestStepResult();
	}

enum TVerdict CTestErrIsSupportedByModuleRetSmallBuffer::doTestStepL()
/** 
Check that RTelServer::SendReceive( EEtelServerQueryTsyFunctionality, ... ) returns an error when a bad return buffer is to small.

@return EPass or EFail 
*/
	{
	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	TInt indexOfPhone=0;
	TName tsyName;
	ret = server.GetTsyName(indexOfPhone, tsyName);	
		// Get a name for us to use.
	TESTCHECK(ret, KErrNone);

	TInt mixin = 0;
	char result = 0;
	TPtr8 ptr1(REINTERPRET_CAST(TText8*,&mixin),sizeof(mixin),sizeof(mixin));
	TPtr8 ptr2(REINTERPRET_CAST(TText8*,&result),sizeof(result),sizeof(result));
	TIpcArgs args(&tsyName,&ptr1,&ptr2);
	ret = server.SendReceive(EEtelServerQueryTsyFunctionality,args);

	TESTCHECK(ret, KErrOverflow);

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);	
	ServerClose(server);
	return TestStepResult();
	}


enum TVerdict CTestErrIsSupportedByModuleIllegalBuffer::doTestStepL()
/** 
Check that RTelServer::SendReceive( EEtelServerQueryTsyFunctionality, ... ) returns an error when a bad buffer is passed in. 

@return EPass or EFail 
*/
	{
	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	TInt indexOfPhone=0;
	TName tsyName;
	ret = server.GetTsyName(indexOfPhone, tsyName);	
		// Get a name for us to use.
	TESTCHECK(ret, KErrNone);

	char mixin = 0;
	TBool result = 0;
	TPtr8 ptr2(REINTERPRET_CAST(TText8*,&result),sizeof(result),sizeof(result));
	TIpcArgs args(&tsyName,&mixin,&ptr2);
	ret = server.SendReceive(EEtelServerQueryTsyFunctionality,args);

	TESTCHECK(ret, KErrBadDescriptor);

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);	
	ServerClose(server);
	return TestStepResult();
	}


enum TVerdict CTestErrIsSupportedByModuleSmallBuffer::doTestStepL()
/** 
Check that RTelServer::SendReceive( EEtelServerQueryTsyFunctionality, ... ) returns an error when a small buffer is passed in. 

@return EPass or EFail 
*/
	{
	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	TInt indexOfPhone=0;
	TName tsyName;
	ret = server.GetTsyName(indexOfPhone, tsyName);	
		// Get a name for us to use.
	TESTCHECK(ret, KErrNone);

	char mixin = 0;
	TBool result = 0;
	TPtr8 ptr1(REINTERPRET_CAST(TText8*,&mixin),sizeof(mixin),sizeof(mixin));
	TPtr8 ptr2(REINTERPRET_CAST(TText8*,&result),sizeof(result),sizeof(result));
	TIpcArgs args(&tsyName,&ptr1,&ptr2);
	ret = server.SendReceive(EEtelServerQueryTsyFunctionality,args);

	TESTCHECK(ret, KErrOverflow);

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);	
	ServerClose(server);
	return TestStepResult();
	}


//
// SetExtendedErrorGranularity
//

enum TVerdict CTestErrSetExtendedErrorGranularityOk::doTestStepL()
/** 
Check that RTelServer::SetExtendedErrorGranularity and RTelServer::SendReceive( EEtelServerSetExtendedErrorGranularity, ... )
 return no errors (or panics) when there is a large enough input buffer to hold the granularity settings.

@return EPass or EFail 
*/
	{
	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	// Check works okay after modification.
	RTelServer::TErrorGranularity granularity = RTelServer::EErrorExtended;
	ret = server.SetExtendedErrorGranularity(granularity);
	TESTCHECK(ret, KErrNone);

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);	
	ServerClose(server);
	return TestStepResult();
	}


enum TVerdict CTestErrSetExtendedErrorGranularityPanic::doTestStepL()
/** 
Check that RTelServer::SendReceive( EEtelServerSetExtendedErrorGranularity, ... ) panics the client when the wrong size buffer is passed in.

@return EPass or EFail 
*/
	{
	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	// Send Fewer bytes required for enum TErrorGranularity. 
	RTelServer::TErrorGranularity granularity = RTelServer::EErrorExtended;
	TPtr8 ptr(REINTERPRET_CAST(TText8*,CONST_CAST(RTelServer::TErrorGranularity*,&granularity)),
		sizeof(RTelServer::TErrorGranularity)-1,sizeof(RTelServer::TErrorGranularity));
		// Do not have all the bytes required to make the enum.

	TIpcArgs args(&ptr);
	ret = server.SendReceive(EEtelServerSetExtendedErrorGranularity, args);
			// This will panic and will not continue.

	ServerClose(server);
	return TestStepResult();
	}



enum TVerdict CTestErrNewTelObjectOk::doTestStepL()
/** 
Check that RTelServer::OpenNewCall returns no errors 
 when it has a large enough valid buffer to put the call name in. 

@return EPass or EFail 
*/
	{
	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	RTelServer::TPhoneInfo phoneInfo;

	ret = server.GetPhoneInfo( 0, phoneInfo );
	TESTCHECK(ret, KErrNone);

	RPhone phone;
	ret = phone.Open( server, phoneInfo.iName );
	TESTCHECK(ret, KErrNone);


	RPhone::TLineInfo lineInfo;
	ret = phone.GetLineInfo( 0, lineInfo );
	TESTCHECK(ret, KErrNone);


	RLine line;
	ret = line.Open( phone, lineInfo.iName );

	TName retName;

	// Check that we can get a name for the newly opened call. 

	RCall call;
	ret = call.OpenNewCall(line , retName); 
	TESTCHECK(ret, KErrNone);

	call.Close();
	line.Close();
	phone.Close();

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);	
	ServerClose(server);

	return TestStepResult();
	}



// Derivation allows us access to part of protected interface.
class RCall1 : public RCall
	{
public: 
	void ConstructL() { RCall::ConstructL(); }
	void Destruct() { RCall::Destruct(); }
	TInt CreateSubSession(const RSessionBase& aSession,TInt aFunction,const TIpcArgs& aArgs) { return RCall::CreateSubSession(aSession,aFunction,aArgs); }
	};

// Derivation allows us access to part of protected interface.
class RSessionBase1 : public RSessionBase
	{	
public:
	TInt SendReceive(TInt aFunction,const TIpcArgs& aArgs) const { return RSessionBase::SendReceive(aFunction,aArgs); }
	};


enum TVerdict CTestErrNewTelObjectIllegalBuffer::doTestStepL()
/** 
Check that CTelSession::NewTelObject fails when an illegal buffer is passed to it for the name of the call.
 (Uses similar code to that in RTelServer::OpenNewCall().)

@return EPass or EFail 
*/
	{
	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	RTelServer::TPhoneInfo phoneInfo;

	ret = server.GetPhoneInfo( 0, phoneInfo );
	TESTCHECK(ret, KErrNone);

	RPhone phone;
	ret = phone.Open( server, phoneInfo.iName );
	TESTCHECK(ret, KErrNone);


	RPhone::TLineInfo lineInfo;
	ret = phone.GetLineInfo( 0, lineInfo );
	TESTCHECK(ret, KErrNone);


	RLine line;
	ret = line.Open( phone, lineInfo.iName );

	char retName;  
		// Illegal buffer.

	RCall1 call;

	// Taken from OpenNewCall(line , retName); 
	RSessionBase* session=&line.SessionHandle();
	call.ConstructL();
	TInt subSessionHandle=line.SubSessionHandle();

	TPtrC name(_L("::"));	// necessary so that server knows to ask TSY for new name

	TIpcArgs args(&name,REINTERPRET_CAST(TDes*,&retName),subSessionHandle);
	call.SetSessionHandle(*session);
	ret = call.CreateSubSession(*session,EEtelOpenFromSubSession,args);
	call.Destruct();

	TESTCHECK(ret, KErrBadDescriptor);

	line.Close();
	phone.Close();

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);	
	ServerClose(server);
	return TestStepResult();
	}


enum TVerdict CTestErrNewTelObjectSmallBuffer::doTestStepL()
/** 
Check that CTelSession::NewTelObject fails when a small buffer is passed to it for the name of the call.
 (Uses similar code to that in RTelServer::OpenNewCall().)

@return EPass or EFail 
*/
	{
	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	RTelServer::TPhoneInfo phoneInfo;

	ret = server.GetPhoneInfo( 0, phoneInfo );
	TESTCHECK(ret, KErrNone);

	RPhone phone;
	ret = phone.Open( server, phoneInfo.iName );
	TESTCHECK(ret, KErrNone);


	RPhone::TLineInfo lineInfo;
	ret = phone.GetLineInfo( 0, lineInfo );
	TESTCHECK(ret, KErrNone);


	RLine line;
	ret = line.Open( phone, lineInfo.iName );

	TBuf<1> retName;
		//Buffer too small to recieve name.

	RCall call;
	ret = call.OpenNewCall(line , retName); 

	TESTCHECK(ret, KErrOverflow);

	line.Close();
	phone.Close();

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);	
	ServerClose(server);
	return TestStepResult();
	}


enum TVerdict CTestErrNewTelObjectHandleIllegalBuffer::doTestStepL()
/** 
Check that CTelSession::NewTelObject fails when an illegal buffer is passed to it for the name of the call.
 (Uses similar code to that in RTelServer::OpenNewCall().)

@return EPass or EFail 
*/
	{
	RTelServer1 server;
	TInt ret = server.Connect();
	TESTCHECK(ret, KErrNone);
	ret=server.LoadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);

	RTelServer::TPhoneInfo phoneInfo;

	ret = server.GetPhoneInfo( 0, phoneInfo );
	TESTCHECK(ret, KErrNone);

	RPhone phone;
	ret = phone.Open( server, phoneInfo.iName );
	TESTCHECK(ret, KErrNone);


	RPhone::TLineInfo lineInfo;
	ret = phone.GetLineInfo( 0, lineInfo );
	TESTCHECK(ret, KErrNone);


	RLine line;
	ret = line.Open( phone, lineInfo.iName );

	TName retName;

	RCall1 call;

	// Below taken from OpenNewCall(line , retName); 
	RSessionBase* session=&line.SessionHandle();
	call.ConstructL();
	TInt subSessionHandle=line.SubSessionHandle();
		// This is wrong size and type.

	TPtrC name(_L("::"));	// necessary so that server knows to ask TSY for new name

	TIpcArgs args(&name,REINTERPRET_CAST(TDes*,&retName),subSessionHandle);
	call.SetSessionHandle(*session);

	//Below taken from ret = call.CreateSubSession(*session,EEtelOpenFromSubSession,args);

	TIpcArgs a;
	a.iArgs[0] = args.iArgs[0];
	a.iArgs[1] = args.iArgs[1];
	a.iArgs[2] = args.iArgs[2];
	a.iFlags = args.iFlags;

	char replyValue=0xff;
		// Bad buffer.

	args.Set(3,&replyValue);
	ret=( (RSessionBase1*) (session) )->SendReceive(EEtelOpenFromSubSession,args);

	call.Destruct();

	TESTCHECK(ret, KErrBadDescriptor);

	line.Close();
	phone.Close();

	ret=server.UnloadPhoneModule(DSTD_MODULE_NAME);
	TESTCHECK(ret, KErrNone);	
	ServerClose(server);
	return TestStepResult();
	}

