bin/test: source/main.cpp lib/LinkedList.o lib/LinkedListNode.o lib/Date.o lib/Record.o lib/AVL_Tree.o lib/AVL_TreeNode.o lib/BucketPair.o lib/Bucket.o lib/HashTable.o lib/HeapEntry.o lib/HeapNode.o lib/MaxHeap.o
# 	g++ -std=c++11 -ggdb3 -o diseaseMonitor source/main.cpp lib/LinkedList.o lib/LinkedListNode.o lib/Date.o lib/Record.o lib/mainAux.o lib/AVL_Tree.o lib/AVL_TreeNode.o lib/BucketPair.o lib/Bucket.o lib/HashTable.o lib/HeapEntry.o lib/HeapNode.o lib/MaxHeap.o

lib/LinkedListNode.o: source/LinkedListNode.cpp headers/LinkedListNode.h
	g++ -std=c++11 -ggdb3 -c source/LinkedListNode.cpp
	mv LinkedListNode.o lib/LinkedListNode.o

lib/Record.o: source/Record.cpp headers/Record.h
	g++ -std=c++11 -ggdb3 -c source/Record.cpp
	mv Record.o lib/Record.o

lib/LinkedList.o: source/LinkedList.cpp headers/LinkedList.h
	g++ -std=c++11 -ggdb3 -c source/LinkedList.cpp
	mv LinkedList.o lib/LinkedList.o


lib/Date.o: source/Date.cpp headers/Date.h
	g++ -std=c++11 -ggdb3 -c source/Date.cpp
	mv Date.o lib/Date.o

# lib/mainAux.o: source/mainAux.cpp headers/mainAux.h
# 	g++ -std=c++11 -ggdb3 -c source/mainAux.cpp
# 	mv mainAux.o lib/mainAux.o

lib/AVL_Tree.o: source/AVL_Tree.cpp headers/AVL_Tree.h
	g++ -std=c++11 -ggdb3 -c source/AVL_Tree.cpp
	mv AVL_Tree.o lib/AVL_Tree.o

lib/AVL_TreeNode.o: source/AVL_TreeNode.cpp headers/AVL_TreeNode.h
	g++ -std=c++11 -ggdb3 -c source/AVL_TreeNode.cpp
	mv AVL_TreeNode.o lib/AVL_TreeNode.o

lib/BucketPair.o: source/BucketPair.cpp headers/BucketPair.h
	g++ -std=c++11 -ggdb3 -c source/BucketPair.cpp
	mv BucketPair.o lib/BucketPair.o

lib/Bucket.o: source/Bucket.cpp headers/Bucket.h
	g++ -std=c++11 -ggdb3 -c source/Bucket.cpp
	mv Bucket.o lib/Bucket.o

lib/HashTable.o: source/HashTable.cpp headers/HashTable.h
	g++ -std=c++11 -ggdb3 -c source/HashTable.cpp
	mv HashTable.o lib/HashTable.o

lib/HeapEntry.o: source/HeapEntry.cpp headers/HeapEntry.h
	g++ -std=c++11 -ggdb3 -c source/HeapEntry.cpp
	mv HeapEntry.o lib/HeapEntry.o

lib/HeapNode.o: source/HeapNode.cpp headers/HeapNode.h
	g++ -std=c++11 -ggdb3 -c source/HeapNode.cpp
	mv HeapNode.o lib/HeapNode.o

lib/MaxHeap.o: source/MaxHeap.cpp headers/MaxHeap.h
	g++ -std=c++11 -ggdb3 -c source/MaxHeap.cpp
	mv MaxHeap.o lib/MaxHeap.o

clean:
	rm lib/*
