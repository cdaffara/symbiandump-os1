// SensibleFormat.h
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

// The purpose of this file is to define a generic format command that works reasonably even when there's no heap free
// Because it uses a stack buffer in this case, it is very difficult to make this a generic function without macroing

// Because I needed a 16-bit variant as well, and even templating would generate the same amount of code, I've done it
// as a #includable code snippet

// Note: If you do define SF_WIDE, Collapse() will be called on the result of the format before being passed to SF_ACTION

// If you want to pass in a VA_LIST, then #define SF_LIST aArgList
// If you want to pass in a bunch of arguments, #define SF_ARGLIST arg1,arg2,arg3

#if !defined(SF_FORMAT) || !defined(SF_BUF) || !defined(SF_ACTION)
#error "you must define all of SF_FORMAT, SF_BUF and SF_ACTION before including this file"
#endif

#if !((defined(SF_LIST) && !defined(SF_ARGLIST)) || (!defined(SF_LIST) && defined(SF_ARGLIST)))
#error "you must define exactly one of SF_LIST and SF_ARGLIST before including this file"
#endif

#ifndef COMMON_H
#error "you need to include common.h somewhere before including this file"
#endif

#ifdef SF_WIDE
#define TOverflowX TOverflow16
#define TBufX TBuf16<256>
#else
#define TOverflowX TOverflow8
#define TBufX TBuf8<256>
#endif

#ifdef SF_ARGLIST
#define SFAppend() AppendFormat(SF_FORMAT, &overflow, SF_ARGLIST)
#else
#define SFAppend() AppendFormatList(SF_FORMAT, SF_LIST, &overflow)
#endif

// Think of this as Format(RBuf8& SF_BUF, FunctionPointer SF_ACTION, const TDesCX& SF_FORMAT, TBool SF_STACKONLY, VA_LIST SF_LIST)
// where SF_ACTION(SF_BUF) is called passing in the fully formatted string
	{
	const TInt minBufSize = Max(SF_BUF.MaxSize(), 512);
	const TInt KMaxBufSize = 8192;
	TInt err = KErrNone;

#ifdef SF_STACKONLY
	if (SF_STACKONLY)
		{
		err = KErrNoMemory;
		}
	else
#endif
		{
		if (SF_BUF.MaxSize() == 0)
			{
			err = SF_BUF.Create(256);
			}
		else
			{
			SF_BUF.Zero();
			}
		}

	TOverflowX overflow;
	if (err == KErrNone)
		{
		// Do clever things to try and ensure that the format string won't be truncated, while at the same time not allocating an insanely big buffer
		for (TInt bufSize = minBufSize; bufSize <= KMaxBufSize; bufSize*=2)
			{
			#ifdef SF_WIDE
				TPtr16 widePtr((TUint16*)SF_BUF.Ptr(), SF_BUF.MaxSize()/2);
				widePtr.SFAppend();
			#else
				SF_BUF.SFAppend();
			#endif
			if (!overflow.iOverflow)
				{
				// Then the format went ok
				#ifdef SF_WIDE
					TPtr8 collapsed = widePtr.Collapse();
					SF_ACTION(collapsed);
				#else
					SF_ACTION(SF_BUF);
				#endif
				break;
				}
			else
				{
				// Overflowed

				if (bufSize == KMaxBufSize)
					{
					// We've grown up to our (self-imposed) max buf size, so just live with the truncation
					#ifdef SF_WIDE
						TPtr8 collapsed = widePtr.Collapse();
						SF_ACTION(collapsed);
					#else
						SF_ACTION(SF_BUF);
					#endif
					break;
					}

				// Try and grow
				SF_BUF.Close();
				err = SF_BUF.Create(bufSize*2);
				if (err) break;
				else continue;
				}
			}
		}
	if (err)
		{
		// Last resort, use 256 char buf on the stack
		TBufX stackBuf;
		stackBuf.SFAppend(); // Silently truncate if overflow happens now
		#ifdef SF_WIDE
			TPtr8 collapsed = stackBuf.Collapse();
			SF_ACTION(collapsed);
		#else
			SF_ACTION(stackBuf);
		#endif
		}
	}

#undef SF_WIDE
#undef SF_FORMAT
#undef SF_LIST
#undef SF_ARGLIST
#undef SF_BUF
#undef SF_ACTION
#undef TBufX
#undef TOverflowX
#undef SFAppend
#undef SF_STACKONLY
