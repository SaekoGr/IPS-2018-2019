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
using namespace std;
#include<regex>
//#include<iterator>

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
void f(int ID, char tr[], char nr[]) {
	printf("My ID is: %d , I look for: %s  and want to replace it with: %s\n", ID, tr, nr);
	
	string to_replace = tr;
	std::regex reg(to_replace);
	string new_regex = nr;
	
	//std::string result = std::regex_replace(line, reg, new_regex);
	//std::cout << result << '\n';
	//printf("%s\n", line);
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


	
	threads.resize(re_count); 
	for(int i = 0; i < re_count; i++){	
		char* to_replace = argv[(i+1) * 2 - 1];
		char* new_regex = argv[(i+1) * 2];
		std::thread *new_thread = new std::thread (f, i + 1, to_replace, new_regex);
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
