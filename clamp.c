#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<signal.h>

#define true 1
#define false 0
#define error -1
#define STOP 2
#define PAUSE 3
#define tmp_pid_file "/tmp/clamp/pid"

/* global variables */
FILE *pid_file_read;				//read from pid file 
FILE *pid_file_write;				//write to pid file 
FILE *proc_file_read;				//read from process status file
char *file;					//file to playing
int pid; 					//ID of current process
int test;					//universal testing variable
char *p_test;					//universal testing pointer

/* declaration of functions */
int play_track(int repeat);			//play one track
char* make_command(char *file);			//make command to play track
int search_extencion(char *file,char *ext);	//search extenction in file
char* create_command_mask(char *file,char*program);//create mask to play command
int arrlen(char **array);			//count arrays in multi-array
int stop_playing(int option);			//stop current playing
int scan_process_status(char status);		//get process status 
char *itoa(int number);				//convert intiger to string
//int play_in_background(char **);

int main(int argc, char **argv) {

  int arg_num,in_arg,repeat=false,play=false;
  char opt;

  /* play track in background */
  //play_in_background(argv);
  test=daemon(true,true);			
  if(test==error) {
    fprintf(stderr,"clamp: Daemon dummed");
    exit(EXIT_FAILURE);
  }

#ifdef PLAY
  test=play_track(repeat,argv[1]);
#endif

  /* read arguments */
  for(arg_num=1;arg_num<argc;arg_num++) {
    if((argv[arg_num][0])=='-') {
      for(in_arg=1;(opt=argv[arg_num][in_arg])!='\0';in_arg++) {

	switch(opt) {
	  case 'r' :
	    repeat=true;
	    break;
	  case 's' :
	    stop_playing(STOP);
	    exit(EXIT_SUCCESS);
	  case 'p' :
	    stop_playing(PAUSE);
	    exit(EXIT_SUCCESS);
	  default :
	    fprintf(stderr,"clamp: clamp: argument %c does not exist\n",opt);
	}
      }
    }
    else {

      /* playing one track */
      file=argv[arg_num];
      test=play_track(repeat);
      if(test == error) {
	fprintf(stderr,"clamp: can't read %s\n",argv[arg_num]);
	exit(EXIT_FAILURE);
      }
    }
  }
  
  /* delete temporary pid file */
  unlink(tmp_pid_file);

  exit(EXIT_SUCCESS);
}

int play_track(int repeat) {

  /* open temporary pid file to write */
  pid_file_write=fopen(tmp_pid_file,"w");
  if(pid_file_write==NULL) {
    fprintf(stderr,"clamp: Can't open pid file to write");
    exit(EXIT_FAILURE);
  }

  /* print current process ID and group process ID to pid file */
  test=fprintf(pid_file_write,"%d",pid=getpid()); 
  if(test==0) {
    fprintf(stderr,"clamp: can't write to tmp pid file\n");
    exit(EXIT_FAILURE);
  }

#ifdef DEBUG0 
  printf(" this is pid: %d\n",pid);
#endif
  
  test = fclose(pid_file_write);		//close pid file stream
  if(test!=0) {
    fprintf(stderr,"clamp: can't close tmp pid file\n");
    exit(EXIT_FAILURE);
  }

#ifdef DEBUG1
  printf("this is %s\n",make_command(file));
#else

  /* repeat and play one track */
  do {
     char *command;

     //execlp("mpg123","mpg123","-q",opt,0); 	//test command
     command=make_command(file);
     test=system(command);
     if(test==error)
       return error;
     wait(NULL);

  } while(repeat);

#endif

  return true;
}

int play_in_background(char **arg) {

  if(getppid()!=1) {

    int i,arglen;
    char *command;

    /* alloc command array */
    if((command=(char*)malloc(256))==NULL) {
      fprintf(stderr,"clamp: clamp: error in allocate memory\n");
      exit(EXIT_FAILURE);
    }

    arglen=arrlen(arg);				//coutnig arg array

    /* copy command of this program to command array */
    for(i=0;i<arglen;i++) {
      strcat(command,arg[i]);
      if(i>0 && i<arglen-1)
	strcat(command,"\" \"");
      else if(i==0)
	strcat(command," \"");
      else 
	strcat(command,"\" ");
    }

    strcat(command,"&");			//add char & to command array
    i=strlen(command);
    command[i]='\0';

#ifdef DEBUG3
    printf("%s\n",command);
#else
    system(command);				//run command in background
#endif

    exit(EXIT_SUCCESS);
  }

  return 1;
}

char* make_command(char *file) {
  char *command;

  if(strlen(file)>256) {			//file is very big
    fprintf(stderr,"clamp: %s is very big\n",file);
    exit(2);
  }

  if(search_extencion(file,"ogg")==true) 	//playing ogg
    command=create_command_mask(file,"ogg");		

  else if(search_extencion(file,"mp3")==true) 	//playing mp3
    command=create_command_mask(file,"mpg");	

  else {
    fprintf(stderr,"clamp: %s isn't ogg or mp3 file\n",file);
    exit(3);
  }

  return command;
}

int search_extencion(char *file,char *ext) {
  int i_file, i_ext, match=true;

  i_ext=strlen(ext);				//max number of extentcion	
  i_file=strlen(file);				//max number of filename

  while(file[i_file]!='.') 
    if(file[i_file--]!=ext[i_ext--])		//is not matching
      return(match=false); 

  return match;
}

char* create_command_mask(char* file, char *program) {
  char *command;
  command=(char*)malloc(280);

  /* program name is ogg or mp3 */
  p_test=strcpy(command,program);	//copy program name to command
  
  if(p_test == NULL) {
    fprintf(stderr,"clamp: Can't generate command from strcpy\n");
    return NULL;
  }

  p_test=strcat(command,"123 -q \"");	//copy rest of program name
  p_test=strcat(command,file);		//copy filename
  p_test=strcat(command,"\"");		//copy rest of command
 
  if(p_test == NULL) {
    fprintf(stderr,"clamp: Can't generate command from strcat\n");
    return NULL;
    
  }
  
  return command;
}

/* function to count lenth of multi-dimensional array */
int arrlen(char **array) {

  int count;

  /* counting */
  for(count=0;array[count]!=NULL;count++) 
    ;
  return count;
}

/* cancel and pause playing */
int stop_playing(int option) {

  /* open temporary pid file to read*/
  pid_file_read = fopen(tmp_pid_file,"r");
  if(pid_file_read == NULL) {
    fprintf(stderr,"clamp: can't open pid file to read\n");
    exit(EXIT_FAILURE);
  }

  /* read pid from temporary pid file */
  test = fscanf(pid_file_read, "%d", &pid);
  if(test == 0) {
    fprintf(stderr,"clamp: can't read from pid file\n");
    exit(EXIT_FAILURE);
  }

  /* close pid file */
  test = fclose(pid_file_read);
  if(test == EOF) {
    fprintf(stderr,"clamp: can't close pid file \n");
    exit(EXIT_FAILURE);
  }
  
#ifdef DEBUG5
  printf("%d\n",pid);
#endif

  /* sending signals */
  if(option == STOP) {				//stop playing
    test = killpg(pid, SIGTERM);
    if(test==error) {
      fprintf(stderr,"clamp: can't stop");
      exit(EXIT_FAILURE);
    }
    unlink(tmp_pid_file);
  }

#ifdef DEBUG6
  printf("%d\n",option);
#endif

  if(option == PAUSE) {				//pause playing
    //printf("this is test\n");
    test = scan_process_status('T');
    if(test == true) 
      test = killpg(pid, SIGCONT);
    else
      test = killpg(pid, SIGSTOP);
    if(test==error) {
      fprintf(stderr,"clamp: can't pause");
      exit(EXIT_FAILURE);
    }
  }

  return 1;
}

/* get process status from pid file */
int scan_process_status(char status) {

  char *proc_file, *spid, c;
  proc_file=(char*)malloc(256);

  /* convert pid to string pid */
  spid=itoa(pid);

#ifdef SPID
  printf("%s\n",spid);
#endif

  /* make full path of proc file  */
  strcpy(proc_file,"/proc/");
  strcat(proc_file,spid);
  strcat(proc_file,"/stat");

#ifdef PROC
  printf("%s\n",proc_file);
#endif

  /* open process status file to read*/
  proc_file_read = fopen(proc_file,"r");
  if(pid_file_read == NULL) {
    fprintf(stderr,"clamp: can't open process status file to read\n");
    exit(EXIT_FAILURE);
  }

  /* scaning proc file for status */
  while((c=getc(proc_file_read)) != EOF) {
#ifdef PUT
    putchar(c);
#endif
    if(c == status)
      return true;
  }

  return false; 
}


/* convert intiger to string */
char *itoa(int number) {
  char *str;
  int rest,num[10],i=0,j;
  str=(char*)malloc(10);

  while(number>=1) {
    rest=number%10;
    number=number/10;
    num[i++]=rest;
  }

  for(j=0;i>=0;j++)
    str[j]=num[--i]+'0';
  str[j-1]='\0';

  for(i=0,j=0;str[i]!='\0';i++)
    if(str[i]!='\n' && str[i]!='\t' && str[i]!=' ')
      str[j++]=str[i];

  return str;
}

