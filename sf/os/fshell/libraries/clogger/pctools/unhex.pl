#!perl
# unhex.pl
# 
# Copyright (c) 2010 Accenture. All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# 
# Initial Contributors:
# Accenture - Initial contribution
#

# Takes a standard hexdump of format:
# 6640000c: 55 00 39 00 54 cb 00 00 42 41 46 4c 20 2d 20 42 U.9.T...BAFL - B
# and outputs the concatenation of the ascii bits
#
# if -c is specified, it will further try to parse the hexdump as if it is a clogger debug router buffer

if (scalar(@ARGV) == 1 && $ARGV[0] eq "-h")
	{
	print << "ENDHELP";
Syntax: perl unhex.pl [-c]
Reads a hexdump from stdin, unhexes it and outputs to stdout.
If -c is specified, attempts to parse the hexdump as a clogger
RDebug::Print crashdumparea buffer.
ENDHELP
	exit 0;
	}
my $clogger = scalar(@ARGV) == 1 && $ARGV[0] eq "-c";

my $currentBuf = "";

while($line = <STDIN>)
	{
	if ($line =~ /.*: (.. .. .. .. .. .. .. .. .. .. .. .. .. .. .. ..)  ?(.*)/)
		{
		my $hex = $1;
		my $frag = "";
		my $strlen = length($hex);
		#print "strlen = $strlen\n";
		for (my $i = 0; $i < $strlen; $i+=3)
			{
			my $c = hex(substr($hex, $i, 2));
			#print ("c = $c\n");
			if (!$clogger)
				{
				# Then escape non-ascii
				if ($c != 13 && $c != 10 && ($c < 32 || $c > 127)) { $c = ord("."); }
				}
			$frag = $frag.chr($c);
			}
			
		if (!$clogger)
			{
			# Just print it
			print($frag);
			}
		else
			{
			if ($frag eq "\0\0\0\0\0\0\0\0" || $frag eq "\3\3\3\3\3\3\3\3")
				{
				# This indicates the buffer doesn't have any data in it. 0x03 is used by as-yet unwritten-to chunks, it would appear
				$frag = "";
				}
			$currentBuf .= $frag;
			my $len = length($currentBuf);
			my $start = index($currentBuf, "U\0");
			if ($start == -1) { $start = index($currentBuf, "K\0"); }
			if ($start == -1) { $start = index($currentBuf, "P\0"); }
			if ($start == -1 || $start + 8 > $len)
				{
				# No header in buf, or not all of it
				}
			else
				{
				my ($type, $entryLen, $timeStamp) = unpack("a1xvV", substr($currentBuf, $start));
				#print("type = $type, entryLen = $entryLen, timestamp = $timeStamp\n");
				
				if (8 + $start + $entryLen <= $len)
					{
					# Check if there was any junk in the buffer before the first header
					if ($start > 0)
						{
						my $s = substr($currentBuf, 0, $start);
						printf("00000000 $s\n");
						}
					$entry = substr($currentBuf, $start + 8, $entryLen);
					printf("%08x %s\n", $timeStamp, $entry);
					$currentBuf = substr($currentBuf, $start + 8 + $entryLen);
					}
				}
			}
		}
	}
if (length($currentBuf))
	{
	# The log is truncated, so just print what we have unparsed
	print("$currentBuf\n");
	}
