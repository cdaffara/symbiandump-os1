# Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

#!/usr/bin/perl


use strict;

my %objectCoordinates =  # pretty dumb. could smarten this up a lot.
(
	"CSocket",					[200,100],
	"CSubConn",					[400,100],
	"CConn",					[600,100],
	"CCommsMgr",				[800,100],
	"CSelectionRequest",		[1000,110], # NOT Tier Manager's sub-session hence offset

	"CFlowFactCtr",				[100,150],
	"CSubConnFactCtr",			[300,150],
	"CConnFactCtr",				[500,150],
	"CMetaConnFactCtr",			[700,150],
	"CTierMgrFactCtr",			[900,150],


	"CIPDefaultSCPrFact",		[300,200],
	"CIPCPrFact",				[500,200],
	"CNetMCPrFact",				[700,200],
	"CNetTierMgrFact",			[900,200],

	"CIPDefaultSCPr",			[400,250],
	"CIPCPr",					[600,250],
	"CNetMCPr",					[800,250],
	"CNetTierMgr",				[1000,250],


	"CIPProtoSCPrFact",			[300,300],
	"CIPProtoCPrFact",			[500,300],
	"CIPProtoMCPrFact",			[700,300],
	"CIPProtoTierMgrFact",		[900,300],

	"CIPProtoDefaultSCPr",		[400,350],
	"CIPProtoCPr",				[600,350],
	"CIPProtoMCPr",				[800,350],
	"CIPProtoTierMgr",			[1000,350],


	"CIPProtoDefaultSCPr-1",	[400,400],
	"CIPProtoCPr-1",			[600,400],
	
	
	"CPPPSCPrFact",				[300,500],
	"CPPPCPrFact",				[500,500],
	"CPPPMCPrFact",				[700,500],
	"CPPPTierMgrFact",			[900,500],
	
	"CPPPSCPr",					[400,550],
	"CPPPCPr",					[600,550],
	"CPPPMCPr",					[800,550],
	"CPPPTierMgr",				[1000,550],
	
);

#
# constants
#
my $incrementY = 20;
my $topMargin = 20;
my $arrowWidth = 10;
my $textArrowSpacing = 1;
my $messageSpacingAboveLine = 1;
my $objectVerticalLineSpacing = 2;
my $objectUnderlineToLifelineGap = 2;

my $objectsPerRow = 4;
my $objectSpacesToSkipBeforeAutoSpacing = 4 * $objectsPerRow - 1; # start on 5th line
my $xSpacing = 200;
my $ySpacing = 100;

my $objectName = 0;
my $objectX = 1;
my $objectY = 2;
my $objectMsgCount = 3;
my $objectCreationPointKnown = 4;

my $sequenceAction = 0;
my $sequenceObjRef = 1;		# if action == "t" || "oc"
my $sequenceMsgRef = 1;		# if action != "t"
my $sequenceX = 2;
my $sequenceY = 3;

my $anims = 1;
my $time = 0;
my $timeDelta = 500;
my $timeDur = $timeDelta * 0.9;

my $colouredBoxes = 1;

my $animKeys = '1234567890qwertyuiopasdfghjklzxcvbnm';
#$animKeys = ''; # uncomment this line to enable animKeys
my $animMessagesPerKey = 5;
my $animKeyCounter = 0;
my $animKeysFirstElement=1;

my @sequences;
my @messages;
my @objects;

if(@ARGV && $ARGV[0] eq '-auto')
{
shift;
$animKeys='';
}

#
# Data Structures:
#
# "objects" is array of arrays:
#	[ <object name> <object X> <object Y> <object message count> <creation point known flag> ]
#
# "sequences" is an array of arrays:
#	[ "t" \objects[<n>] "text" ]
#	[ "[p|r]" \messages[n] \objects[<source>], \objects[<destination>] ]
#	[ "oc" \objects[<n>] ]


# Input file format:
#
#	<action> <arguments> ...
#
# specifically:
#
#	[P|R] <message name> <source object> <destination object>
#	T <object> <text>
#	OC <object>
#
# actions:
#
# P		Post
# R		Receive
# T		Text
# OC	Object Create
#
# All fields are space separated text.  For example:
#
# P StartFlow SCPR Flow
#

while (<>) {
	die unless s/^(\w+)\s+//;
	my $action = $1;
	if ($action eq "t")
		{
		# Text
		# t <object> <text>
		my $pos;
		if (s/^([\w\d-]+)\s+//) { $pos = $1; }
		my $objRef = addObject($pos);
		chomp;
		my $text = $_;
		push @sequences, [$action, $objRef, $text];
		}
	elsif ($action eq "oc")
		{
		# Object Create
		# oc <object>
		my $objName;
		if (s/^([\w\d-]+)\s+//) { $objName = $1; }
		my $objRef = addObject($objName);
		${$objRef}->[$objectCreationPointKnown] = 1;
		chomp;
		push @sequences, [$action, $objRef];
		}
	else
		{
		# Post/Receive
		# [P|R] <message> <source object> <destination object>
		split;
		my $msgRef = addMessage(shift @_);
		my $srcRef = addObject(shift @_);
		my $destRef = addObject(shift @_);
		${$srcRef}->[$objectMsgCount]++;
		${$destRef}->[$objectMsgCount]++;
		push @sequences, [$action, $msgRef, $srcRef, $destRef];
		}
	}


my ($screenWidth,$screenHeight) = calculateObjectPositions();

#my $screenWidth = $objects[$#objects]->[$objectX] + $rightMargin;
#my $screenHeight = scalar(@sequences) * ($incrementY + 2) + $topMargin;
#my $screenWidth = 500;
#my $screenHeight = 500;

outputDocHeader($screenWidth, $screenHeight);

drawObjectsAtTop();

	
drawCollaborations();

outputDocFooter();

####################
# Message routines
####################


sub drawObjectsAtTop()
	{
	my $i;
	foreach $i (@objects)
		{
		if ($i->[$objectCreationPointKnown] == 1)
			{
			next;
			}
		outputText($i->[$objectX], $i->[$objectName], $i->[$objectY], "middle", "underline","freeze",'',0);
		} 
		print "\n";
	}


sub addMessage()
	{
	my $obj = $_[0];
	my $i;
	for ($i = 0 ; $i < scalar(@messages) ; ++$i) {
		if ($messages[$i] eq $obj) {
			return \$messages[$i];
			}
		}
	$messages[$i] = $obj;
	return \$messages[$i];
	}

sub printMessages()
	{
	print "Messages (", scalar(@messages), ") : ";
	foreach my $msg (@messages) {
		print $msg, " ";
		}
	print "\n";
	}

###################
# Object routines
###################

sub addObject()
	{
	my $objName = $_[0];
	my $i;
	for ($i = 0 ; $i < scalar(@objects) ; ++$i) {
		if ($objects[$i]->[$objectName] eq $objName) {
			return \$objects[$i];
			}
		}
		$objects[$i] = [ $objName, 0, 0, 0, 0 ];
		return \$objects[$i];
	}

sub printObjects()
	{
	print "Objects (", scalar(@objects), "): ";
	foreach my $obj (@objects) {
		print $obj->[0], "(", $obj->[1], ") ";
		}
	print "\n";
	}

sub calculateObjectPositions()
	{
	my ($maxX, $maxY) = (0,0);
	
	my $xSpacing = 200;
	my $ySpacing = 100;

	my $objctr = $objectSpacesToSkipBeforeAutoSpacing;
	my $i;
	for ($i = 0 ; $i < scalar(@objects) ; ++$i)
		{
 		my $coords = $objectCoordinates{$objects[$i][$objectName]};
 		if(defined $coords)
 			{
			($objects[$i][$objectX],$objects[$i][$objectY]) =
		    	($coords->[0], $coords->[1]);
			}
		else
			{
	 		$objctr++;
#	for($objctr = 0 ; $objctr < 50 ; $objctr++)
#	{
 			my $x = (1+($objctr % $objectsPerRow)) * $xSpacing;
 			my $y = (1+int($objctr / $objectsPerRow)) * $ySpacing;
 #			print STDERR ("X $x Y $y\n");
 #	}		
 #	exit;
#my ($x,$y); 
			($objects[$i][$objectX],$objects[$i][$objectY]) =
		    	($x, $y);
			}

		if($objects[$i][$objectX] > $maxX) { $maxX = $objects[$i][$objectX];}
		if($objects[$i][$objectY] > $maxY) { $maxY = $objects[$i][$objectY];}
		
		$objects[$i][$objectX] -= $xSpacing/2;
		$objects[$i][$objectY] -= $ySpacing/2;
		
		print STDERR ("$i $objects[$i][$objectName] $objects[$i][$objectX] $objects[$i][$objectY]\n")
		}
#	die("$maxX $maxY");
	return ($maxX,$maxY);
	}

sub trimSilentObjects()
	{
	# get rid of objects that didn't end up with any messages sent to/from them
	my $i;
	for ($i = 0 ; $i < scalar(@objects) ; ) {
		if ($objects[$i][$objectMsgCount] == 0)
			{
			my $j;
			for ($j = 0 ; $j < scalar(@sequences) ; )
				{
				my $action = $sequences[$j][$sequenceAction];
				if ( (($action eq "t") || ($action eq "oc")) &&
				     ($sequences[$j][$sequenceObjRef] == \$objects[$i]) )
				    {
					splice @sequences, $j, 1;
#print "delete sequence $j\n";
#++$j;
					}
				else
					{
					++$j;
					}
				}
			splice @objects, $i, 1;
#print "delete object $i $objects[$i][$objectName]\n";
#++$i;
			}
		else
			{
			++$i;
			}
		}
	}

sub drawObject($$$)
	{
	my ($x,$y,$name) = @_;
	outputText($x, $name, $y, "middle", "underline","freeze",qq{opacity="0.9"},0);
#	outputVerticalLine($x, $y + $objectUnderlineToLifelineGap, $screenHeight - $topOffset);
	}
	


sub drawCollaborations()
	{
	foreach my $ref (@sequences) {
		my $action = $ref->[$sequenceAction];
		if ($action eq "t")
			{
			my $objX = ${$ref->[$sequenceObjRef]}->[$objectX];
			my $objY = ${$ref->[$sequenceObjRef]}->[$objectY] + $incrementY;
			my $text = $ref->[2];
print STDERR ("DRAWING $text $objX $objY\n");
			outputText($objX, $text, $objY, "middle", "", "freeze",qq{opacity="0.9"},0);
			$time += $timeDelta/2;
			}
		elsif ($action eq "oc") 
			{
#			if ($objectsDisplayedAtCreationPoint == 1)
				{
				my $objX = ${$ref->[$sequenceObjRef]}->[$objectX];
				my $objY = ${$ref->[$sequenceObjRef]}->[$objectY];
				my $objName = ${$ref->[$sequenceObjRef]}->[$objectName];
				drawObject($objX, $objY, $objName);
				}
			}
		else
			{
			my $msg = ${$ref->[$sequenceObjRef]};
			my $srcX = ${$ref->[$sequenceX]}->[$objectX];
			my $srcY = ${$ref->[$sequenceX]}->[$objectY];
			my $destX = ${$ref->[$sequenceY]}->[$objectX];
			my $destY = ${$ref->[$sequenceY]}->[$objectY];
			my $align;
			if ($action eq "p") {
				$align = "tail";
				}
			elsif ($action eq "r") {
				$align = "head";
				}
			outputLabelledLine($srcX, $srcY, $destX, $destY, $msg, $align);

			$time += $timeDelta;
			
			if($animKeys)
				{
				$animKeyCounter++;
				if($animKeyCounter % $animMessagesPerKey == 0)
					{
					$time = 0;
					$animKeysFirstElement=1;
					}
				}
				
			}
		print "\n";
		}
	}


#######################
# SVG output routines
#######################

sub outputDocHeader()
	{
	my ($width,$height) = @_;
	print '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>',"\n";
	print '<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.0//EN" "http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd">',"\n";
	print "<svg height=\"$height\" width=\"$width\" xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n";
	outputDefs();
	}

sub outputDefs()
	{
	print "<defs>\n";
	print qq{<marker id="arr" markerHeight="10" markerUnits="strokeWidth" markerWidth="10" orient="auto" refX="10" refY="5" viewBox="0 0 10 10">\n};
	print qq{\t<path d="M 0 0 L 10 5 L 0 10 " />\n};
	print "</marker>\n";
	print "</defs>\n";
	}

sub outputDocFooter()
	{
	print "</svg>\n";
	}

sub shortenLine($$$$$)
{
 	my($x1,$y1,$x2,$y2,$amount) = @_;

	my $xdiff = $x1-$x2;
	my $ydiff = $y1-$y2;
	my $linelen = sqrt(($xdiff*$xdiff) + ($ydiff*$ydiff));
	my $factor = 1 - ($amount / ($linelen+1));

 	if($x2>$x1)
 	{
 		$amount = (1-$factor) * ($x2-$x1);
 		$x1 += $amount;
 		$x2 -= $amount;
	}
	else
	{
		$amount = (1-$factor) * ($x1-$x2);
 		$x1 -= $amount;
 		$x2 += $amount;
	}
 	
 	if($y2>$y1)
 	{
 		$amount = (1-$factor) * ($y2-$y1);
 		$y1 += $amount;
 		$y2 -= $amount;
	}
	else
	{
		$amount = (1-$factor) * ($y1-$y2);
 		$y1 -= $amount;
 		$y2 += $amount;
	}
 	
 	return ($x1,$y1,$x2,$y2);
}

sub outputLabelledLine()
	{
	my ($x1,$y1,$x2,$y2,$text,$alignment) = @_;

#	my $scale = 0.9;
	($x1,$y1,$x2,$y2) = shortenLine($x1,$y1,$x2,$y2,15);
	outputLine($x1,$y1,$x2,$y2);
	
	my ($textx,$texty);
	my $anchor;
#	if (!$alignment || $alignment eq "mid") {
		$textx = (($x1 + $x2) / 2);
		$texty = (($y1 + $y2) / 2);
		$anchor = "middle";
#		}
#	else {
#		if ($alignment eq "head") {
#			if ($x1 < $x2) {
#				$anchor = "end";
#				$textx = $x2 - $arrowWidth - $textArrowSpacing;
#				$texty = $y2 - $arrowWidth - $textArrowSpacing;
#				}
#			else {
#				$anchor = "start";
#				$textx = $x2 + $arrowWidth + $textArrowSpacing;
#				$texty = $y2 + $arrowWidth + $textArrowSpacing;
#				}
#		} else {	# "tail"
#			$textx = $x1;
#			$texty = $y1;
#			if ($x1 < $x2) {
#				$anchor = "start";
#			} else {
#				$anchor = "end";
#				}
#			}
#		}

	my $rectangleColour = "black";

	if($colouredBoxes)
	{
		if($alignment eq "head")
		{
			$rectangleColour='pink';
		}
		else
		{
			$rectangleColour='lightgreen';
		}
	}
	
	outputText($textx,$text,$texty, $anchor, "", "remove", "", $rectangleColour);
	}

sub getAnimStr
{
	my ($animFill) = @_;
	
	my $beginStr = '';
	my $idStr = '';
	if($animKeys)
		{
		my $char = substr ($animKeys, int($animKeyCounter/$animMessagesPerKey) , 1);
		if($animKeysFirstElement) # if first in sequence
			{
			$animKeysFirstElement=0;
			$beginStr.=qq{accessKey($char)};
			$idStr = qq{ id="anim_$char" };
			}
		else
			{
			$beginStr = qq{anim_$char.begin};
			if($time) {$beginStr .= qq{ + $time}.'ms'}
			}
		}
	else
		{
		$beginStr = $time;
		$beginStr .= 'ms';
		}

	my $anim = qq{<set $idStr attributeName="visibility" attributeType="CSS" to="visible" begin="$beginStr" dur="$timeDur}.qq{ms" fill="$animFill" />};
	return $anim;
}

sub outputText()
	{
	my ($x,$text,$y,$anchor,$decoration,$animFill,$otherAttrs,$rectangleColour) = @_;

#$rectangleUnder=1;	
	my $attrs = qq{ x="$x" y="$y" $otherAttrs};
	
	if ($decoration)
		{
		$attrs .= qq{ text-decoration="$decoration" };
		}

	my $anim='';

	if ($anims)
		{
		$attrs .= qq{ visibility="hidden" };
		$anim = getAnimStr($animFill);
		}
	
	if ($anchor)
		{
		$attrs .= qq { text-anchor="$anchor" };
		}

	if($rectangleColour)
		{
		my $w = 8*length($text);  # guessing
		my $h = 18;  # guessing
		my $rx = $x - $w/2;		
		my $ry = $y - $h/2 - 4;
		print qq{<rect x="$rx" y="$ry" width="$w" height="$h" visibility="hidden" style="fill:$rectangleColour" opacity="0.95">$anim</rect>\n};
		}
	print "<text $attrs>$text$anim</text>\n";
	}

sub outputLine()
	{
	my ($x1,$y1,$x2,$y2) = @_;
	my $animTag='';
	my $animAttr='';
	if($anims)
		{
		$animAttr=qq{visibility="hidden"};
		$animTag= getAnimStr('remove');
		}
	print qq{<line stroke="black" marker-end="url(#arr)" $animAttr x1="$x1" y1="$y1" x2="$x2" y2="$y2">$animTag</line>\n};
	}

sub outputVerticalLine()
	{
	my ($x,$y1,$y2) = @_;
	print qq{<line stroke="black" x1="$x" y1="$y1" x2="$x" y2="$y2" />\n};
	}


 
