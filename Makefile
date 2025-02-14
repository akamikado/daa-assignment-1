clique: clique.cpp
	g++ clique.cpp -o clique -Wall -Wextra -Wconversion

bronkerbosch: bronkerbosch.cpp
	g++ bronkerbosch.cpp -o bronkerbosch -Wall -Wextra -Wconversion

arboricity-clique: arboricity_clique.cpp
	g++ arboricity_clique.cpp -o arboricity_clique -Wall -Wextra -Wconversion

clean:
	rm clique bronkerbosch arboricity_clique
