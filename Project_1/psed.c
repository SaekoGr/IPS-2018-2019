#include<stdio.h>
#include<unistd.h>
#include<thread>
#include<queue>
#include<mutex>
#include<string>
#include<vector>
#include<iostream>
#include<string.h>
#include<iostream>
#include<string>
#include<regex>

std::vector<std::mutex *> printing, working; /* pole zamku promenne velikosti */
std::mutex currentline; // mutex for first line 
std::mutex print; // mutex for printing out the output
char *line = NULL;

// function recreates c++ string to c string
char *to_cstr(std::string a) {
	char *str;
	str=(char *) malloc(sizeof(char)*(a.length()+1));
	strcpy(str,a.c_str());
	return str;
}
// function reads first line from input pipeline
char *read_line(int *res) {
	std::string line;
	char *str;
	if (std::getline(std::cin, line)) {
		str=to_cstr(line);
		*res=1;
		return str;
	} 
	else {
		*res=0;
		return NULL;

	}
}

// Function performs work of the single thread, parameters: ID - number of thread, tr - string to replace,  re_count - number of threads working
void f(int ID, char tr[], char nr[], int re_count) {
	currentline.lock(); // threads needs to wait for first line
	currentline.unlock(); // if line was read, threads need to unlock the mutex so no deadlock occures

	while(line != NULL) {
		// Regex work and replacing
		std::string to_replace = tr;
		std::regex reg(to_replace);
		std::string new_regex = nr;

		std::string result = std::regex_replace(line, reg, new_regex);

		working[ID]->unlock();
		printing[ID]->lock(); // thread is finished with work, now needs to lock and wait until the main proccess doesnt call the thread to print the output

		print.lock(); // printing out the output must also be in mutex also critical section
		std::cout << result << '\n';
		print.unlock(); // unlocking the print mutex

		if(ID != (re_count - 1)) {// every single thread will unlock the next one
			printing[ID + 1]->unlock();
		}
	}
}


int main(int argc, char* argv[]) {
	
	// check whether we have at least 2 parameters
	if(argc < 3){
		fprintf(stderr, "ERROR: Not enough parameters\n");
		return 1;
	} // check whether we have even number of parameters
	else if((argc-1) % 2){
		fprintf(stderr, "ERROR: There must be an even number of parameters\n");
		return 1;
	}

	/*******************************
	 * Inicializace threadu a zamku
	 * *****************************/
	int re_count = ((argc - 1) / 2); // Number of regular expressions  
	std::vector <std::thread *> threads; /* pole threadu promenne velikosti */

	/* vytvorime zamky */
	printing.resize(re_count); // all thread need their own priting and working mutexes
	working.resize(re_count);
	for(int i=0; i<re_count; i++){	
		std::mutex *new_zamek = new std::mutex();
		printing[i] = new_zamek; // Printing mutex needs to be locked from start
		printing[i]->lock();
	}
	
	for(int i=0; i<re_count; i++){	
		std::mutex *new_zamek = new std::mutex();
		working[i] = new_zamek; // Printing mutex needs to be locked from start
		working[i]->lock();
	}

	currentline.lock(); // line was still not loaded , threads needs to wait for line

	
	threads.resize(re_count); 
	for(int i = 0; i < re_count; i++){	
		char* to_replace = argv[(i+1) * 2 - 1];
		char* new_regex = argv[(i+1) * 2];
		std::thread *new_thread = new std::thread (f, i, to_replace, new_regex, re_count);
		if(new_thread == NULL){
			fprintf(stderr, "ERROR: Failed to create thread!\n");
			exit(1);
		}
		else
			threads[i] = new_thread;
	}

	/**********************************
	 * Vlastni vypocet psed
	 * ********************************/
	int res;
	line = read_line(&res);
	
	currentline.unlock(); // line was read, we can unlock the threads

	while (res) {
		for(int i = 0; i < re_count ; i++) {
			working[i]->lock(); // waiting for every single each thread to finish
		}
		free(line); // threads are locked here and finished with work we can deallocate line
		line = read_line(&res); // read new line threads are still locked
		
		printing[0]->unlock(); // unlocking the first thread, this thread will unlock the others
	}

	/**********************************
	 * Uvolneni pameti
	 * ********************************/
	
	/* provedeme join a uvolnime pamet threads */
	for(int i=0;i < re_count;i++){
		(*(threads[i])).join();
		delete threads[i];
	}
	/* uvolnime pamet zamku */
	for(int i=0;i<re_count;i++){
		delete printing[i];
	}
}