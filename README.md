# Gitlet C++ Implementation

A C++ implementation of a simplified version control system similar to Git. This project implements core Git functionality using custom data structures.

## Features

- Custom data structures (SimpleVec, SimpleMap)
- Basic Git operations: init, add, commit, log, checkout
- Content-addressed storage using hashing
- Simple staging area mechanism

## Custom Data Structures

- **SimpleVec**: A dynamic array implementation
- **SimpleMap**: A hash map implementation with simple collision handling

## Usage Example

```cpp
Gitlet repo;
repo.init();
repo.add("file1.txt", "Hello");
repo.commit("Add file1.txt");
repo.add("file1.txt", "Hello World!");
repo.commit("Update file1");
repo.log();
```

## Implementation Details

The project uses content-addressed storage where file contents are stored as blobs referenced by their hash values. Commits are identified by hash values generated from their content and metadata.