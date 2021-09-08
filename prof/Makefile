SOURCE=ribas-ac.cpp
all: ribas-ac-ingenuo ribas-ac-ioajustado

ribas-ac-ingenuo: $(SOURCE)
	$(CXX) -static -O3 -DINGENUO $(SOURCE) -o $@

ribas-ac-ioajustado: $(SOURCE)
	$(CXX) -static -O3 $(SOURCE) -o $@

clean:
	rm -f ribas-ac-ingenuo ribas-ac-ioajustado
