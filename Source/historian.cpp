/*

historian program

*/


#include "finalProject.h"

void printWelcomeMessage();
int checkUserInput(string input);
int main(void){

	while(1){

		string userInput;
		int option;
		while(1){
			//print the welcome message
			printWelcomMessage();
			//get user input
			getline(cin,userInput);
			//Validate user input
			if(!checkUserInput(userInput)){
				cout<<"Invalid input!"<<endl;
			}else{
				break;
			}

		}

	}
	return(1);
}

void printWelcomeMessage(){
	cout<<"\nWelcome to the Historian Program!"<<endl;
	cout<<"\nPlease select an option:"<<endl;
	cout<<"1)Start the RTUs"<<endl;
	cout<<"2)Send a command to the RTUs"<<endl;
	cout<<"3)Print the event history since start"<<endl;
	cout<<"4)Exit"<<endl;
}

void checkUserInput(string input){
	if(input == "1"){
		return(1);
	}else if(input == "2"){
		return(1);
	}else if(input == "3"){
		return(1);
	}else if(input == "4"){
		return(1);
	}else{
		return(0);
	}
}
