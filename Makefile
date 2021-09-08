INPUT=$(wildcard input/*)
OUTPUT=$(patsubst input/%,output/%,$(INPUT))
TESTRUN=$(patsubst input/%,benchmark/%,$(INPUT))
VERIFICARUN?=$(patsubst input/%,verificar/%,$(INPUT))
TIMELIMIT?=70
GABARITO=./prof/ribas-ac-ingenuo
BENCHMARKBINARY?=$(GABARITO)

check: /usr/bin/tar /usr/bin/time /usr/bin/diff

input/.create: input.tar.gz
	tar xf input.tar.gz
	@touch input/.create

output/%: input/%
	@echo "* Processando $^"
	@time -p $(GABARITO) $^ > $@
	@echo

gerargabarito: input/.create outputdir $(GABARITO) $(OUTPUT)

outputdir:
	mkdir -p output

benchmark/%: input/%
	@echo "* Processando $^"
	time -p $(BENCHMARKBINARY) $^ > $@
	@echo

benchmark: benchmarkdir input/.create $(TESTRUN)

benchmarkdir:
	mkdir -p benchmark

verificar/%: output/%
	@echo "* Comparando $^"
	@diff -q -s $^ $(patsubst verificar/%,benchmark/%,$@)
	@echo

verificar: benchmark $(VERIFICARUN)

clean:
	rm -rf benchmark
dist-clean: clean
	rm -rf input output

PHONY: check benchmark benchmarkdir clean dist-clean verificar gerargabarito outputdir
