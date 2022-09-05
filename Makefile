.SILENT :
			all
			clean

all:
	make -s -C ./structures
	make -s -C ./auxiliarry
	make -s -C ./whoMaster
	make -s -C ./whoWorker
	make -s -C ./buffer
	make -s -C ./client
	make -s -C ./server

clean:
	make clean -s -C ./structures
	make clean -s -C ./auxiliarry
	make clean -s -C ./whoMaster
	make clean -s -C ./whoWorker
	make clean -s -C ./buffer
	make clean -s -C ./client
	make clean -s -C ./server
	rm master
	rm workerProc
	rm whoClient
	rm whoServer
