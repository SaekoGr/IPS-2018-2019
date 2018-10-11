#include<stdio.h>
#include<unistd.h>
#include<thread>
#include<queue>
#include<mutex>
#include<vector>
#include <iostream>
#include<string.h>



std::vector<std::mutex *> zamky; /* pole zamku promenne velikosti */

char *line;

char *to_cstr(std::string a) {
	// prevede retezec v c++ do retezce v "c" (char *)
	char *str;
	str=(char *) malloc(sizeof(char)*(a.length()+1));
	strcpy(str,a.c_str());
	return str;
}

char *read_line(int *res) {
	std::string line;
	char *str;
	if (std::getline(std::cin, line)) {
		str=to_cstr(line);
		*res=1;
		return str;
	} else {
		*res=0;
		return NULL;
	}

}

void f(int ID) {
	/* funkce implementujici thread */
	printf("Thread %i started\n",ID);
}

// TODO: failed thread malloc
void create_threads(std::vector <std::thread *> threads, int re_count) 
{
	for(int i = 0; i < re_count; i++){	
	std::thread *new_thread = new std::thread (f,i);
	threads[i] = new_thread;
	}
}

int main(int argc, char* argv[]) {
	
	// check whether we have at least 2 parameters
	if(argc < 3){
		fprintf(stderr, "Not enough parameters\n");
		return 1;
	} // check whether we have even number of parameters
	else if((argc-1) % 2){
		fprintf(stderr, "There must be an even number of parameters\n");
		return 1;
	}

	/*******************************
	 * Inicializace threadu a zamku
	 * *****************************/
	int re_count = ((argc - 1) / 2); // Number of regular expressions  
	int num_zamky = 15;
	std::vector <std::thread *> threads; /* pole threadu promenne velikosti */

	/* vytvorime zamky */
	zamky.resize(num_zamky); /* nastavime si velikost pole zamky */
	for(int i=0; i<num_zamky; i++){	
		std::mutex *new_zamek = new std::mutex();
		zamky[i] = new_zamek;
	}

	/* vytvorime thready */
	threads.resize(num); /* nastavime si velikost pole threads */
	/**********************************
	 * Vlastni vypocet psed
	 * ********************************/
	int res;
	line = read_line(&res);
	while (res) {
		create_threads(threads, re_count);
		free(line); /* uvolnim pamet */
		line = read_line(&res);
	}


	/**********************************
	 * Uvolneni pameti
	 * ********************************/

	/* provedeme join a uvolnime pamet threads */
	for(int i=0;i<num;i++){
		(*(threads[i])).join();
		delete threads[i];
	}
	/* uvolnime pamet zamku */
	for(int i=0;i<num_zamky;i++){
		delete zamky[i];
	}

}
