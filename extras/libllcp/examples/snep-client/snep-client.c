/*-
 * Copyright (C) 2011, Romain Tartière
 * Copyright (C) 2013, JiapengLi
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

/*
 * This implementation was written based on information provided by the
 * following documents:
 *
 * NFC Forum SNEP and LLCP specfication
 * 
 * Version 1 - 2013-12-04 - Modified By JiapengLi<gapleehit@gmail.com>
 *
 * http://www.nfc-forum.org/specs/
 * 
 */

/*
 * $Id$
 */

#include "config.h"

#include <err.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <llcp.h>
#include <llc_service.h>
#include <llc_link.h>
#include <mac.h>
#include <llc_connection.h>

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct snepBuf{
	char * buffer;
	int length;
}snepBuf;

 struct mac_link *mac_link;
 nfc_device *device;

char* read_file(const char* filename, size_t* length)
{
  int fd;
  struct stat file_info;
  char* buffer;

  /* Open the file.  */
  fd = open (filename, O_RDONLY);

  /* Get information about the file.  */
  fstat (fd, &file_info);
  *length = file_info.st_size;
  /* Make sure the file is an ordinary file.  */
  if (!S_ISREG (file_info.st_mode)) {
    /* It's not, so give up.  */
    close (fd);
    return NULL;
  }

  /* Allocate a buffer large enough to hold the file's contents.  */
  buffer = (char*) malloc (*length);
  /* Read the file into the buffer.  */
  read (fd, buffer, *length);

  /* Finish up.  */
  close (fd);
  return buffer;
}

static void
stop_mac_link(int sig)
 {
  (void) sig;

  if (mac_link && mac_link->device)
    nfc_abort_command(mac_link->device);
}

static void
bye(void)
{
  if (device)
    nfc_close(device);
}

static size_t
shexdump(char *dest, const uint8_t *buf, const size_t size)
{
  size_t res = 0;
  for (size_t s = 0; s < size; s++) {
    sprintf(dest + res, "%02x  ", *(buf + s));
    res += 4;
  }
  return res;
}

static void printhex(char *s, uint8_t *buf, int len)
{
  printf("%s", s);
  int i;
  for(i=0; i<len; i++){
    printf("%02X ", buf[i]);
  }
  printf("\n");
}

static void
print_usage(char *progname)
{
  fprintf(stderr, "usage: %s\n", progname);
}

static void *
com_android_snep_service(void *arg)
{
  struct llc_connection *connection = (struct llc_connection *) arg;

  snepBuf *snepBuffer = connection->user_data;
  printf("Sending Ndef Message : ");
  fflush(stdout);
  write(1,snepBuffer->buffer, snepBuffer->length);

  uint8_t frame[] = {
	  //this is snep request frame
	  // Version + Response + Length + Information
    0x10, 0x02,				//version 10= Major(4) + Minor(4) Request Field: 02 = (Accept ndef massage) 
    0x00, 0x00, 0x00, 33,		//  Length field = 33 bytes
    0xd1, 0x02, 0x1c, 0x53, 0x70, 0x91, 0x01, 0x09, 0x54, 0x02,		//Information field
    0x65, 0x6e, 0x4c, 0x69, 0x62, 0x6e, 0x66, 0x63, 0x51, 0x01,		//51 = START [MB][ME][CF][SR][IL][TNF] FRAME 
																	//01 = RECORD TYPE
																	//0b = PAYLOAD
																	//55 = URI RECORD TYPE
																	//03 = http://
																	//THAN WHILE PAYLOAD FRAME (libnfc.org)
    0x0b, 0x54, 0x02, 0x65, 0x6e, 0x62, 0x6e, 0x66, 0x63, 0x2e,
    0x6f, 0x72, 0x67
  };
  uint8_t buf[1024];
  int ret;
  uint8_t ssap;

  llc_connection_send(connection, snepBuffer->buffer, sizeof(snepBuffer->buffer));

  ret = llc_connection_recv(connection, buf, sizeof(buf), &ssap);
  if(ret>0){
    printf("Send NDEF message done.\n");
  }else if(ret ==  0){
    printf("Received no data\n");
  }else{
    printf("Error received data.");
  }
  llc_connection_stop(connection);

  return NULL;
}

int
main(int argc, char *argv[])
{
  snepBuf *snepBuffer = (snepBuf *)malloc(sizeof(snepBuf));
  (void)argc;
  (void)argv;
  snepBuffer->buffer = NULL;
  snepBuffer->length = 0;
  printf("Start App.\n");
  printf("Argv[1] : %s.\n",argv[1]);

  snepBuffer->buffer = read_file(argv[1], &(snepBuffer->length));
  printf("Ndef Message : ");
  fflush(stdout);
  write(1, snepBuffer->buffer, snepBuffer->length);
  printf("\nNdef length : %d.\n",snepBuffer->length);

  nfc_context *context;
  nfc_init(&context);

  if (llcp_init() < 0)
    errx(EXIT_FAILURE, "llcp_init()");

  signal(SIGINT, stop_mac_link);
  atexit(bye);

  if (!(device = nfc_open(context, NULL))) {
    errx(EXIT_FAILURE, "Cannot connect to NFC device");
  }

  struct llc_link *llc_link = llc_link_new();
  if (!llc_link) {
    errx(EXIT_FAILURE, "Cannot allocate LLC link data structures");
  }

  mac_link = mac_link_new(device, llc_link);
  if (!mac_link){
    errx(EXIT_FAILURE, "Cannot create MAC link");
  }
  
  struct llc_service *com_android_npp;
  if (!(com_android_npp = llc_service_new(NULL, com_android_snep_service, NULL))){
    errx(EXIT_FAILURE, "Cannot create com.android.npp service");
  }
  llc_service_set_miu(com_android_npp, 512);
  llc_service_set_rw(com_android_npp, 2);

  int sap;
  if ((sap = llc_link_service_bind(llc_link, com_android_npp, 0x20)) < 0)
    errx(EXIT_FAILURE, "Cannot bind service");

//  struct llc_connection *con = llc_outgoing_data_link_connection_new_by_uri(llc_link, sap, "urn:nfc:sn:snep");
  struct llc_connection *con = llc_outgoing_data_link_connection_new(llc_link, sap, LLCP_SNEP_SAP);
  if (!con){
    errx(EXIT_FAILURE, "Cannot create llc_connection");
  }
  con->user_data = snepBuffer;

  if (mac_link_activate_as_initiator(mac_link) < 0) {
    errx(EXIT_FAILURE, "Cannot activate MAC link");
  }

  if (llc_connection_connect(con) < 0)
    errx(EXIT_FAILURE, "Cannot connect llc_connection");

  llc_connection_wait(con, NULL);

  llc_link_deactivate(llc_link);

  mac_link_free(mac_link);
  llc_link_free(llc_link);

  nfc_close(device);
  device = NULL;

  llcp_fini();
  nfc_exit(context);
  free(snepBuffer->buffer);
  free(snepBuffer);
  con->user_data = NULL;
  exit(EXIT_SUCCESS);
}
