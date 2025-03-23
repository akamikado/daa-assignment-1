tomita: clique.cpp
	g++ tomita.cpp -o tomita -Ofast -Wall -Wextra -Wconversion

els: els.cpp
	g++ bronkerbosch.cpp -o els -Ofast -Wall -Wextra -Wconversion

arcli: arboricity_clique.cpp
	g++ chiba.cpp -o chiba -Ofast -Wall -Wextra -Wconversion

clean:
	rm tomita els chiba

GZ_FILES = $(wildcard *.gz)
prepare: $(GZ_FILES)
	echo "Processing all datasets: $(GZ_FILES)"
	gzip $(GZ_FILES) -d
