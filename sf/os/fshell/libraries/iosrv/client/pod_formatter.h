// pod_formatter.h
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __POD_FORMATTER_H__
#define __POD_FORMATTER_H__

#include <e32std.h>
#include "ioutils.h"

namespace IoUtils
	{

/**
 * Responsible for handling POD commands ('=head1', '=over', etc).
 */
class TPodFormatter
	{
public:
	TPodFormatter(CTextFormatter& aFormatter);
	void FormatL(const TDesC& aPod);
private:
	void OverL(TInt aIndent);
	void FormatToNextCommandL(TInt aIndent = 0);
	TPtrC NextCommand();
	TPtrC NextParagraph();
	TPtrC TextToNextCommand();
	void SkipToLineEnd();
	TInt SkipToNextLine();
	TInt SkipToNextCommand();
	TInt SkipToNextBlankLine();
	TInt SkipTo(const TDesC& aDes);
	TBool AtCommandStart();
private:
	CTextFormatter& iFormatter;
	TLex iPodLex;
	};

	}

#endif
