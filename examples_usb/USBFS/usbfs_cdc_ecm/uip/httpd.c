/**
 * \addtogroup exampleapps
 * @{
 */

/**
 * \defgroup httpd Web server
 * @{
 *
 * The uIP web server is a very simplistic implementation of an HTTP
 * server. It can serve web pages and files from a read-only ROM
 * filesystem, and provides a very small scripting language.
 *
 * The script language is very simple and works as follows. Each
 * script line starts with a command character, either "i", "t", "c",
 * "#" or ".".  The "i" command tells the script interpreter to
 * "include" a file from the virtual file system and output it to the
 * web browser. The "t" command should be followed by a line of text
 * that is to be output to the browser. The "c" command is used to
 * call one of the C functions from the httpd-cgi.c file. A line that
 * starts with a "#" is ignored (i.e., the "#" denotes a comment), and
 * the "." denotes the last script line.
 *
 * The script that produces the file statistics page looks somewhat
 * like this:
 *
 \code
i /header.html
t <h1>File statistics</h1><br><table width="100%">
t <tr><td><a href="/index.html">/index.html</a></td><td>
c a /index.html
t </td></tr> <tr><td><a href="/cgi/files">/cgi/files</a></td><td>
c a /cgi/files
t </td></tr> <tr><td><a href="/cgi/tcp">/cgi/tcp</a></td><td>
c a /cgi/tcp
t </td></tr> <tr><td><a href="/404.html">/404.html</a></td><td>
c a /404.html
t </td></tr></table>
i /footer.plain
.
 \endcode
 *
 */


/**
 * \file
 * HTTP server.
 * \author Adam Dunkels <adam@dunkels.com>
 */

/*
 * Copyright (c) 2001, Adam Dunkels.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.  
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: httpd.c,v 1.28.2.6 2003/10/07 13:22:27 adam Exp $
 *
 */


#include "uip.h"
#include "httpd.h"
#include "fs.h"
#include "fsdata.h"
#include "stddef.h"


/* The HTTP server states: */
#define HTTP_NOGET        0
#define HTTP_FILE         1
#define HTTP_TEXT         2
#define HTTP_FUNC         3

#if DEBUG
#include <stdio.h>
#define PRINT(x) printf("%s", x)
#define PRINTLN(x) printf("%s\n", x)
#else /* DEBUG */
#define PRINT(x)
#define PRINTLN(x)
#endif /* DEBUG */

struct httpd_state *hs;

extern const struct fsdata_file file_index_html;
extern const struct fsdata_file file_404_html;

__attribute__((weak))
int uip_api_handler(const char *endpoint, char **data, int *len)
{
   (void)endpoint;
   (void)data;
   (void)len;
   return 0;
}

/*-----------------------------------------------------------------------------------*/
/**
 * Initialize the web server.
 *
 * Starts to listen for incoming connection requests on TCP port 80.
 */
/*-----------------------------------------------------------------------------------*/
void
httpd_init(void)
{
  fs_init();
  
  /* Listen to port 80. */
  uip_listen(HTONS(80));
}
/*-----------------------------------------------------------------------------------*/
void
httpd_appcall(void)
{
  struct fs_file fsfile;  

  u8_t i;

  switch(uip_conn->lport) {
    /* This is the web server: */
  case HTONS(80):
    /* Pick out the application state from the uip_conn structure. */
    hs = (struct httpd_state *)(uip_conn->appstate);

    /* We use the uip_ test functions to deduce why we were
       called. If uip_connected() is non-zero, we were called
       because a remote host has connected to us. If
       uip_newdata() is non-zero, we were called because the
       remote host has sent us new data, and if uip_acked() is
       non-zero, the remote host has acknowledged the data we
       previously sent to it. */
    if(uip_connected()) {
      /* Since we have just been connected with the remote host, we
         reset the state for this connection. The ->count variable
         contains the amount of data that is yet to be sent to the
         remote host, and the ->state is set to HTTP_NOGET to signal
         that we haven't received any HTTP GET request for this
         connection yet. */
      hs->state = HTTP_NOGET;
      hs->count = 0;
      return;

    } else if(uip_poll()) {
      /* If we are polled ten times, we abort the connection. This is
         because we don't want connections lingering indefinately in
         the system. */
       if(hs->count++ >= 10) {
          uip_abort();
       }
      return;
    } else if(uip_newdata() && hs->state == HTTP_NOGET) {
      /* This is the first data we receive, and it should contain a GET. */
      
      /* Check for GET. This produces smaller code than strncmp*/
       if(uip_appdata[0] != 'G' ||
          uip_appdata[1] != 'E' ||
          uip_appdata[2] != 'T' ||
          uip_appdata[3] != ' ')
       {
          /* If it isn't a GET, we abort the connection. */
          uip_abort();
          return;
       }

       // check if host is captive portal
       char *c = (char *)uip_appdata;
       const char *end = (char *)uip_appdata + uip_len;
       while(c + 5 < end)
       {
         if((*c == 'H' || *c == 'h') &&
            (*(c + 1) == 'o' || *(c + 1) == 'O') &&
            (*(c + 2) == 's' || *(c + 2) == 'S') &&
            (*(c + 3) == 't' || *(c + 3) == 'T') &&
            (*(c + 4) == ':'))
         {
           c += 6; // skip "Host: "
           // check if host is not an IP address
           if((*c < '0' || *c > '9') &&
              *c != '[') // IPv6 start
           {
             PRINTLN("Captive portal redirect");
             // serve captive portal page
             static const char captive_portal_html[] =
               "HTTP/1.0 302 Found\r\n"
               "Location: http://172.16.42.1/\r\n"
               "Content-Length: 0\r\n"
               "Connection: close\r\n"
               "\r\n";
             hs->script = NULL;
             hs->state = HTTP_FILE;
             hs->dataptr = (char *)captive_portal_html;
             hs->count = sizeof(captive_portal_html) - 1;
             goto redirect;
           }
         }
         ++c;
       }
	       
      /* Find the file we are looking for. */
      for(i = 4; i < 40; ++i) {
         if(uip_appdata[i] == ' ' ||
            uip_appdata[i] == '\r' ||
            uip_appdata[i] == '\n')
         {
            uip_appdata[i] = 0;
            break;
         }
      }

      PRINT("request for file ");
      PRINTLN(&uip_appdata[4]);
      
      /* Check for a request for "/". */
      if(uip_appdata[4] == '/' &&
         uip_appdata[5] == 0)
      {
         fs_open(file_index_html.name, &fsfile);    
      } else if(uip_appdata[4] == '/' &&
                uip_appdata[5] == 'a' &&
                uip_appdata[6] == 'p' &&
                uip_appdata[7] == 'i' &&
                uip_appdata[8] == '/' )
      {
         if(!uip_api_handler((char *)&uip_appdata[9], &fsfile.data, &fsfile.len))
         {
            PRINTLN("API handler failed");
            fs_open(file_404_html.name, &fsfile);
         }
      } else {
         if(!fs_open((const char *)&uip_appdata[4], &fsfile)) {
            PRINTLN("couldn't open file");
            fs_open(file_404_html.name, &fsfile);
         }
      } 

      hs->script = NULL;
      /* The web server is now no longer in the HTTP_NOGET state, but
         in the HTTP_FILE state since is has now got the GET from
         the client and will start transmitting the file. */
      hs->state = HTTP_FILE;

      /* Point the file pointers in the connection state to point to
         the first byte of the file. */
      hs->dataptr = fsfile.data;
      hs->count = fsfile.len;	
redirect:
    }

    
    /* Check if the client (remote end) has acknowledged any data that
       we've previously sent. If so, we move the file pointer further
       into the file and send back more data. If we are out of data to
       send, we close the connection. */
    if(uip_acked()) {
       if(hs->count >= uip_conn->len) {
          hs->count -= uip_conn->len;
          hs->dataptr += uip_conn->len;
       } else {
          hs->count = 0;
       }

       if(hs->count == 0) {
          uip_close();
       }
    }         

    if(!uip_poll()) {
      /* Send a piece of data, but not more than the MSS of the
	 connection. */
      uip_send(hs->dataptr, hs->count);
    }

    /* Finally, return to uIP. Our outgoing packet will soon be on its
       way... */
    return;

  default:
    /* Should never happen. */
    uip_abort();
    break;
  }  
}
/*-----------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
/** @} */
/** @} */
