struct {
	int	statusCode;
	char 	message[1024];
} typedef serverResponse;

struct {
	int	operation;
	char 	username[1024];
	char 	message[1024];
	int 	myPortNumber;
	char 	userDestiny[1024];
} typedef msg;