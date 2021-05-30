#define MAX 1024
#define FILE_NAME  "bd.txt"
#define TEMP_FILE_NAME  "temp.txt"
#define MESSAGES_FILE_NAME "messages.txt"

void setAllOffline(){
    FILE * file;
    FILE * tempFile;

    file = fopen(FILE_NAME, "a+");
    tempFile = fopen(TEMP_FILE_NAME, "w");

    char buff[MAX] = {'\0'};
    char aux[MAX]  = {'\0'};

    if(file == NULL) {
        fprintf(stderr, "Could not open input file\n");
        exit(1);
    }

    if(tempFile == NULL) {
        fprintf(stderr, "Could not open temp file\n");
        exit(1);
    }

	int i = 0;
	
	bzero(aux, sizeof(aux));
	bzero(buff, sizeof(buff));
	
	while(!feof(file)) {
		bzero(buff, sizeof(buff));

		fgets(buff, 1024, file);
        fprintf(tempFile, "%s", buff);
		if(strlen(buff) >=1){
        	fprintf(tempFile, "status=OFFLINE;\n", buff);
		}
		fgets(buff, 1024, file);

		i++;
	}

   	fclose(file);
    fclose(tempFile);
    remove(FILE_NAME); 
    rename(TEMP_FILE_NAME, FILE_NAME);
}
int searchUser(char *username) {

	char buff[MAX] = {};
	char aux[MAX] ={'\0'};
	//read(sockfd, (char *) &userMessage, sizeof(userMessage));
	int lenUsername = sizeof(username);
	FILE * file;
	file = fopen(FILE_NAME, "r");
	

	if(file == NULL) {
        	fprintf(stderr, "Could not open input file\n");
        	exit(1);
    	}

	int i = 0;
	fgets(buff, 1024, file);
	

	while(!feof(file)) {
		if( i % 2 == 0 ) {
			bzero(aux, sizeof(aux));
			valueAfterEquals(aux, buff);
			//printf("aux %s usernmae: %s \n\n\n", aux, username);
			if(strcmp(aux, username) == 0 ) {
				fclose(file);
				return 0;
			}
		}
		

		fgets(buff, 1024, file);
		i++;
	}
	
	fclose(file);
	return 1;
}
int numberOfLines() {
    FILE *file;
    file = fopen(FILE_NAME, "r");
    int lines = 0;
    char ch;

    while(!feof(file)) {
  	ch = fgetc(file);

  	if(ch == '\n') 
    		lines++;
  	
    }

	fclose(file);
	printf("Linhas: %d\n", lines);
	return lines;
}
void printContactList(int count, char * message ) {
    FILE *file;

    file = fopen(FILE_NAME,"r");
    int i = 0;

    char line[MAX] = {'\0'};
    char destiny[MAX] = {'\0'};
    char aux[MAX] = "User ";
    char secAux[MAX] = " is ";
    //char *message;

    //message = malloc (sizeof (char) * 40);
    bzero(message, sizeof(message));

    if(file == NULL) {
        fprintf(stderr, "Could not open input file\n");
        exit(1);
    }

    while(!feof(file)) {
	
		if(count == i) {
			
			bzero(destiny, sizeof(destiny));

			fgets(line, sizeof(line), file);
			valueAfterEquals(destiny, line);
			strcat(aux, destiny);

			bzero(destiny, sizeof(destiny));
			
			fgets(line, sizeof(line), file);
			valueAfterEquals(destiny, line);

			strcat(aux, secAux);
			strcat(aux, destiny);
			strcat(message, aux);

			fclose(file);
			return;
		}

		fgets(line, sizeof(line), file);	
		fgets(line, sizeof(line), file);
		i++;
	}

	return ;
}
void printStatusAtRightPosition(char *username, char *status) {

    FILE * file;
    FILE * tempFile;

    file = fopen(FILE_NAME, "a+");
    tempFile = fopen(TEMP_FILE_NAME, "w");

    char buff[MAX] = {'\0'};
    char aux[MAX]  = {'\0'};

    if(file == NULL) {
        fprintf(stderr, "Could not open input file\n");
        exit(1);
    }

    if(tempFile == NULL) {
        fprintf(stderr, "Could not open temp file\n");
        exit(1);
    }

	int i = 0;
	fgets(buff, 1024, file);
	
	while(!feof(file)) {
		if( i % 2 == 0 ) {
            		bzero(aux, sizeof(aux));
			valueAfterEquals(aux, buff);

			if( strcmp(aux, username ) == 0 ) {
                		//encontrei o usuario
               	 		fprintf(tempFile, "username=%s;\n", username);
                		fprintf(tempFile, "status=%s;\n", status);
		        	fgets(buff, 1024, file);
                		i++;
			} else
                		fprintf(tempFile, "%s", buff);
		} else 
            		fprintf(tempFile, "%s", buff);
       	 	

		fgets(buff, 1024, file);
		i++;
	}

   	fclose(file);
    	fclose(tempFile);
    	remove(FILE_NAME); 
    	rename(TEMP_FILE_NAME, FILE_NAME);
}
int isUserOnline(char *username) {

    FILE *file;
    file = fopen(FILE_NAME, "r");

    if(file == NULL) {
        fprintf(stderr, "Could not open input file\n");
        exit(1);
    }

    char line[MAX] = {'\0'};
    char auxStr[MAX] = {'\0'};
    char usernameCompare[MAX] = {'\0'};

    while(!feof(file)) {

	fgets(line, sizeof(line), file);
        strcpy(auxStr, line);
        sscanf(auxStr, "%*[^=]=%[^;]", usernameCompare); 

        if (strcmp(usernameCompare, username) == 0) {
	
            fgets(line, sizeof(line), file);
            sscanf(line, "%*[^=]=%[^;]", usernameCompare);

            if(strcmp(usernameCompare, "OFFLINE") == 0) {
		    fclose(file);
		    return 1;
	    }  
        }
    }

    fclose(file);
    return 0;
}
int isEqualsToUsername(char *username, int n){

	char aux[n];
	int k = 0;

	//copio ate o igual
	while(username[k] != '='){ 
		aux[k] = username[k]; 
		k++;
	}
	
	//pego o pre fixo copiado e comparo com o conteudo que desejo encontrar
	if(strcmp(aux, "username")) return 1;
	else return -1;
}
void valueAfterEquals(char * destiny, char * buffer){

	int k = 0;
	while(buffer[k] != '='){ //encontrar o fim do igual
		k++;
	}

	int i = 0;
	k++; //pular o '='
	while(buffer[k] != ';'){
		destiny[i] = buffer[k]; 
		k++;
		i++;
	}
}
void printAtEndOfFile(char *buff,  char *userStatus){

	FILE * file;
	file = fopen(FILE_NAME, "a+");

	fprintf(file, "username=%s;\n", buff);
	fprintf(file, "status=%s;\n", userStatus);

	fclose(file);
}
void getMessage(char *username, char *result) {

    FILE *file;
    file = fopen(MESSAGES_FILE_NAME, "r");
    char message[MAX];

    if(file == NULL) {
        fprintf(stderr, "Could not open input file\n");
        exit(1);
    }

    char line[MAX] = {'\0'};
    char auxStr[MAX] = {'\0'};
    char usernameCompare[MAX] = {'\0'};
    int i = 0;
    while(!feof(file)) {

        fgets(line, sizeof(line), file);
        strcpy(auxStr, line);
        sscanf(auxStr, "%*[^=]=%[^;]", usernameCompare); 

        if (strcmp(usernameCompare, username) == 0) {
            	i++;
            	fgets(line, sizeof(line), file);
            	sscanf(line, "%*[^=]=%[^;]", usernameCompare);
	        	strcpy(result, usernameCompare);
				fclose(file);

            	deleteLineFromFile(i);
            	deleteLineFromFile(i - 1);	

		return message;
        }else 
		i++;
    }

    fclose(file);
    return NULL;
}
void deleteLineFromFile(int lineNumber) {

    FILE *fileptr1, *fileptr2;
    char line[MAX] = {'\0'};

    fileptr1 = fopen(MESSAGES_FILE_NAME, "r");

    fseek(fileptr1, 0, SEEK_SET);

    fileptr2 = fopen("replica.txt", "w");

    int i = 0;
    while(!feof(fileptr1)) {
		bzero(line, sizeof(line));

		if(i == lineNumber){
			fgets(line, sizeof(line), fileptr1);
			printf("dve ser excluida %d \n\n", i);
		} else {
			fgets(line, sizeof(line), fileptr1);
			fprintf(fileptr2, "%s", line);
			printf("line: %s %d\n\n", line, i);
		}	
		
		i++;
    }
	
    fclose(fileptr1);
    fclose(fileptr2);

    remove(MESSAGES_FILE_NAME);
    rename("replica.txt", MESSAGES_FILE_NAME);
}
int hasMessage(char *username) {

    FILE *file;
    file = fopen(MESSAGES_FILE_NAME, "r");
    char *message;
    message = malloc (sizeof (char) * 15);

    if(file == NULL) {
        fprintf(stderr, "Could not open input file\n");
        exit(1);
    }

    char line[MAX] = {'\0'};
    char auxStr[MAX] = {'\0'};
    char usernameCompare[MAX] = {'\0'};
    int i = 0;

    while(!feof(file)) {
	bzero(auxStr, sizeof(auxStr));
	fgets(line, sizeof(line), file);
        strcpy(auxStr, line);
        sscanf(auxStr, "%*[^=]=%[^;]", usernameCompare); 

        if (strcmp(usernameCompare, username) == 0) {
		fclose(file);
		return 1;
        }

	i++;
    }

    fclose(file);
    return 0;
}