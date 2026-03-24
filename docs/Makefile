PDF=out/BaneWfn_Project_Manual_zh.pdf

.PHONY: all pdf clean

all: pdf

pdf: $(PDF)

$(PDF): BaneWfn_Project_Manual_zh.md pandoc.yaml preamble.tex build.sh
	./build.sh

clean:
	rm -f $(PDF)
