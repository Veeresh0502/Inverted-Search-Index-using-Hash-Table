# Inverted Search Index using Hash Table

![Language](https://img.shields.io/badge/Language-Advanced%20C-blue)
![Data Structure](https://img.shields.io/badge/Data%20Structure-Hash%20Table%20%2B%20Linked%20List-purple)
![Topic](https://img.shields.io/badge/Topic-Search%20Engine%20Indexing-orange)
![License](https://img.shields.io/badge/License-Open%20Source%20%28Educational%29-green)

> Build a word index across multiple files — just like a search engine does — using hash tables with separate chaining, persistent storage, and fast lookups.

---

## 📌 Overview

This project implements an **inverted index** — the core data structure behind every search engine. Given a set of text files, it extracts every unique word, maps each word to the list of files it appears in, and stores the index in a hash table for O(1) average-case lookup.

```
"hello"  →  [ f1.txt, f3.txt ]
"world"  →  [ f1.txt, f2.txt, f3.txt ]
"linux"  →  [ f2.txt ]
```

The index is **case-insensitive** and **persists to disk** — so it survives across sessions without re-indexing.

---

## 🔗 Data Structure Design

```
Hash Table (array of buckets)

Index 0  →  [ "apple" → {f1.txt, f2.txt} ] → [ "ant" → {f3.txt} ] → NULL
Index 1  →  NULL
Index 2  →  [ "ball"  → {f1.txt} ]          → NULL
Index 3  →  [ "cat"   → {f2.txt, f3.txt} ]  → NULL
  ...
```

Each bucket holds a **linked list** (separate chaining) of word nodes. Each word node holds another linked list of file names where that word appears. Collisions are resolved cleanly without probing or rehashing.

---

## 📂 Project Structure

```
.
├── main.c          ← entry point, CLI, menu loop
├── main.h          ← structs, macros, function declarations
├── create.c        ← index creation from input files
├── search.c        ← word lookup in the hash table
├── display.c       ← print index contents
├── update.c        ← add new files to existing index
├── save.c          ← persist index to disk (database format)
├── validate.c      ← input validation and error handling
├── function.c      ← hash function and utility helpers
├── Makefile        ← build system
├── f1.txt          ← sample input file 1
├── f2.txt          ← sample input file 2
├── f3.txt          ← sample input file 3
└── exe.txt         ← saved index database
```

---

## ▶️ Getting Started

### Build

```bash
make
```

### Run

```bash
./a.out f1.txt f2.txt f3.txt
```

Pass any number of `.txt` files as arguments. The program indexes all of them in one pass.

---

## 🖥️ Example Session

```
Files indexed: f1.txt  f2.txt  f3.txt

1. Search word
2. Display index
3. Update index (add files)
4. Save index to database
5. Exit

Choice: 1
Enter word: linux

"linux" found in:
  → f2.txt
```

```
Choice: 2

Word          Files
-----------   ----------------------------
hello         f1.txt  f3.txt
linux         f2.txt
world         f1.txt  f2.txt  f3.txt
```

---

## 🛠️ Technologies Used

- Advanced C
- Hash tables (custom implementation, separate chaining)
- Linked lists (word list + file list — nested structures)
- Dynamic memory allocation (`malloc`, `free`, `realloc`)
- File I/O for multi-file reading and database persistence
- String manipulation (tokenization, case normalization)
- Pointers and pointer-to-pointer operations
- Structures (`struct` for word nodes and file nodes)
- Command line arguments (`argc`, `argv`)
- Makefile-based build system

---

## 🧠 Key Learnings

- Implemented a hash table from scratch with separate chaining for collision resolution
- Designed nested linked list structures: one list per word, one list per file within each word
- Built a custom database persistence format to save and reload the index across sessions
- Handled case-insensitive indexing by normalizing all words to lowercase at insert time
- Mastered deep pointer manipulation for traversal, insertion, and safe memory cleanup

---

## ⚠️ Challenges Faced

- Designing a clean hash function that distributes words uniformly across buckets
- Managing nested dynamic structures without memory leaks (word nodes + file nodes + strings)
- Avoiding duplicate file entries for the same word when processing multiple files
- Implementing the save/load format so the index can be fully reconstructed from disk
- Handling edge cases: empty files, punctuation in words, repeated words, single-word files

---

## 🚀 Roadmap

- [ ] Track word frequency per file (not just presence)
- [ ] Rank search results by frequency (TF-IDF style scoring)
- [ ] Support multi-word phrase search
- [ ] Add delete word / remove file from index
- [ ] Index recursively from a directory path

---

## 👨‍💻 Author

**Veeresh T** (`Veeresh0502`)

Developed as part of learning Advanced Data Structures and Systems Programming in C.

---

## ⭐ License

Open source for educational use. Fork it, learn from it, build on it!
