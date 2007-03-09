biditex(1)							    biditex(1)



NAME
       biditex - automatic bidirectional support for LaTeX.

SYNOPSIS
       biditex [ parameters ] [ inputfilename ]

DESCRIPTION
       This  program  adds  bidirectional  tags  \L{}  or \R{} to LaTeX source
       according to unicode algorithm. It was designed to work	together  with
       ivritex.

OPTIONS
       The  program  receives  as input latex file that includes mixed English
       and Hebrew. In case no input file is given standard input is used.

       -o file_name.tex
	      Output file name.

       -e utf8 | iso8859-8 | cp1255
	      Source and target encoding. Default encoding is UTF-8.

       -m     Replace --  and  --- by \L{--} and \L{---}.  This  fixes
	      the  \emdash and \endash issue in Culmus David font that is sup
	      ported with ivritex.


       SEE ALSO
	      The documentation of the biditex - biditex-doc.pdf

BUGS
       TBD

AUTHOR
       Artyom Tonkikh 2007



								    biditex(1)
