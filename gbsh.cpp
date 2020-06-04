#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <queue>
#include <dirent.h>
#include <fcntl.h>
#include <sstream>
#include <signal.h>
using namespace std;


string toString(char*);			//converts the character array to the string
void init(char*,int);				//initialize the character array with null value
void breakCommands(string,queue<string>*);			//it breaks the commads and store each piece into a vector
void displayBrokenCommands(queue<string>*);					//displays vector on the terminal
bool strcmp(char*,string);      //compare the string with the character array
void clearQueue(queue<string>*);				//to clear the queue
char* toChar(string);       //to convert the string in to a character array


void execution(queue<string>);			//it calls the required commands
void clear();				//clear the contents of the shell
void ls();						//shows the contents of the current working directory
void cd(queue<string>);					//change the current working directory
void environn();					//prints the environment variables
void setenvv(queue<string>);			//set the environment variable to the specified value
void unsetenvv(string);				//unset the environment variable
void top();							//executes the top command
void ps();								//executes the ps command
void man(queue<string>);							//executes the man command
void pwdwrite(queue<string>);				//writes content to the given file
void lswrite(queue<string>);					//writes content to the given file
void environwrite(queue<string>);				//writes content to the given file
void sigintCatcher(int);						//catch the CTRL C command and doesn't let the program break
void backgroundExecution(queue<string>);				//execute the command in the background
void execCall(int, int, queue<string>);						//creates a new process and calls exec
void redirection(queue<string>);								//performs the piping and input output redirection
void inputFileredirection(string);				//redirect stdin to file
void outputFileredirection(string);				//redirect stdout to file
void redirectionReset(int,int);					//redirect the stdout or stdin to the origianl descriptors


int main(int argc, char *argv[]) 
{
	string username = getlogin();		//gets the login name of the current user

	char temp[255];		init(temp,255);		gethostname(temp,255);			//gets the host name of the current device
	string hostname;		hostname = toString(temp);				//converts the character array to string

	string currentdir;					//gets the current directory in which terminal is open

	string env_shell = get_current_dir_name();	env_shell+=+"/gbsh";		//storing the shell path way for gbsh
	setenv("SHELL",env_shell.c_str(),1);					//setting the gbsh pathway into the environment

	queue<string> brokenCommands;				//break the commands and store it in the queue

	string data;			//variable for storing input from user

	int console_read_copy = dup(0), console_write_copy = dup(1);			//keep the original integer to the stdin and stdout

	while(1)
	{
		redirectionReset(console_read_copy,console_write_copy);			//reset the stdin and stdout to the original descriptor holding it
		currentdir = get_current_dir_name();	clearQueue(&brokenCommands);	//get the current directory and clear the queue containing the command entered by the user
		signal(SIGINT,sigintCatcher);			//catch the ctrl c call by the user
		cout<<"\033[1;32m<"<<username<<">@<"<<hostname<<"> \033[1;34m<"<<currentdir<<"> \033[0m> ";					//display the hostname, username, and pwd
		getline(cin,data);				//get the input from the user

		breakCommands(data,&brokenCommands);				//break the input entered by the user

		if(!brokenCommands.empty())					//if no command is entered
		{
			if("exit" == brokenCommands.front())			//close the terminal if exit command has been given
			{
				brokenCommands.pop();				//clear the queue
				break;
			}
			else
			{
				execution(brokenCommands);				//contains the execution calls to all the commands supported by the shell
			}
		}

		
		data.clear();				//flushing the input by the user
	}

	exit(0);	 // exit normally	
}


string toString(char* temp)			//converts the character array to the string
{
	string str;				//the string container
	for(int a=0;temp[a]!='\0';a++)					//iterator to iterate character array
	{
		str+=temp[a];			//storing the character array into a string
	}

	return str;
}


void init(char* arr,int size)				//initialize the character array with null value
{
	for(int a=0;a<size;a++)				//iterator for initializing the character array
	{
		arr[a]='\0';				//storing null in it
	}

	return;
}


void breakCommands(string command ,queue<string>* vect)			//it breaks the commads and store each piece into a vector
{
	string data;			//string storing a single word of a command
	size_t size = command.size();				//size of the command entered by the user

	for(int a=0;a<=size;a++)				//iterator for iterating the command
	{
		if(command[a]!='\0' && command[a]!='\n' && command[a]!=' ')					//dividing it on the basis of nextLine, space or end of file
		{
			data+=command[a];				//storing it string by string
		}
		else
		{
			if(!data.empty())				//if the contaiiner isn't empty
			{
				vect->push(data);		data.clear();			//pushes it into the queue
			}
		}
	}

	return;
}


void displayBrokenCommands(queue<string>* vect)					//displays vector on the terminal
{
	size_t size = vect->size();				//getting the size of the queue

	for(int a=0;a<size;a++)
	{
		cout<<"		"<<vect->front()<<endl;	vect->pop();				//displaying the command word by words
	}

	return;
}


bool strcmp(char* str1, string str2)        //compare the string with the character array
{
	int a=0,b=0;			//counters
	for(a=0, b=0; str1[a]!='\0' || str2[b]!='\0'; a++,b++)			//iterator for iterating through character array and string
	{
		if(str1[a]!=str2[b])			//if a character doesn't match then return false
		{
			return false;
		}
	}

	if(a==b)			//if the counters are equal the string and character array matched
	{
		return true;
	}
	else
	{
		return false;			//if the string and character array dont match
	}
	
}


void clear()				//clear the contents of the shell
{
	cout<<"\e[1;H\e[2J";			//clear the screen of the shell

	return;
}


void ls()						//shows the contents of the current working directory
{
	string currentdir = get_current_dir_name();			//storing the current directory in a string
	
	DIR *dir = opendir(currentdir.c_str());		dirent *files = readdir(dir);		//open the current directory to get the contents

	while(files != NULL)		//untill all the files aren't listed
	{
		if(!strcmp(files->d_name,".") && !strcmp(files->d_name,".."))			//the descriptors to the return of folder
		{
			if(files->d_name[0]!='.')			//if the file is hidden
			{
				cout<<files->d_name<<endl;			//display the file in a listing method
			}
		}
		files = readdir(dir);			//reads the next file
	}

	closedir(dir);			//close the file descriptor

	return;
}


void cd(queue<string> brokenCommands)					//change the current working directory
{
	string data;			//storing the address

	if(!brokenCommands.empty())				//if no directory is specified
	{
		int size = brokenCommands.size();			//getting the size of the directory

		for(int a=0;a<size;a++)				//iterator if the file name contain many words
		{
			data+=brokenCommands.front();	brokenCommands.pop();		//storing the directory or filename
			if(a+1<size)
			{
				data+=" ";		//if the file name contains spaces
			}
		}

	}

	if(data.empty())			//if no directory or file is specified
	{
		data+="/home/talha/";			//get to the default directory
	}

	int err = chdir(data.c_str());			//to store the error produced by the command
	if(err == -1)						//if no file or directory is found
	{
		cout<<"No such file or directory\n";
	}

	return;
}


void environn()					//prints the environment variables
{
	extern char **environ;			//the array containing the environment variables

	for(int a=0;environ[a]!=NULL;a++)					//iterator for iterating through the environment variables
	{
		cout<<environ[a]<<endl;			//listing the environment variables
	}

	return;
}


void setenvv(queue<string> brokenCommands)			//set the environment variable to the specified value
{
	string var = brokenCommands.front();	brokenCommands.pop();			//geting the name of the variable
	string value;				//container for storing the value for environment variable

	if(brokenCommands.empty())			//if no value is mentioned
	{
		value="";			//store nothing
	}
	else
	{
		while(!brokenCommands.empty())
		{
			value += brokenCommands.front();		brokenCommands.pop();	//store the value given by the user

			if(!brokenCommands.empty())		//if the value contains spaces 
			{
				value += " ";
			}
		}
	}

	if(getenv(var.c_str())==NULL)			//if the variable isn't already set as environment variable
	{
		setenv(var.c_str(),value.c_str(),0);			//set the environment variable
	}
	else if(strcmp(getenv(var.c_str()),""))				//if the environment variable has been set to nothing
	{
		setenv(var.c_str(),value.c_str(),1);				//reset the value
	}
	else
	{
		cout<<"		environment variable is already defined\n";				//if the variable is already defined
	}

	return;
}


void unsetenvv(string var)
{
	unsetenv(var.c_str());			//unset the environment variabl

	return;
}


void clearQueue(queue<string>* vect)				//to clear the queue
{
	int size = vect->size();			//gettig the size of the queue

	for(int a=0;a<size;a++)				//iterator for iteratoring through the queue
	{
		vect->pop();				//popping the values of the queue
	}

	return;
}


char* toChar(string str)           //converts the string into the character array
{
    char *ch = new char[str.length()+1];      //initializing the character array of the size of the string
    for(int a=0;str[a]!='\0';a++)           //storing it into the character array
    {
        ch[a]=str[a];			//storing the string in the array
    }
	ch[str.length()]='\0';			//putting a null value after
    return ch;
}


void top()							//executes the top command
{
	char command[3] = {'t','o','p'};		char *args[] = {command,NULL};		execvp("top",args);			//calling exec for top

	return;
}


void ps()								//executes the ps command
{
	char command[2] = {'p','s'};		char *args[] = {command,NULL};		execvp("ps",args);		//calling exec for ps
	
	return;
}


void man(queue<string> brokenCommands)							//executes the man command
{
	char *command = toChar(brokenCommands.front());		//onverting the string to the character
	brokenCommands.pop();

	int size = brokenCommands.size();		//storing the size of the command and the arguments
	char **inst = new char *[size+2];			//character array for storing the arguments

	inst[0] = command;

	for(int a=1;a<size+1;a++)			//iterator for storing the arguments
	{
		inst[a] = toChar(brokenCommands.front());		brokenCommands.pop();
	}

	inst[size+1]=NULL;			//storing the NULL

	execvp("man",inst);				//calling the exec

	return;
}


void environwrite(queue<string> brokenCommands)				//writes content to the given file
{
	if(">" == brokenCommands.front())			//if the write command is specified
	{
		if(brokenCommands.size() == 1)			//if the commands entered aren't correct
		{
			cout<<"		"<<"environ"<<endl;
			displayBrokenCommands(&brokenCommands);			//display them according to the specified format
		}
		else
		{
			brokenCommands.pop();

			creat(brokenCommands.front().c_str(),S_IRWXU);				//create the file with all the access privileges
			int file_fd = open(brokenCommands.front().c_str(),O_WRONLY);			//open the file

			stringstream content;		//character array for storing the all the environment variables

			extern char ** environ;			//the character array containing all the environmrnt variables

			for(int a=0; environ[a]!=NULL;a++)			//iterate untill the end of environment variable array
			{
				content<<environ[a];	content<<"\n";			//storing them in the array
			}

			write(file_fd,content.str().c_str(),content.str().size()-1);		close(file_fd);		//writing the data to the file
		}
	}
	else
	{
		cout<<"		"<<"environ"<<endl;			//if the commands aren't given in a right format
		displayBrokenCommands(&brokenCommands);			//display it according to the given format
	}

	return;
}


void sigintCatcher(int sigInt)						//catch the CTRL C command and doesn't let the program break
{
	cout<<endl;			main(0,NULL);			//if ctrl c id pressed, the function catches it and call the main function again
	
	return;
}


void backgroundExecution(queue<string> brokenCommands)				//execute the command in the background
{
	if("&" == brokenCommands.back())	//if the background function has been called
	{
		int size = brokenCommands.size();		//gets the size of the queue

		while(size-1>0)			//pushes it in the queue again except the &
		{
			brokenCommands.push(brokenCommands.front());		brokenCommands.pop();			
			size--;
		}
		brokenCommands.pop();		//pop the & from queue

	}
	else if('&' == brokenCommands.back()[brokenCommands.back().size()-1])	//if the & is specified without space
	{
		int counter = 0; string temp;
		while(brokenCommands.back()[counter]!='&')			//get the command without & with the help of an iterator
		{
			temp += brokenCommands.back()[counter];		counter++;
		}
		brokenCommands.back() = temp;			//store it back into queue
	}

	pid_t proc = fork();			//creates a new processor

	if(proc == 0)			//child process
	{
		execution(brokenCommands);			//calls the execution function to execute commands
	}
	else				//parent process
	{
		return;
	}
	
	return;
}


void execution(queue<string> brokenCommands)			//it calls the required command
{
	if("pwd" == brokenCommands.front() && brokenCommands.size() == 1)			//supported commands are executed by if
	{
		cout << get_current_dir_name() << endl;
		brokenCommands.pop();
	}
	else if ("clear" == brokenCommands.front())
	{
		brokenCommands.pop();
		clear();
	}
	else if ("ls" == brokenCommands.front() && brokenCommands.size() == 1)
	{
		brokenCommands.pop();
		ls();
	}
	else if ("cd" == brokenCommands.front())
	{
		brokenCommands.pop();
		cd(brokenCommands);
	}
	else if ("environ" == brokenCommands.front() && brokenCommands.size() == 1)
	{
		brokenCommands.pop();
		environn();
	}
	else if ("setenv" == brokenCommands.front())
	{
		brokenCommands.pop();
		setenvv(brokenCommands);
	}
	else if ("unsetenv" == brokenCommands.front())
	{
		brokenCommands.pop();
		unsetenvv(brokenCommands.front());
	}
	else
	{
		int status_A;
		pid_t proc_A = fork();

		if (proc_A == 0)
		{
			if ("top" == brokenCommands.front())
			{
				top();
			}
			else if ("ps" == brokenCommands.front())
			{
				ps();
			}
			else if ("man" == brokenCommands.front())
			{
				man(brokenCommands);
			}
			else if ("environ" == brokenCommands.front())
			{
				brokenCommands.pop();
				environwrite(brokenCommands);
				exit(0);
			}
			else if ("&" == brokenCommands.back() || '&' == brokenCommands.back()[brokenCommands.back().size() - 1])
			{
				backgroundExecution(brokenCommands);
				exit(0);
			}
			else
			{
				redirection(brokenCommands);		exit(0);
			}
		}
		else
		{
			wait(NULL);
		}
	}

	return;
}


void execCall(int in, int out, queue<string> arguments)						//creates a new process and calls exec
{
	pid_t proc = fork();			//creates the process

	if(proc == 0)			//child process
	{
		char *args [arguments.size()+1];	args[0] = toChar(arguments.front());	int counter = 1;	arguments.pop();		//intialized the character pointer array
			
		while(!arguments.empty())		//store the arguments in the array
		{
			args[counter] = toChar(arguments.front());	arguments.pop();	counter++;		//counter for iterator
		}

		args[counter] = NULL;		//storing the null

		if(in!=0)		//if read file descriptor has been changed
		{
			dup2(in,0);
		}
		
		if(out!=1)			//if write file descriptor has been changed
		{
			dup2(out,1);
		}

		execvp(args[0],args);		//executing the commands

		exit(0);
	}
	else
	{
		wait(NULL);
	}
	

	return;
}


void redirection(queue<string> brokenCommands)						//contains redirection for files and pipes
{
	queue<string> used, arguments;			//queue for storing the used part of the command and arguments

	bool read_write_file = false, read_write_pipe = false; int prev_read = 0, console_read_copy = dup(0), console_write_copy = dup(1);		//copying the file descriptors and read_write pipe/file check

	while(!brokenCommands.empty())		//until the queue isn't empty
	{
		while(!brokenCommands.empty() && brokenCommands.front() != "<" && brokenCommands.front() != ">" && brokenCommands.front() != "|")		//collect all the arguments
		{
			arguments.push(brokenCommands.front());	used.push(brokenCommands.front());	brokenCommands.pop();
		}

		if(!brokenCommands.empty() && brokenCommands.front() == ">")		//for output to the file
		{
			used.push(brokenCommands.front());	brokenCommands.pop();
			
			if(!brokenCommands.empty())
			{
				outputFileredirection(brokenCommands.front());
				used.push(brokenCommands.front());	brokenCommands.pop();	read_write_file = true;
			}
		}
		else if(!brokenCommands.empty() && brokenCommands.front() == "<")			//for input to the file
		{
			used.push(brokenCommands.front());	brokenCommands.pop();
			
			if(!brokenCommands.empty())
			{
				inputFileredirection(brokenCommands.front());	used.push(brokenCommands.front());	brokenCommands.pop();
				read_write_file = true;
			}
		}
		else if((!brokenCommands.empty() && brokenCommands.front() == "|") || (brokenCommands.empty() && prev_read!=0))			//if its a pipe
		{
			if(!brokenCommands.empty())
			{
				used.push(brokenCommands.front());	brokenCommands.pop();
			}

			if(prev_read != 0)			//if the pipe has been used previously
			{
				if(brokenCommands.empty())				///if thats only command left
				{
					execCall(prev_read,1,arguments);	prev_read = 0;	read_write_pipe = true;	read_write_file = false;
				}
				else		//if there is a input or output redirection to the file is given
				{
					if(!brokenCommands.empty() && brokenCommands.front() == ">")
					{
						used.push(brokenCommands.front());	brokenCommands.pop();
				
						if(!brokenCommands.empty())
						{
							outputFileredirection(brokenCommands.front());	used.push(brokenCommands.front());	brokenCommands.pop();
							execCall(prev_read,1,arguments);	prev_read = 0;	read_write_pipe = true;	read_write_file = false;
						}
					}
					else if(!brokenCommands.empty() && brokenCommands.front() == "<")
					{
						used.push(brokenCommands.front());	brokenCommands.pop();
				
						if(!brokenCommands.empty())
						{
							inputFileredirection(brokenCommands.front());	used.push(brokenCommands.front());	brokenCommands.pop();
							execCall(prev_read,1,arguments);	prev_read = 0;	read_write_pipe = true;	read_write_file = false;
						}
					}
					else			//execute the command normally
					{
						int fd[2];	pipe(fd);	execCall(prev_read,fd[1],arguments);	close(fd[1]);	prev_read = fd[0];
	
					}					
				}
			}
			else			//execute the command normally
			{
				int fd[2];	pipe(fd);	execCall(prev_read,fd[1],arguments);	close(fd[1]);	prev_read = fd[0];
			}
			read_write_file = false;	clearQueue(&arguments);			//clear the arguments
		}
	}

	if(prev_read!=0 && read_write_file)		//if at the end of command a file has been specified
	{
		execCall(prev_read,1,arguments); read_write_pipe = true;	read_write_file = false;
	}

	if(read_write_file)			//if its simple file indirection
	{
		if(!arguments.empty())				//storing the arguments
		{
			char *args [arguments.size()+1];	args[0] = toChar(arguments.front());	int counter = 1;	arguments.pop();
			
			while(!arguments.empty())
			{
				args[counter] = toChar(arguments.front());		arguments.pop();	counter++;
			}

			args[counter] = NULL;

			execvp(args[0],args);			//calling the exec
		}
	}

	if(!read_write_pipe)			//if a wrong command or unknown format is specified
	{
		redirectionReset(console_read_copy,console_write_copy);	 displayBrokenCommands(&used);			//display it in the specified format
	}

	return;
}


void inputFileredirection(string file_name)				//redirect stdin to file
{
	int fd_read = open(file_name.c_str(),O_RDONLY);			//for redirection to the file for input
	dup2(fd_read,0);

	return;
}


void outputFileredirection(string file_name)	//redirect stdout to file
{
	int fd_write = open(file_name.c_str(),O_TRUNC|O_WRONLY);		//open the file

	if(fd_write == -1)
	{
		creat(file_name.c_str(),S_IRWXU);	fd_write = open(file_name.c_str(),O_TRUNC|O_WRONLY);		//if file isn't present, it would be created and openened
	}

	dup2(fd_write,1);			//redirecting the stdout to the file
	
	return;
}


void redirectionReset(int read,int write)					//redirect the stdout or stdin to the origianl descriptors
{
	dup2(read,0);	dup2(write,1);			//resetting to the original descriptors

	return;
}

