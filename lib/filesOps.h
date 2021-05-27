#define MAX 1024
#define FILE_NAME  "bd.txt"
#define TEMP_FILE_NAME  "temp.txt"

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

void printContactList() {
	FILE *f;

	f = fopen(FILE_NAME,"r");
	int i = 0;

    char line[MAX] = {'\0'};
    char destiny[MAX] = {'\0'};

    while(!feof(f)) {

		bzero(destiny, sizeof(destiny));

		fgets(line, sizeof(line), f);
      	valueAfterEquals(destiny, line);
		printf("User: %s ", destiny);
		
		bzero(destiny, sizeof(destiny));

		fgets(line, sizeof(line), f);
		valueAfterEquals(destiny, line);
		printf("is: %s \n", destiny);
	}
}

void printStatusAtRightPosition(char *username, char *status){
    FILE * file;
    FILE * tempFile;
    file = fopen(FILE_NAME, "a+");
    tempFile = fopen(TEMP_FILE_NAME, "w");

	char buff[MAX] = {0};
    char aux[MAX] = {'\0'};
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
			}else{
                //printf("errad %s %d\n", aux, i);

                fprintf(tempFile, "%s", buff);
            }
		}else{
            fprintf(tempFile, "%s", buff);
        }
		fgets(buff, 1024, file);
		i++;
	}

   fclose(file);
    fclose(tempFile);
    remove(FILE_NAME);  		// remove the original file 
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

        strcpy(auxStr, fgets(line, sizeof(line), file));
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
	while(username[k] != '='){ //copio ate o igual
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
	//printf("buffer k %s\n", buffer);
	//printf("buffer at k %c\n", buffer[k]);

	int i = 0;
	k++; //pular o '='
	while(buffer[k] != ';'){
		destiny[i] = buffer[k]; 
		k++;
		i++;
	}
	//printf("aux31321  %s\n", destiny);
}

void printAtEndOfFile(char *buff,  char *userStatus){
	FILE * file;
	file = fopen(FILE_NAME, "a+");
	char c;
	/*c = fgetc( file );
	printf("\ncu nostro %c\n", c);*/
	/*while(1) {

		c = fgetc( file ); // reading the file
		if(feof(file)){
			printf("\ncu nostro %c\n", c);
			break;
		};
		//printf ( "%c", c ) ;

	}*/

	fprintf(file, "username=%s;\n", buff);
	fprintf(file, "status=%s;\n", userStatus);

	fclose(file);
}

char *getMessage(char *username) {
    FILE *file;
    file = fopen("messages.txt", "r");
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

        strcpy(auxStr, fgets(line, sizeof(line), file));
        sscanf(auxStr, "%*[^=]=%[^;]", usernameCompare); 

        if (strcmp(usernameCompare, username) == 0) {
            i++;
            fgets(line, sizeof(line), file);
            sscanf(line, "%*[^=]=%[^;]", usernameCompare);
	        strcpy(message, usernameCompare);
			fclose(file);
            deleteLineFromFile(i);
            //printf("\nLINHA: %d", i);
            deleteLineFromFile(i - 1);
            //printf("\nLINHA2: %d", i-1);
			

	        return message;
        }else{
			i++;
		}
        printf("PASSOU\n");
        printf("PASSOU3: %d\n", i);
    }
	fclose(file);

    return "false";
}
void deleteLineFromFile(int lineNumber) {

    FILE *fileptr1, *fileptr2;
    char line[MAX] = {'\0'};
    //open file in read mode
    fileptr1 = fopen("messages.txt", "r");

    //rewind(fileptr1);
    fseek(fileptr1, 0, SEEK_SET);


    fileptr2 = fopen("replica.txt", "w");
    //ch = getc(fileptr1);
    int i = 0;

    while(!feof(fileptr1)) {
		bzero(line, sizeof(line));
		if(i == lineNumber){
			fgets(line, sizeof(line), fileptr1);
			printf("dve ser excluida %d \n\n", i);
		}else{
			fgets(line, sizeof(line), fileptr1);
			fprintf(fileptr2, "%s", line);
			printf("line: %s %d\n\n", line, i);
		}	
		
		i++;
    }
	
    fclose(fileptr1);
    fclose(fileptr2);
    remove("messages.txt");
    rename("replica.txt", "messages.txt");
	
   /* fileptr1 = fopen("messages.txt", "r");
   
    fclose(fileptr1);*/
}


int hasMessage(char *username) {
    FILE *file;
    file = fopen("messages.txt", "r");
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