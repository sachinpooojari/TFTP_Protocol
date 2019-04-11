//RFC 1782

#include"header.h"
#include"frames.h"
#define WAIT_TIME 5
union FRAME frame;
void main(int argc,char**argv)
{
	if(argc<3)
	{	puts("./client  port_no fname ");	return;	}
	int sfd,fd;
	sfd=socket(AF_INET,SOCK_DGRAM,0);
	perror("socket");
	if(sfd<0)
		return;
	////////
	struct sockaddr_in v;
	int len,len_r;
	v.sin_family=AF_INET;
	v.sin_port=htons(atoi(argv[1]));
	v.sin_addr.s_addr=inet_addr("127.0.0.1");
	len=sizeof(v);
	////////////////////////////////////////////////////////////////////////

	bzero(&frame,sizeof(frame));
	if(atoi(argv[3])==1)
		RRQ_frame_make(&frame,argv[2],"netasCii");
	else if(atoi(argv[3])==2)
		WRQ_frame_make(&frame,argv[2],"netasCii");
	else
		ERROR_frame_make(&frame,3,"File not found");

	sendto(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v,len);  //send the data frame or error frame 
	//////////////////////////////////////////////////////////////////////////////////////	
	int block_no_buff=1; /// to store previous block number 
		int r;
                fd_set readfds;
                struct timeval t;
                t.tv_sec=WAIT_TIME;
                t.tv_usec=0;
                FD_ZERO(&readfds);
                FD_SET(sfd,&readfds);   // to check timout  
repeat: 
		/// to check disconnect stats if not in time 
                r=select(sfd+1,&readfds,0,0,&t);
                if(r<0)
                {
                        perror("\"Select\" of Ack DaTA");
                        return;
                }
                else if(r==0)
                {
                        {
                                printf("disconnected ...");
                                return;
                        }

		}

	


	bzero(&frame,sizeof(frame));
	recvfrom(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v,&len_r); //recive the frame from server 
////////////////////////  
	if(frame.DATA.opcode==5 )
		ERROR_frame_read(frame);  // if error frame show   else data frame 
	else if(frame.DATA.opcode==3 && frame.DATA.block==1)
	{
		if(frame.DATA.block==block_no_buff && block_no_buff!=0)   //repeate for next block if same block is treansmitted twice
			goto repeat;
		block_no_buff=frame.DATA.block;
		printf("%s",frame.DATA.data);
		int frame_len=strlen(frame.DATA.data);
		////write data into file
		fd=open("temp",O_WRONLY|O_CREAT|O_APPEND,0664);   //open file to save the recived data 
		if(fd<0)
		{               perror("create");
			return;
		}
		if(write(fd,frame.DATA.data,strlen(frame.DATA.data))<0)  //write recived data into file 
		{
			perror("write file ");	
			return;

		}

		int i;

		ACK_frame_make(&frame,frame.DATA.block); //ack with blockno
		sleep(1);
		sendto(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v,len); //send ackno  
		//	if(strlen(frame.DATA.data)==DATA_MAX)   //recive untill data <512
		if(frame_len==DATA_MAX)   //recive untill data <512
			goto repeat;
	}

	close(fd);

	//////////////////////////////////////////////////////////////////////////


	/*	int fd;
		fd=open(argv[2],0,0664);
		if(fd<0)
		{		perror("read");
		return;
		}
	//	printf("fd=%d\n\n",fd);
	int i;


	int flag=0;
repeat:
bzero(&frame,sizeof(frame));
for(i=0;i<DATA_MAX;i++)
{
char ch;
if(read(fd,&ch,sizeof(ch))==0)   //come out if EOF
{
flag=1; // to indicate EOF occureed
perror("read");
break;
}
frame.DATA.data[i]=ch;
}
frame.DATA.block=55;
	//	printf("SIze %d\n",i);	
	printf("%s\n%d",frame.DATA.data,strlen(frame.DATA.data));
	*/
	//	sendto(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v,len);
	//	if(flag==0)
	//		goto repeat;





	}



/*
 *
 * char s[20];	


 printf("Enter data\n");
 scanf("%s",s);
 sendto(sfd,s,sizeof(s),0,(struct sockaddr*)&v,len);
 printf("data :  %s\n",s);
 */


///////////////

/*
   bzero(&frame,sizeof(frame));
   frame.RRQ.opcode=01;
   strcpy(frame.RRQ.fname,argv[2]);
   strcpy(frame.RRQ.mode,"netascii");
   sendto(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v,len);
   printf("%d,%s,%s\n",frame.RRQ.opcode,frame.RRQ.fname,frame.RRQ.mode);
   */


/*
   bzero(&frame,sizeof(frame));
   frame.WRQ.opcode=02;
   strcpy(frame.WRQ.fname,argv[2]);
   strcpy(frame.WRQ.mode,"netascii");
   sendto(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v,len);
   printf("%d,%s,%s\n",frame.WRQ.opcode,frame.WRQ.fname,frame.WRQ.mode);
   */


/*
   char *s;

   int fd;
   fd=open(argv[2],0,0664);
   if(fd<0)
   {		perror("read");
   return;
   }
//	printf("fd=%d\n\n",fd);
int i;


int flag=0;
repeat:
bzero(&frame,sizeof(frame));
for(i=0;i<DATA_MAX;i++)
int flag=0;
repeat:
bzero(&frame,sizeof(frame));
for(i=0;i<DATA_MAX;i++)
{
char ch;
if(read(fd,&ch,sizeof(ch))==0)   //come out if EOF
{
flag=1; // to indicate EOF occureed
perror("read");
break;
}
frame.DATA.data[i]=ch;
}
frame.DATA.block=55;
//      printf("SIze %d\n",i);  
printf("%s\n%d",frame.DATA.data,strlen(frame.DATA.data));
sendto(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v,len);
if(flag==0)
goto repeat;
*/


/*
 *
 *
 * {
 char ch;
 if(read(fd,&ch,sizeof(ch))==0)   //come out if EOF
 {
 flag=1; // to indicate EOF occureed
 perror("read");
 break;
 }
 frame.DATA.data[i]=ch;
 }
 frame.DATA.block=55;
//	printf("SIze %d\n",i);	
printf("%s\n%d",frame.DATA.data,strlen(frame.DATA.data));
sendto(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v,len);
if(flag==0)
goto repeat;
*/	


/*
   bzero(&frame,sizeof(frame));
   frame.ERROR.opcode=05;
   frame.ERROR.error_c=2;
   strcpy(frame.ERROR.error_msg,"wrong data");
   sendto(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v,len);
   printf("%d,%d,%s\n",frame.ERROR.opcode,frame.ERROR.error_c,frame.ERROR.error_msg);

*/
/*
   bzero(&frame,sizeof(frame));
   frame.ACK.opcode=04;
   frame.ACK.block=256;
   sendto(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v,len);
   printf("%d,%d\n",frame.ACK.opcode,frame.ACK.block);
   */

