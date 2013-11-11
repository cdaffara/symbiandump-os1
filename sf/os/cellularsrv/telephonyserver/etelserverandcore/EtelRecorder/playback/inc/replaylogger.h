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
// Definitions of logging related replayer classes.
// 
//

/**
 @file
*/

#ifndef __REPLAYLOGGER_H__
#define __REPLAYLOGGER_H__

#include <f32file.h>

const TInt KBufSize = 2048;

class CLogger : public CBase
{
public:

		
public:
	static CLogger* NewL();
	void ConstructL();
	~CLogger();
	
 //   void WriteL(const TDesC8& aFmt,TBool aFormat = EFalse);
	void Write(TRefByValue<const TDesC8> aFmt,...); 

		

    
private:  
	RFs             iFs; 
	RFile            iFile;  
	TBuf8<KBufSize>  iFormatted;	
};

// TODO move macros to a separate file?

#include <comms-infras/commsdebugutility.h>

#include <e32cons.h>
//console logging


_LIT(KNewLine, "\n");

#define LOGLINE1(A)      							\
	{												\
	CDULOGLINE1(A);									\
	iConsole->Printf(A);							\
	iConsole->Printf(KNewLine);						\
	}

#define LOGLINE2(A, B)      						\
	{												\
	CDULOGLINE2(A, B);								\
	iConsole->Printf(A, B);							\
	iConsole->Printf(KNewLine);						\
	}
	
#define LOGLINE3(A, B, C)      						\
	{												\
	CDULOGLINE3(A, B, C);							\
	iConsole->Printf(A, B, C);						\
	iConsole->Printf(KNewLine);						\
	}
	
#define LOGLINE4(A, B, C, D)     					\
	{												\
	CDULOGLINE4(A, B, C, D);						\
	iConsole->Printf(A, B, C, D);					\
	iConsole->Printf(KNewLine);						\
	}

#define LOGLINE5(A, B, C, D, E)    					\
	{												\
	CDULOGLINE5(A, B, C, D, E);						\
	iConsole->Printf(A, B, C, D, E);				\
	iConsole->Printf(KNewLine);						\
	}
	
#define LOGLINE6(A, B, C, D, E, F)   				\
	{												\
	CDULOGLINE6(A, B, C, D, E, F);					\
	iConsole->Printf(A, B, C, D, E, F);				\
	iConsole->Printf(KNewLine);						\
	}
	
#define LOGLINE7(A, B, C, D, E, F, G)   			\
	{												\
	CDULOGLINE7(A, B, C, D, E, F, G);				\
	iConsole->Printf(A, B, C, D, E, F, G);			\
	iConsole->Printf(KNewLine);						\
	}
	
#define LOGLINE8(A, B, C, D, E, F, G, H)   			\
	{												\
	CDULOGLINE7(A, B, C, D, E, F, G, H);			\
	iConsole->Printf(A, B, C, D, E, F, G, H);		\
	iConsole->Printf(KNewLine);						\
	}
	
#ifdef _DEBUG

_LIT8(KReplaySubSystem, "ReplaySession");

_LIT8(KReplaySession, "Log");


#define CDULOGLINE1(A)	               __FLOG_STATIC0(KReplaySubSystem,KReplaySession,(A))
#define CDULOGLINE2(A,B)               __FLOG_STATIC1(KReplaySubSystem,KReplaySession,(A),(B))
#define CDULOGLINE3(A,B,C)             __FLOG_STATIC2(KReplaySubSystem,KReplaySession,(A),(B),(C))
#define CDULOGLINE4(A,B,C,D)           __FLOG_STATIC3(KReplaySubSystem,KReplaySession,(A),(B),(C),(D))
#define CDULOGLINE5(A,B,C,D,E)         __FLOG_STATIC4(KReplaySubSystem,KReplaySession,(A),(B),(C),(D),(E))
#define CDULOGLINE6(A,B,C,D,E,F)       __FLOG_STATIC5(KReplaySubSystem,KReplaySession,(A),(B),(C),(D),(E),(F))
#define CDULOGLINE7(A,B,C,D,E,F,G)     __FLOG_STATIC6(KReplaySubSystem,KReplaySession,(A),(B),(C),(D),(E),(F),(G))
#define CDULOGLINE8(A,B,C,D,E,F,G,H)   __FLOG_STATIC7(KReplaySubSystem,KReplaySession,(A),(B),(C),(D),(E),(F),(G),(H))

                              
#else  // _DEBUG

#define CDULOGLINE1(A)
#define CDULOGLINE2(A,B)
#define CDULOGLINE3(A,B,C)
#define CDULOGLINE4(A,B,C,D)
#define CDULOGLINE5(A,B,C,D,E)
#define CDULOGLINE6(A,B,C,D,E,F)
#define CDULOGLINE7(A,B,C,D,E,F,G)
#define CDULOGLINE8(A,B,C,D,E,F)

                                 
#endif	//_DEBUG

#endif
