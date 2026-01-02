/*
cswrapper Source File
*/
#include"cswrapper.h"
#include <unistd.h>

int cswrapper::cs_connect(char *host,int port)
{
	return s_connect(host,port);
}

int cswrapper::cs_serve(int port)
{
	return s_serve(port);
}

int cswrapper::cs_write(int sfd, char *str,int len)
{
	if(str == NULL)
		return 0;
	if(len <= 0)
		return 0;
	
	// Check if this socket is using SSL
	if(useSSL && ssl_sessions.find(sfd) != ssl_sessions.end())
	{
		SSL *ssl = ssl_sessions[sfd];
		int bytes = SSL_write(ssl, str, len);
		
		// Flush SSL buffer to ensure data is sent immediately
		if(bytes > 0)
		{
			BIO_flush(SSL_get_wbio(ssl));
		}
		
		return bytes;
	}
	
	// Write directly using the provided length
	return s_write(sfd, str, len);
}

int cswrapper::cs_read(int sfd,char *str, int len)
{
	// Check if this socket is using SSL
	if(useSSL && ssl_sessions.find(sfd) != ssl_sessions.end())
	{
		SSL *ssl = ssl_sessions[sfd];
		int bytes = SSL_read(ssl, str, len);
		
		return bytes;
	}
	
	return read(sfd,str,len);
}

int cswrapper::generate_self_signed_cert()
{
	printf("Generating self-signed certificate...\n");
	
	// Generate RSA key
	EVP_PKEY *pkey = EVP_PKEY_new();
	if(!pkey) return -1;
	
	RSA *rsa = RSA_generate_key(2048, RSA_F4, NULL, NULL);
	if(!rsa)
	{
		EVP_PKEY_free(pkey);
		return -1;
	}
	
	if(!EVP_PKEY_assign_RSA(pkey, rsa))
	{
		RSA_free(rsa);
		EVP_PKEY_free(pkey);
		return -1;
	}
	
	// Create X509 certificate
	X509 *x509 = X509_new();
	if(!x509)
	{
		EVP_PKEY_free(pkey);
		return -1;
	}
	
	ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);
	X509_gmtime_adj(X509_get_notBefore(x509), 0);
	X509_gmtime_adj(X509_get_notAfter(x509), 31536000L); // 1 year
	
	X509_set_pubkey(x509, pkey);
	
	X509_NAME *name = X509_get_subject_name(x509);
	X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char *)"US", -1, -1, 0);
	X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (unsigned char *)"radi8d", -1, -1, 0);
	X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char *)"localhost", -1, -1, 0);
	X509_set_issuer_name(x509, name);
	
	if(!X509_sign(x509, pkey, EVP_sha256()))
	{
		X509_free(x509);
		EVP_PKEY_free(pkey);
		return -1;
	}
	
	// Write certificate and key to memory files
	FILE *cert_file = fopen("radi8d_self_signed.crt", "wb");
	if(!cert_file)
	{
		X509_free(x509);
		EVP_PKEY_free(pkey);
		return -1;
	}
	PEM_write_X509(cert_file, x509);
	fclose(cert_file);
	
	FILE *key_file = fopen("radi8d_self_signed.key", "wb");
	if(!key_file)
	{
		X509_free(x509);
		EVP_PKEY_free(pkey);
		return -1;
	}
	PEM_write_PrivateKey(key_file, pkey, NULL, NULL, 0, NULL, NULL);
	fclose(key_file);
	
	cert_path = new char[256];
	key_path = new char[256];
	strcpy(cert_path, "radi8d_self_signed.crt");
	strcpy(key_path, "radi8d_self_signed.key");
	
	X509_free(x509);
	EVP_PKEY_free(pkey);
	
	printf("Self-signed certificate generated: %s\n", cert_path);
	printf("Private key generated: %s\n", key_path);
	using_self_signed = true;
	
	return 0;
}

int cswrapper::init_ssl(const char *cert_file, const char *key_file)
{
	SSL_library_init();
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();
	
	ssl_ctx = SSL_CTX_new(TLS_server_method());
	if(!ssl_ctx)
	{
		ERR_print_errors_fp(stderr);
		return -1;
	}
	
	// If no certificate provided, generate self-signed
	if(!cert_file || !key_file)
	{
		if(generate_self_signed_cert() < 0)
		{
			fprintf(stderr, "Failed to generate self-signed certificate\n");
			SSL_CTX_free(ssl_ctx);
			ssl_ctx = NULL;
			return -1;
		}
		cert_file = cert_path;
		key_file = key_path;
	}
	else
	{
		cert_path = new char[strlen(cert_file) + 1];
		key_path = new char[strlen(key_file) + 1];
		strcpy(cert_path, cert_file);
		strcpy(key_path, key_file);
	}
	
	if(SSL_CTX_use_certificate_file(ssl_ctx, cert_file, SSL_FILETYPE_PEM) <= 0)
	{
		ERR_print_errors_fp(stderr);
		SSL_CTX_free(ssl_ctx);
		ssl_ctx = NULL;
		return -1;
	}
	
	if(SSL_CTX_use_PrivateKey_file(ssl_ctx, key_file, SSL_FILETYPE_PEM) <= 0)
	{
		ERR_print_errors_fp(stderr);
		SSL_CTX_free(ssl_ctx);
		ssl_ctx = NULL;
		return -1;
	}
	
	if(!SSL_CTX_check_private_key(ssl_ctx))
	{
		fprintf(stderr, "Private key does not match certificate\n");
		SSL_CTX_free(ssl_ctx);
		ssl_ctx = NULL;
		return -1;
	}
	
	useSSL = true;
	printf("SSL initialized successfully\n");
	printf("Using certificate: %s\n", cert_file);
	printf("Using key: %s\n", key_file);
	
	return 0;
}

int cswrapper::cs_accept_ssl(int listen_sfd, int client_sfd)
{
	if(!useSSL || !ssl_ctx)
		return 0;
	
	SSL *ssl = SSL_new(ssl_ctx);
	if(!ssl)
	{
		ERR_print_errors_fp(stderr);
		return -1;
	}
	
	SSL_set_fd(ssl, client_sfd);
	
	if(SSL_accept(ssl) <= 0)
	{
		ERR_print_errors_fp(stderr);
		SSL_free(ssl);
		return -1;
	}
	
	ssl_sessions[client_sfd] = ssl;
	printf("SSL connection established on socket %d\n", client_sfd);
	
	return 0;
}

void cswrapper::cs_close_ssl(int sfd)
{
	if(ssl_sessions.find(sfd) != ssl_sessions.end())
	{
		SSL *ssl = ssl_sessions[sfd];
		SSL_shutdown(ssl);
		SSL_free(ssl);
		ssl_sessions.erase(sfd);
	}
}

void cswrapper::cleanup_ssl()
{
	// Close all SSL sessions
	for(std::map<int, SSL*>::iterator it = ssl_sessions.begin(); it != ssl_sessions.end(); ++it)
	{
		SSL_shutdown(it->second);
		SSL_free(it->second);
	}
	ssl_sessions.clear();
	
	if(ssl_ctx)
	{
		SSL_CTX_free(ssl_ctx);
		ssl_ctx = NULL;
	}
	
	// Remove self-signed certificate files if they were generated
	if(using_self_signed)
	{
		unlink("radi8d_self_signed.crt");
		unlink("radi8d_self_signed.key");
	}
	
	EVP_cleanup();
	ERR_free_strings();
}

