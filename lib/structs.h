
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
	int	statusCode;
	int operation;
	//char message[1024];
	msg payload;
} typedef serverResponse;


struct {

	int  * socket;
	char username[1024];

}typedef user;