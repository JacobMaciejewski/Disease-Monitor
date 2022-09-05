bin/test: source/mainAux.cpp lib/mainAux.o ../structures/lib/LinkedList.o ../structures/lib/LinkedListNode.o ../structures/lib/Date.o ../structures/lib/Record.o ../structures/lib/AVL_Tree.o ../structures/lib/AVL_TreeNode.o ../structures/lib/BucketPair.o ../structures/lib/Bucket.o ../structures/lib/HashTable.o ../structures/lib/HeapEntry.o ../structures/lib/HeapNode.o ../structures/lib/MaxHeap.o
	g++ -std=c++11 -ggdb3 -o whoClient source/main.cpp lib/mainAux.o ../auxiliarry/auxiliarry.o ../structures/lib/LinkedList.o ../structures/lib/LinkedListNode.o ../structures/lib/Date.o ../structures/lib/Record.o ../structures/lib/AVL_Tree.o ../structures/lib/AVL_TreeNode.o ../structures/lib/BucketPair.o ../structures/lib/Bucket.o ../structures/lib/HashTable.o ../structures/lib/HeapEntry.o ../structures/lib/HeapNode.o ../structures/lib/MaxHeap.o -lpthread
	mv whoClient ../

lib/mainAux.o: source/main.cpp source/mainAux.cpp headers/mainAux.h 
	g++ -std=c++11 -ggdb3 -c source/mainAux.cpp -lpthread
	mv mainAux.o lib/mainAux.o

clean:
	rm lib/*