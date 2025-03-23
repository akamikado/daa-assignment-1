cli: clique.cpp
	g++ clique.cpp -o cli -Ofast -Wall -Wextra -Wconversion

bb: bronkerbosch.cpp
	g++ bronkerbosch.cpp -o bb -Ofast -Wall -Wextra -Wconversion

arcli: arboricity_clique.cpp
	g++ chiba.cpp -o chiba -Ofast -Wall -Wextra -Wconversion

clean:
	rm clique bronkerbosch arboricity_clique

GZ_FILES = $(wildcard *.gz)
prepare: $(GZ_FILES)
	echo "Processing all datasets: $(GZ_FILES)"
	gzip $(GZ_FILES) -d
