/*
 *
 * Marshall Lindsay
 * ECE 4220
 * Lab2 Week1
 *
 */

/*
 *
 * Marshall Lindsay
 * ECE 4220
 * Lab2 Week1
 *
 */


#include <stdio.h>
#include <vector>
#include <time.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <pthread.h>
#include <unistd.h>

using namespace std;



void *singleThread(void* ptr);
void *rowThreads(void* ptr);
void *columnThreads(void* ptr);
void *everyThread(void* ptr);

int keyValue = 0;
int row = 0;
int column = 0;

int main(int argc, char* argv[]){

	//Check our command line arguments
	if(argc > 3){
		cout<<"\nInvalid number of command line arguments!"<<endl;
		return(1);

	}
	istringstream temp(argv[2]);
	temp>>keyValue;
	//cout<<"\nkeyValue = "<<keyValue<<endl;

	//Grab the filename
	string fileName(argv[1]);

	//Open the file
	ifstream file;
	file.open(fileName.c_str(), ios::in);


	//Check that the file was opened correctly
	if(!file.is_open()){
		cout<<"\nFailed to open the file!"<<endl;
		return(1);
	}


	int number;
	string line;

	//Get the first line from the file, these are the rows and columns respectively
	getline(file, line);

	//istringstream will be nice for the space delimited data
	istringstream iss(line);

	//Process the line
	iss>>row;
	//cout<<"\nRows = "<<row<<endl;
	iss>>column;
	//cout<<"\nColumns = "<<column<<endl;

	//cout<<"\nRow : "<<row<<"Column: "<<column<<endl;


	//Build the array of data
	vector< vector<int> > array;;

	for(int i = 0; getline(file, line); i++){
		istringstream iss(line);
		vector<int> row;
		for(int j = 0; iss>>number; j++){
			//cout<<"number = "<<number<<endl;
			(row).push_back(number);
		}
		array.push_back(row);
	}

	double average = 0;

	//Create the single thread case

	struct timespec start, end;
	double timeDiff;
	pthread_t soloThread;


	int* found;
	for(int r = 0; r < 10; r++){
		clock_gettime(CLOCK_REALTIME, &start);

		pthread_create(&soloThread, NULL, singleThread, (void*)&array);
		pthread_join(soloThread, (void**)(&found));
		clock_gettime(CLOCK_REALTIME, &end);

		timeDiff = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec)/(1E9);
		cout<<"\nTime taken for a single thread: "<<timeDiff<<" Number of matches: "<<*found<<endl;
		average += timeDiff;
	}
	for(int r = 0; r < 100; r++){
		cout<<"*";
	}
	average/=10;
	cout<<"\nAverage time taken for a single thread over 10 iterations: "<<average<<endl;
	for(int r = 0; r < 100; r++){
		cout<<"*";
	}
	//Create a thread for each row
	average = 0;
	pthread_t threadArray_row[row];
	int* rFound;
	for(int r = 0; r < 10; r++){
		*found = 0;
		clock_gettime(CLOCK_REALTIME, &start);
		for(int i = 0; i < row; i++){
			pthread_create(&threadArray_row[i], NULL, rowThreads, (void*)(&array[i]));
		}
		for(int i = 0; i < row; i++){
			pthread_join(threadArray_row[i], (void**)(&rFound));
			*found += *rFound;
			delete(rFound);
		}

		clock_gettime(CLOCK_REALTIME, &end);
		timeDiff = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec)/(1E9);
		cout<<"\nTime taken for one thread per row: "<<timeDiff<<" Number of matches: "<<*found<<endl;
		average+=timeDiff;
	}
	average/=10;
	for(int r = 0; r < 100; r++){
		cout<<"*";
	}
	cout<<"\nAverage time taken for one thread per row: "<<average<<endl;
	for(int r = 0; r < 100; r++){
		cout<<"*";
	}
	//Create a thread for each column

	//Going to split the vector into columns

	vector<vector<int> > cArray;

	for(int i = 0; i < column; i++){
		vector<int> vColumn;
		for(int j = 0; j < row; j++){
			vColumn.push_back(array[j][i]);
		}
		cArray.push_back(vColumn);
	}

	pthread_t threadArray_column[column];
	average = 0;
	for(int r = 0; r < 10; r++){
		*found = 0;
		*rFound = 0;

		clock_gettime(CLOCK_REALTIME, &start);
		for(int i = 0; i < column; i++){
			pthread_create(&threadArray_column[i], NULL, columnThreads, (void*)(&cArray[i]));
		}
		for(int i = 0; i < column; i++){
			pthread_join(threadArray_column[i], (void**)(&rFound));
			*found += *rFound;
			delete(rFound);
		}
		clock_gettime(CLOCK_REALTIME, &end);

		timeDiff = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec)/(1E9);
		cout<<"\nTime taken for one thread per column: "<<timeDiff<<" Number of matches: "<<*found<<endl;
		average += timeDiff;
	}
	average/=10;
	for(int r = 0; r < 100; r++){
		cout<<"*";
	}
	cout<<"\nAverage time taken for one thread per column: "<<average<<endl;
	for(int r = 0; r < 100; r++){
		cout<<"*";
	}

	average = 0;

	//Create a thread for each value
	pthread_t everyThreadArray[row][column];
	for(int r = 0; r < 10; r++){
		*found = 0;
		*rFound = 0;

		clock_gettime(CLOCK_REALTIME, &start);
		for(int i = 0; i < row; i++){
			for(int j = 0; j < column; j++){
				pthread_create(&everyThreadArray[i][j], NULL, everyThread, (void*)(&array[i][j]));
			}
		}
		for(int i = 0; i < row; i++){
			for(int j = 0; j < column; j++){
				pthread_join(everyThreadArray[i][j], (void**)(&rFound));
				*found += *rFound;
				delete(rFound);
			}
		}

		clock_gettime(CLOCK_REALTIME, &end);
		timeDiff = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec)/(1E9);
		cout<<"\nTime taken for one thread per element: "<<timeDiff<<" Number of matches: "<<*found<<endl;
		average+=timeDiff;
	}
	average/=10;
	for(int r = 0; r < 100; r++){
		cout<<"*";
	}
	cout<<"\nAverage time taken for one thread per element: "<<average<<endl;
	for(int r = 0; r < 100; r++){
		cout<<"*";
	}
	cout<<endl;
	delete(found);
	return(0);
}


void* singleThread(void* test){
	int* retVal = new int;
	*retVal = 0;
	vector<vector<int> > *array = (vector<vector<int> > *)test;

	for(int i = 0; i < row; i++){
		for(int j = 0; j < column; j++){
			if((*array)[i][j] == keyValue){
				(*retVal)++;
			}
		}
	}

	pthread_exit((void*)(retVal));

}

void* rowThreads(void* ptr){
	int* retVal = new int;
	*retVal = 0;

	vector<int> *array = (vector<int> *)(ptr);

	for(int i = 0; i < column; i++){
		if((*array)[i] == keyValue){
			(*retVal)++;
		}
	}


	pthread_exit((void*)(retVal));
}


void* columnThreads(void* ptr){
	int* retVal = new int;
	*retVal = 0;

	vector<int> * array = (vector<int> *)(ptr);

	for(int i = 0; i < row; i++){
		if((*array)[i] == keyValue){
			(*retVal)++;
		}
	}

	pthread_exit((void*)(retVal));
}

void* everyThread(void* ptr){

	int* retVal = new int;
	*retVal = 0;

	int* value = (int*)(ptr);

	if(*value == keyValue){
		*retVal = 1;
	}

	pthread_exit((void*)(retVal));
}
