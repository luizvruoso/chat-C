#define MAX 1024
#define FILE_NAME  "bd.txt"
#define TEMP_FILE_NAME  "temp.txt"
int searchUser(char *username) {

	char buff[MAX] = {};
	char aux[MAX] ={'\0'};
	//read(sockfd, (char *) &userMessage, sizeof(userMessage));
	
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
			valueAfterEquals(aux, buff, sizeof(buff));
			if(strcmp(aux, username ) == 0 ) {
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
			valueAfterEquals(aux, buff, sizeof(buff));

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

void valueAfterEquals(char * destiny,char * buffer, int n){

	
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