#define BYTESBETWEENMETA 16000
#define NETBUFFSIZE 4000

#define SHOUTSERVMSG "ICY 200 OK\r\n\
icy-notice1:This stream is served using Ample/%s\r\n\
icy-notice2:AMPLE - An MP3 LEnder - http://ample.sf.net/\r\n\
icy-name:%s\r\n\
icy-genre:Mixed\r\n\
icy-url:http://ample.sf.net/\r\n\
icy-pub:1\r\n\
icy-metaint:%d\r\n\
icy-br:128\r\n\
\r\n"

#define SINGLESERVMSG "HTTP/1.1 200 OK\r\n\
Server: Ample/%s\r\n\
Accept-Range: none\r\n\
Content-Type: audio/mpeg\r\n\
Content-Length: %lu\r\n\
Connection: close\r\n\
\r\n"

#define BASICSERVMSG "HTTP/1.1 200 OK\r\n\
Server: Ample/%s\r\n\
Accept-Range: none\r\n\
Content-Type: audio/mpeg\r\n\
Connection: close\r\n\
\r\n"

#define HTTPSERVMSG "HTTP/1.1 200 OK\r\n\
Server: Ample/%s\r\n\
Accept-Range: none\r\n\
Content-Type: text/html\r\n\
Connection: close\r\n\
\r\n"

extern int handleclient(int conn);
