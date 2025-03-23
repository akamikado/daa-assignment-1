# **Maximal Clique Enumeration**  

## **Table of Contents**  
- [Introduction](#introduction)  
- [Prerequisites](#prerequisites)  
- [Dataset Preparation](#dataset-preparation)  
- [Execution Instructions](#execution-instructions)  
- [Individual Contributions](#individual-contributions)  

## **Introduction**
This project implements multiple algorithms for **Maximal Clique Enumeration** in sparse graphs. The goal is to efficiently list all maximal cliques using different algorithmic approaches, each optimized for specific scenarios.  
Project Webpage: [Click here to view](https://daa-assignment-1.pages.dev/)

---

## **Prerequisites**  
Ensure the following dependencies are installed before running the project:  
- **Make** – for build automation  
- **g++** – GNU C++ compiler  
- **gzip** – for handling compressed datasets  

To verify installations, run:  
```bash
make --version
g++ --version
gzip --version
```

---

## Dataset Preparation
To prepare compressed datasets (.gz files), move them into the project directory and run:
```bash
make prepare
```

## **Execution Instructions**  
To run different Maximal Clique Enumeration algorithms, use the following commands:
- The worst-case time complexity for generating all maximal cliques and computational experiments.
```bash
make tomita
./tomita dataset.txt
```

- Listing All Maximal Cliques in Sparse Graphs in Near-Optimal TIme
```bash
make els
./els dataset.txt
```

- Arboricity and Subgraph Listing Algorithms
```bash
make chiba
./chiba dataset.txt
```

## Individual Contributions
| Name | ID Number |
| --- | --- |
| Manish Vasireddy | 2022A7PS0145H |
| Linga Sai Vaishnav Yerram | 2022A7PS0060H | worked on implementing proc BronKerboschDegeneracy|
| Srichandra Lolla | 2022A7PS0051H | worked on implementing proc BronKerboschDegeneracy|
| Sasidhara Reddy Appireddy | 2022A7PS0019H |
| Vipul Alampally | 2022A7PS0098H |


