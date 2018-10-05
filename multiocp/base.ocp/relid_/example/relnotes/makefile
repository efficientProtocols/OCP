#########################################################################
# Instructions to Make, for generation of nice-looking papers using LaTeX
#########################################################################

# RCS: $Id: makefile,v 1.1.1.1 1997/10/21 18:24:57 mohsen Exp $

SHELL=/bin/sh
DVIPS=dvips -f

.SUFFIXES:
.SUFFIXES:	.lpr .ps .tty .xdvi .dvi .tex 


.tex.dvi:
	sh -c 'if latex \\nonstopmode\\input $<; \
	  then exit 0; else rm $@; exit 1; fi'

.tex.tty:	
	sh -c 'if latex \\nonstopmode\\input $*.tex; \
	  then exit 0; else rm $*.dvi; exit 1; fi' 
	dvi2txt $*.dvi > $*.tty

.tex.ps:	
	sh -c 'if latex \\nonstopmode\\input $<; \
	  then exit 0; else rm $*.dvi; exit 1; fi' 
	sh -c 'if $(DVIPS) $*.dvi > $@; \
	  then exit 0; else rm $@; exit 1; fi'

.tex.xdvi:	
	sh -c 'if latex \\nonstopmode\\input $<; \
	  then exit 0; else rm $*.dvi; exit 1; fi' 
	xdvi $*.dvi &

.tex.lpr:	
	sh -c 'if latex \\nonstopmode\\input $*.tex; \
	  then exit 0; else rm $*.dvi; exit 1; fi' 
	sh -c 'if $(DVIPS) $*.dvi > $*.ps; \
	  then exit 0; else rm $*.dvi; exit 1; fi'
	lp $*.ps


.dvi.tty:
	dvi2txt $< > $@

.dvi.ps:
	sh -c 'if $(DVIPS)  $*.dvi > $*.ps; \
	  then exit 0; else rm $*.dvi; exit 1; fi'

.dvi.xdvi:
	xdvi $*.dvi &

.ps.lpr:
	lp $*.ps


#############################################################
# Here it is...
#############################################################

MANUAL=curenv-sol2

$(MANUAL).dvi:	$(MANUAL).tex 
	latex2dvi $(MANUAL).tex


all:	$(MANUAL).dvi $(MANUAL).ps $(MANUAL).tty

clean: 
	rm -f $(MANUAL).aux $(MANUAL).bbl $(MANUAL).blg $(MANUAL).cp \
	$(MANUAL).toc $(MANUAL).cps $(MANUAL).lot $(MANUAL).tty \
	$(MANUAL).log $(MANUAL).dvi $(MANUAL).auxO $(MANUAL).bblO $(MANUAL).ps


