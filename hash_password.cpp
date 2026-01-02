#include <iostream>
#include <cstring>
#include <openssl/sha.h>

using namespace std;

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		cout << "Usage: " << argv[0] << " <password>" << endl;
		cout << "Generates a SHA-256 hash for the given password." << endl;
		return 1;
	}
	
	char *password = argv[1];
	
	// Hash the password using SHA-256
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256((unsigned char*)password, strlen(password), hash);
	
	// Convert hash to hex string
	char hexHash[SHA256_DIGEST_LENGTH * 2 + 1];
	memset(hexHash, 0, SHA256_DIGEST_LENGTH * 2 + 1);
	
	for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(hexHash + (i * 2), "%02x", hash[i]);
	}
	
	cout << hexHash << endl;
	
	return 0;
}
