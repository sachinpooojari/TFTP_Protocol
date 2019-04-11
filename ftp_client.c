//TFTP Client Program 
#include"header.h"
#include"frames.h"

#define WAIT_TIME 5
union FRAME frame;
struct sockaddr_in v;
int len,len_r;

#include "file_manage.h"  // contains send file functions 


//function ti recive the file 
void recive_file(int sfd)
{
	int block_no_buff=1; /// to store previous block number
	int r;
	int fd;
	fd_set readfds;
	struct timeval t;
	remove("temp");  //remove file if already present 
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
			printf("\n server disconnected ...\n");
			return;
		}

	}

	bzero(&frame,sizeof(frame));
	recvfrom(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v,&len_r); //recive the frame from server
	////////////////////////
	if(frame.DATA.opcode==5 )
		ERROR_frame_read(frame);  // if error frame show   else data frame
	else if(frame.DATA.opcode==3 & frame.DATA.block!=0)
	{
		if(frame.DATA.block==block_no_buff && block_no_buff!=1)   //repeate for next block if same block is treansmitted twice
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
		usleep(MS);
		sendto(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v,len); //send ackno
		//      if(strlen(frame.DATA.data)==DATA_MAX)   //recive untill data <512
		if(frame_len==DATA_MAX)   //recive untill data <512
			goto repeat;
		close(fd);
	}


}






void main(int argc,char**argv)
{
	if(argc<4)
	{	puts("./client  port_no fname option (1=RRQ 2=WRQ)");	return;	}
	int sfd;
	sfd=socket(AF_INET,SOCK_DGRAM,0);
	perror("socket");
	if(sfd<0)
		return;
	////////
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
	char fname[10];
	strcpy(fname,frame.RRQ.fname);
	recive_file(sfd);

	if(frame.DATA.opcode==3 & frame.DATA.block==0)
	{
		printf("Acknowlgment recived from server\n");

		send_file(sfd,fname);	


	}


}


