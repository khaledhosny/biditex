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

$(TARGET).tex: $(SRC).tex
	biditex -m $(SRC).tex | iconv -f utf8 -t cp1255 >$(TARGET).tex

clean: 
	rm -f $(TARGET).dvi $(TARGET).ps $(TARGET).tex $(TARGET).log $(TARGET).aux $(TARGET).cot $(TARGET).pdf

