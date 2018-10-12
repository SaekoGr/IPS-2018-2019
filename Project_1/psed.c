#include<stdio.h>
#include<unistd.h>
#include<thread>
#include<queue>
#include<mutex>
#include<string>
#include<vector>
#include<iostream>
#include<string.h>

#define PRINT
#define READ

char **argv;
std::vector<std::mutex *> zamky; /* pole zamku promenne velikosti */
char *line = NULL;

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
/* Function performs work of the single thread, parameters: ID - number of thread, line - pointer to the currently read line, args - list of arguments */
void thread_regex_solve(int ID) {
	std::string re(argv[ID * 2 - 1]);
	std::string repl(argv[ID * 2]);
}

// TODO: failed thread malloc
void create_threads(std::vector <std::thread *> threads, int re_count) {
	threads.resize(re_count); 
	for(int i = 0; i < re_count; i++){	
		std::thread *new_thread = new std::thread (thread_regex_solve, i + 1);
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

	::argv = argv;

	/*******************************
	 * Inicializace threadu a zamku
	 * *****************************/
	int re_count = ((argc - 1) / 2); // Number of regular expressions  
	int num_zamky = 15; //tba
	std::vector <std::thread *> threads; /* pole threadu promenne velikosti */

	/* vytvorime zamky */
	zamky.resize(num_zamky); /* nastavime si velikost pole zamky */
	for(int i=0; i<num_zamky; i++){	
		std::mutex *new_zamek = new std::mutex();
		zamky[i] = new_zamek;
	}

	/* vytvorime thready */

	/**********************************
	 * Vlastni vypocet psed
	 * ********************************/
	int res;
	line = read_line(&res);
	create_threads(threads, re_count);
	while (res) {
		printf("%s\n",line);
		free(line); /* uvolnim pamet */
		line = read_line(&res);
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
	for(int i=0;i<num_zamky;i++){
		delete zamky[i];
	}

}
