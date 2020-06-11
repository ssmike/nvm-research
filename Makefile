PDFTEX=pdflatex
VIEWER=okular
MAIN=report
VIM=vi
BIBTEX=bibtex
SRC=*.tex *.bib
BIBLIOGRAPHY=report

ifeq '$(BIBLIOGRAPHY)' ''
all: pdf

else
all: pdf bib pdf

endif

pdf:
	$(PDFTEX) $(MAIN).tex 2>/dev/null

view:
	$(VIEWER) $(MAIN).pdf

edit:
	$(VIM) $(SRC)

bib:
	$(BIBTEX) $(BIBLIOGRAPHY)

clean:
	rm *.log *.aux *.bbl *.xml *.toc *.blg *-blx.bib
