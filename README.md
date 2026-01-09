# Parallel TF-IDF Based Document Similarity Search Engine

## Overview
This project presents the design and implementation of a document similarity–based search engine using the TF-IDF (Term Frequency–Inverse Document Frequency) model and cosine similarity.  
To study performance improvements, both **serial** and **parallel** implementations are provided, where the parallel version leverages **OpenMP** for concurrent execution.

The system indexes a collection of text files and allows users to query documents based on semantic similarity rather than exact keyword matching.

---

## Motivation
With the exponential growth of unstructured text data, efficient information retrieval has become increasingly important. Traditional serial search techniques struggle to scale when handling large document collections.

This project explores:
- The effectiveness of TF-IDF for document representation
- The impact of parallelism on indexing and vector construction
- Performance gains achieved using OpenMP

---

## Methodology

### 1. Text Preprocessing
- Tokenization
- Conversion to lowercase
- Removal of stopwords
- Alphabetic filtering

### 2. TF-IDF Vector Construction
Each document is represented as a normalized TF-IDF vector:

- **TF (Term Frequency)**  
  Number of occurrences of a term divided by total terms in the document.

- **IDF (Inverse Document Frequency)**  
  \[
  \log\left(\frac{N}{df(t)} + 1\right)
  \]

- **Vector Normalization**  
  Ensures cosine similarity is scale-independent.

---

## Serial Implementation
The serial version performs:
- Sequential document parsing
- Vocabulary construction
- TF-IDF vector computation
- Cosine similarity–based ranking

This implementation serves as a baseline for correctness and performance comparison.

---

## Parallel Implementation
The parallel version uses **OpenMP** to accelerate computation:

### Parallelized Phases
- Document tokenization and term frequency calculation
- Document frequency aggregation
- TF-IDF vector construction

### Techniques Used
- `#pragma omp parallel for`
- Dynamic scheduling
- Thread-local maps with critical section merging

The query phase is kept serial due to its interactive nature.

---

## Dataset Generation
A Python-based data generator is included to create synthetic datasets of varying sizes:
- Small files (1–5 KB)
- Medium files (50–100 KB)
- Large files (500 KB–1 MB)

This enables controlled performance evaluation.

---

