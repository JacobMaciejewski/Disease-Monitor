bin/test: source/workerMain.cpp lib/workerMainAux.o ../structures/lib/LinkedList.o ../structures/lib/LinkedListNode.o ../structures/lib/Date.o ../structures/lib/Record.o ../structures/lib/AVL_Tree.o ../structures/lib/AVL_TreeNode.o ../structures/lib/BucketPair.o ../structures/lib/Bucket.o ../structures/lib/HashTable.o ../structures/lib/HeapEntry.o ../structures/lib/HeapNode.o ../structures/lib/MaxHeap.o
	g++ -std=c++11 -ggdb3 -o workerProc source/workerMain.cpp lib/workerMainAux.o ../auxiliarry/auxiliarry.o ../structures/lib/LinkedList.o ../structures/lib/LinkedListNode.o ../structures/lib/Date.o ../structures/lib/Record.o ../structures/lib/AVL_Tree.o ../structures/lib/AVL_TreeNode.o ../structures/lib/BucketPair.o ../structures/lib/Bucket.o ../structures/lib/HashTable.o ../structures/lib/HeapEntry.o ../structures/lib/HeapNode.o ../structures/lib/MaxHeap.o
	mv workerProc ../

lib/workerMainAux.o: source/workerMain.cpp source/workerMainAux.cpp headers/workerMainAux.h
	g++ -std=c++11 -ggdb3 -c source/workerMainAux.cpp
	mv workerMainAux.o lib/workerMainAux.o

clean:
	rm lib/*
