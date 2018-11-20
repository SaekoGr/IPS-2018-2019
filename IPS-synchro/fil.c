#include <stdio.h>
#include<unistd.h>
#include<thread>
//#include<queue>
#include<mutex>

#define NUM 10


std::mutex zamky[NUM];


void fil(int ID) {
	printf("filosof %i\n",ID);
	/*
	zamky[ID].lock();
	usleep(1);
	//zamky[(ID+1)%NUM].lock();
	while (!zamky[(ID+1)%NUM].try_lock()){
		printf("cekam na zamek 2\n");
		zamky[ID].unlock();
		usleep(10);
		zamky[ID].lock();
	}*/
	lock(zamky[ID],zamky[(ID+1)%NUM]);
	
	printf("F %i eating\n",ID);
	sleep(1);
	zamky[ID].unlock();
	zamky[(ID+1)%NUM].unlock();
	 
}

int main() {
	std::thread * t[NUM];
	for(int i=0;i<NUM;i++){	
		t[i] = new std::thread (fil,i);
	}
	for(int i=0;i<NUM;i++){
		(*(t[i])).join();
		delete t[i];
	}

	printf("everything finished\n");
	
}

