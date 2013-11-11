# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Script to validate the unit links in a system definition or package definition XML file
#!/usr/bin/perl

use strict;


use FindBin;		# for FindBin::Bin
use lib $FindBin::Bin;
use lib "$FindBin::Bin/lib";

use Cwd;
use Cwd 'abs_path';
use Getopt::Long;
use File::Basename;
use File::Spec;
use XML::DOM;

my $output;
my $path;
my %defineParams;
my %defines;
my $defaultns = 'http://www.symbian.org/system-definition';	# needed if no DTD
my $realloc;

# need to add options for controlling which metas are filtered out and which are included inline
GetOptions
	(
	'path=s' => $path,
	 'effective-sysdef=s' => \$realloc
	);

# -path specifies the full system-model path to the file which is being processed. 
#	This must be an absolute path if you're processing a root sysdef.
#	If processing a pkgdef file, you can use "./package_definition.xml" to leave all links relative. Though I can't really see the use case for this.


# if config is not set, no confguration will be done.
# If it is set, all configuration metadata will be processed and stripped from the output, even if the confguration data is empty

 if($path eq '') {$path = '/os/deviceplatformrelease/foundation_system/system_model/system_definition.xml'}

($#ARGV == -1 ) && &help();
my $sysdef = &abspath(shift);	# resolve the location of the root sysdef

$realloc = $realloc || $sysdef;

my %unitmap;
my @p1=reverse(split(/[\\\/]/,$path));
my @p2=reverse(split(/[\\\/]/,$realloc));

shift(@p1);shift(@p2); # don't care abt file name
while(lc($p1[0]) eq lc($p2[0])) {shift(@p1);shift(@p2)}

$unitmap{join('/',reverse(@p1))} = join("/",reverse(@p2));

my @p1=reverse(split(/[\\\/]/,$sysdef));
my @p2=reverse(split(/[\\\/]/,$realloc));

shift(@p1);shift(@p2); # don't care abt file name
while(lc($p1[0]) eq lc($p2[0])) {shift(@p1);shift(@p2)}

$unitmap{join('/',reverse(@p1))} = join("/",reverse(@p2));


# rootmap is a mapping from the filesystem to the paths in the doc
my %rootmap = &rootMap($path,$sysdef);	
my %nsmap;
my %urimap;

my $parser = new XML::DOM::Parser;
my   $sysdefdoc = $parser->parsefile ($sysdef);


my $maxschema = $sysdefdoc->getDocumentElement()->getAttribute('schema');	# don't check value, just store it.

my $docroot =  $sysdefdoc->getDocumentElement;

my $ns = $docroot->getAttribute('id-namespace');
if(!$ns && $nsmap{''})
	{
	$docroot->setAttribute('id-namespace',$nsmap{''});
	}

$docroot->setAttribute('schema',$maxschema);	# output has the largest syntax version of all includes


while(my($pre,$uri) = each(%nsmap))
	{
	$pre ne '' || next ;
	$docroot->setAttribute("xmlns:$pre",$uri);
	}

&walk($sysdef,$docroot);	# process the XML

 
sub abspath
	{ 	# normalize the path into an absolute one
	my  ($name,$path) = fileparse($_[0]);
	$path=~tr,\\,/,;
	if( -e $path)
		{
		return abs_path($path)."/$name";
		}
	my @dir = split('/',$_[0]);
	my @new;
	foreach my $d (@dir)
		{
		if($d eq '.') {next}
		if($d eq '..')
			{
			pop(@new);
			next;
			}
		push(@new,$d)
		}
	return join('/',@new);
	}

sub rootMap {
	my @pathdirs = split(/\//,$_[0]);
	my @rootdirs = split(/\//,$_[1]);

	while(lc($rootdirs[$#rootdirs])  eq lc($pathdirs[$#pathdirs])  )
		{
		pop(@rootdirs);
		pop(@pathdirs);
		}
	return (join('/',@rootdirs)  => join('/',@pathdirs) );
	}

sub rootMapMeta {
	# find all the explict path mapping from the link-mapping metadata
	my $node = shift;
	foreach my $child (@{$node->getChildNodes})
			{
			if ($child->getNodeType==1 && $child->getTagName eq 'map-prefix')
				{
				my $from = $child->getAttribute('link');
				my $to = $child->getAttribute('to');		# optional, but blank if not set
				$rootmap{$from} = $to;
				}
			}
	# once this is processed we have no more need for it. Remove from output
	$node->getParentNode->removeChild($node);
	}


sub walk
	{ 	# walk through the doc, resolving all links
	my $file = shift;
	my $node = shift;
	my $type = $node->getNodeType;
	if($type!=1) {return}
	my $tag = $node->getTagName;
	if($tag=~/^(layer|package|collection|component)$/ )
		{
		my $link= $node->getAttribute('href');
		if($link)
			{
			my $file = &resolvePath($file,$link); 
			if(-e $file)
				{
				&combineLink($node,$file);
				}
			else
				{
				print  "Note: $file not found\n";
				$node->removeAttribute('href');
				}
			return;
			}
		}
	elsif($tag=~/^(SystemDefinition|systemModel)$/ )
		{
		}
	elsif($tag eq 'unit')
		{
		my %at = &atts($node);
		my $pro;
		foreach my $o (keys(%at)) 
			{
			if($o eq 'proFile' || $o=~/:proFile$/)
				{
				$pro = $at{$o};
				last;
				}
			}
		my $filter=$node->getParentNode()->getAttribute('filter');
		if($filter ne '' && $at{'filter'}) {$filter.=','.$at{'filter'}}
		elsif($at{'filter'}) {$filter=$at{'filter'}}
		if($filter ne '') {$filter="\t($filter)"}
		foreach my $atr ('bldFile','mrp','base')
			{
			my $ext;
			my $link= $at{$atr};
			if($atr eq 'bldFile') {
				$ext = ($pro ne '') ? "/$pro" : '/bld.inf'
			}
			if($link ne '')
				{
				my $ok = 0;
				my $trylink;
				if($link && !($link=~/^\//))
					{
					$link= &abspath(File::Basename::dirname($file)."/$link");
					$ok = (-e "$link$ext");
					if(!$ok)	
						{
						foreach my $a (keys %rootmap)
							{
							$link=~s,^$a,$rootmap{$a},ie;
							# remove leading ./  which is used to indicate that paths should remain relative
							$link=~s,^\./([^/]),$1,; 
							}

						}
					}
				if(!$ok)
					{
					foreach my $a (keys %unitmap) {
						if($a eq substr($link,0,length($a))) {
							$trylink = $unitmap{$a}.substr($link,length($a));
							if(-e "$trylink$ext") {
								$ok=1;
								$link = $trylink;
								last;
							}
						}
					}
					}
				if(!$ok)
					{
					print "Error: $atr not found in ",($trylink ne '') ? $trylink : $link,"$filter\n";
					}				
				}
			}
		}
	elsif($tag eq 'meta')
		{
		my $rel= $node->getAttribute('rel') || 'Generic';
		my $link= $node->getAttribute('href');
		$link=~s,^file://(/([a-z]:/))?,$2,; # convert file URI to absolute path
		if ($link ne '' ) 
			{ 
			if($link=~/^[\/]+:/)
				{
				print  "Note: Remote URL $link not validated\n";
				next; # do not alter children
				}
			if(! ($link=~/^\//))
				{
				$link= &abspath(File::Basename::dirname($file)."/$link");
				}
			if(! -e $link) 
				{
				if(! -e &realPath($link)) {
					print  "Warning: Local metadata file not found: $link\n";
				}
				next; # do not alter children
				}
			}
		if($node->getAttribute('rel') eq 'link-mapping')
			{# need to process this now
			&rootMapMeta($node);
			}
		return;
		}
	else {return}
	my $checkversion=0;
	foreach my $item (@{$node->getChildNodes})
		{
		#print $item->getNodeType,"\n";
		&walk($file,$item);
		}



	}


sub realPath
	{
	my $link = shift;
	foreach my $a (keys %unitmap)
		{
		if($a eq substr($link,0,length($a))) 
			{
			my $trylink = $unitmap{$a}.substr($link,length($a));
			if(-e $trylink) {return $trylink}
			}
		}
	}

sub combineLink
	{
	# combine data from linked sysdef fragment w/ equivalent element in parent document
	my $node = shift;
	my $file = shift;
	my $getfromfile = &localfile($file);
	$getfromfile eq '' && return;  # already raised warning, no need to repeat
	my  $doc = $parser->parsefile ($getfromfile);
	my $item =&firstElement($doc->getDocumentElement);
	$item || die "badly formatted $file";	
	&fixIDs($item);
	my %up = &atts($node);
	my %down = &atts($item);
	$up{'id'} eq $down{'id'}  || die "$up{id} differs from $down{id}";
	$node->removeAttribute('href');
	foreach my $v (keys %up) {delete $down{$v}}
	foreach my $v (keys %down)
		{
		$node->setAttribute($v,$down{$v})
		}
	foreach my $child (@{$item->getChildNodes})
		{
		&copyInto($node,$child);
		}
	&walk($file,$node);
	}


sub copyInto
	{
	# make a deep copy the node (2nd arg) into the element (1st arg)
	my $parent=shift;
	my $item = shift;
	my $doc = $parent->getOwnerDocument;
	my $type = $item->getNodeType;
	my $new;
	if($type==1) 
		{
		&fixIDs($item);
		$new = $doc->createElement($item->getTagName);
		my %down = &atts($item);
		foreach my $ordered ('id','name','bldFile','mrp','level','levels','introduced','deprecated','filter')
			{
			if($down{$ordered})
				{
				$new->setAttribute($ordered,$down{$ordered});
				delete $down{$ordered}
				}
			}
		while(my($a,$b) = each(%down))
			{
			$new->setAttribute($a,$b);
			}
		foreach my $child (@{$item->getChildNodes})
			{
			&copyInto($new,$child);
			}
		}
	elsif($type==3) 
		{
		$new = $doc->createTextNode ($item->getData);
		}
	elsif($type==8) 
		{
		$new = $doc->createComment  ($item->getData);
		}
	if($new)
		{
		$parent->appendChild($new);
		}
	}

sub getNs
	{
	# find the namespace URI that applies to the specified prefix.
	my $node = shift;
	my $pre = shift;
	my $uri = $node->getAttribute("xmlns:$pre");
	if($uri) {return $uri}
	my $parent = $node->getParentNode;
	if($parent && $parent->getNodeType==1)
		{
		return getNs($parent,$pre);
		}
	}


sub fixIDs
	{
	# translate the ID to use the root doc's namespaces 
	my $node = shift;
	foreach my $id ('id','before')
		{
		&fixID($node,$id);
		}
}

sub fixID
	{
	# translate the ID to use the root doc's namespaces 
	my $node = shift;
	my $attr = shift || 'id';
	my $id = $node->getAttribute($attr);
	if($id eq '') {return}
	my $ns;
	if($id=~s/^(.*)://)
		{ # it's got a ns, find out what it is
		my $pre = $1;
		$ns=&getNs($node,$pre);
		}
	else
		{
		$ns = $node->getOwnerDocument->getDocumentElement->getAttribute("id-namespace") ||
			$defaultns;
		}
	$ns = $urimap{$ns};
	$id = ($ns eq '') ? $id : "$ns:$id";
	return $node->setAttribute($attr,$id);
}

sub firstElement {
	# return the first element in this node
	my $node = shift;
	foreach my $item (@{$node->getChildNodes}) {
		if($item->getNodeType==1) {return $item}
	}
}


sub atts {
	# return a hash of all attribtues defined for this element
	my $node = shift;
	my %at = $node->getAttributes;
	my %list;
	foreach my $a (keys %{$node->getAttributes}) 
		{
		if($a ne '')
			{
			$list{$a} = $node->getAttribute ($a);
			}
		}
	return %list;
}


sub ns 
	{
	# return a hash of ns prefix and uri -- the xmlns: part is stripped off
	my $node = shift;
	my %list;
	foreach my $a (keys %{$node->getAttributes}) 
		{
		my $pre = $a;
		if($pre=~s/^xmlns://)
			{
			$list{$pre} = $node->getAttribute ($a);
			}
		}
	return %list;
	}


sub resolvePath
	{
	# return full path to 2nd arg relative to first (path or absolute URI)
	my $base = shift;
	my $path = shift;
	if($path=~m,^/,) {return $path } # path is absolute, but has no drive. Let OS deal with it.
	if($path=~s,^file:///([a-zA-Z]:/),$1,) {return $path } # file URI with drive letter
	if($path=~m,^file://,) {return $path } # file URI with no drive letter (unit-style). Just pass on as is with leading / and let OS deal with it
	if($path=~m,^[a-z0-9][a-z0-9]+:,i) {return $path } # absolute URI -- no idea how to handle, so just return
	return &abspath(File::Basename::dirname($base)."/$path");
	}


sub resolveURI
	{
	# return full path to 2nd arg relative to first (path or absolute URI)
	my $base = shift;
	my $path = shift;
	if($path=~m,[a-z0-9][a-z0-9]+:,i) {return $path } # absolute URI -- just return
	if($path=~m,^/,) {return $path } # path is absolute, but has no drive. Let OS deal with it.
	return &abspath(File::Basename::dirname($base)."/$path");
	}

sub localfile
	{
	my $file = shift;
	if($file=~s,file:///([a-zA-Z]:/),$1,) {return $file } # file URI with drive letter
	if($file=~m,file://,) {return $file } # file URI with no drive letter (unit-style). Just pass on as is with leading / and let OS deal with it
	if($file=~m,^([a-z0-9][a-z0-9]+):,i)
		{
		print  "ERROR: $1 scheme not supported\n";
		return;  # return empty string if not supported.
		} 
	return $file
	}



	

sub help
	{
	my $name= $0; $name=~s,^.*[\\/],,;
my $text;
format STDERR =
 ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  $text,
     ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<~~
    $text
.
print STDERR "usage: $name  [options...] sysdef\n  valid options are:\n\n";
	foreach (
		"-path [sm-path]\tspecifies the full system-model path to the file which is being processed. By default this is  \"/os/deviceplatformrelease/foundation_system/system_model/system_definition.xml\"",
			"   This must be an absolute path if you're processing a root sysdef.",
			"   If processing a pkgdef file, you can use \"./package_definition.xml\" to leave all links relative.",
		"effective-sysdef [local-file]\tspecifies another local filesystem location the sysdef should be considered when resolving linked metas and unit paths, but not system model item hrefs. This is mainly used for testing system-wide changes to pkgdefs since it allows the pkgdefs to exist in a separate location to the rest of the codeline"
		) {
		$text = $_;
		write STDERR;
		print STDERR "\n";
	}

	exit(1);
	}


	
