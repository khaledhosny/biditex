TARGET=biditex-doc
SRC=biditex
LATEX=latex
P2P=ps2pdf

all: pdf

pdf: $(TARGET).pdf

$(TARGET).pdf: $(TARGET).ps
	$(P2P) $(TARGET).ps

$(TARGET).ps: $(TARGET).dvi
	dvips $(TARGET)

$(TARGET).dvi: $(TARGET).tex
	$(LATEX) $(TARGET).tex
	$(LATEX) $(TARGET).tex

$(TARGET).tex: $(SRC).tex
	biditex -m -e utf8 -t cp1255 $(SRC).tex -o $(TARGET).tex

clean: 
	rm -f $(TARGET).dvi $(TARGET).ps $(TARGET).tex $(TARGET).log $(TARGET).aux $(TARGET).cot $(TARGET).pdf

