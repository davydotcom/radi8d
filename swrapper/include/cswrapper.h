/*
cSWrapper Library Is A Class Interface for the Swrapper Functions
*/
#ifndef CSWRAPPER_H
#define CSWRAPPER_H
#include"swrapper.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <map>

class cswrapper
{
private:
	SSL_CTX *ssl_ctx;
	std::map<int, SSL*> ssl_sessions;
	bool useSSL;
	char *cert_path;
	char *key_path;
	bool using_self_signed;
	
	int generate_self_signed_cert();
	void cleanup_ssl();
	
public:
	char *TheData;
	int cs_connect(char *host,int port);
	int cs_serve(int port);
	int cs_write(int sfd, char *str,int len);
	int cs_read(int sfd,char *str, int len);
	
	// SSL methods
	int init_ssl(const char *cert_file, const char *key_file);
	int cs_accept_ssl(int listen_sfd, int client_sfd);
	void cs_close_ssl(int sfd);
	bool is_ssl_enabled() { return useSSL; }
	
	cswrapper()
	{
		TheData = new char[1024];
		ssl_ctx = NULL;
		useSSL = false;
		cert_path = NULL;
		key_path = NULL;
		using_self_signed = false;
	};
	~cswrapper()
	{
		delete [] TheData;
		cleanup_ssl();
		if(cert_path) delete [] cert_path;
		if(key_path) delete [] key_path;
	}

};

#endif
