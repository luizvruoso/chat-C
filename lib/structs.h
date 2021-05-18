struct {
	int	statusCode;
	char 	message[1024];
} typedef serverResponse;

struct{
	char content[1024];
	int nBytes;
}typedef charContentOps;


struct {
	int	operation;
	charContentOps username;
	charContentOps message;
	int myPortNumber;
	charContentOps userDestiny;
} typedef msg;



struct {

	int  * socket;
	char username[1024];

}typedef user;