#include <stdio.h>
#include<unistd.h>
#include<thread>
#include<mutex>

FILE *f;
//std::mutex zamek;


void t(int ID){
	//zamek.lock();
	fprintf(f,"Thread %i\n",ID);
	//usleep(1);
	//fprintf(f,"DATA Thread %i\n",ID);
	//zamek.unlock();
}

int main() {
	f=fopen("pokus.txt","w");
	std::thread t1(t,0);
	std::thread t2(t,1);
	fclose(f);
	t1.join();
	t2.join();
	//fclose(f);
	return 0;
}

