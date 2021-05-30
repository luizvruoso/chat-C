
struct{
	char content[1024];
	int nBytes;
}typedef charContentOps;

struct {
	int	operation;
	charContentOps username;
	charContentOps message;
	char ip[15];
	charContentOps userDestiny;
} typedef msg;

struct {
	int	statusCode;
	int operation;
	//char message[1024];
	msg payload;
} typedef serverResponse;

struct {

	int  *socket;
	char username[1024];
	int portNumber;
	char ip[15];
	
}typedef user;


struct{
	char nameFile[40];
	long long blockSize;

}typedef fileTransfer;


struct{
	int i;
	char message[1024];
	char from[1024];
}typedef messageReceived;
