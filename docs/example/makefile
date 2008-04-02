TARGET=example-heb
SRC=example
LATEX=latex

all: pdf

pdf: $(TARGET).pdf

$(TARGET).pdf: $(TARGET).ps
	ps2pdf $(TARGET).ps
	

$(TARGET).ps: $(TARGET).dvi
	dvips $(TARGET)

$(TARGET).dvi: $(TARGET).tex
	$(LATEX) $(TARGET).tex
	$(LATEX) $(TARGET).tex

$(TARGET).tex: $(SRC).tex
	biditex -e utf8 -t cp1255 -m $(SRC).tex -o $(TARGET).tex

clean: 
	rm -f $(TARGET).dvi $(TARGET).ps $(TARGET).tex $(TARGET).log $(TARGET).aux $(TARGET).cot $(TARGET).pdf

