///TFTP Server Program 
#include"header.h"
#include"frames.h"
#define WAIT_TIME 5
union FRAME frame;
struct sockaddr_in v,v1_recive,v2_recive;
#include "file_manage.h"
void main(int argc,char**argv)
{
	if(argc<2)
	{	puts("./server port_no");	return;	}
	int sfd;
	sfd=socket(AF_INET,SOCK_DGRAM,0);
	perror("socket");
	if(sfd<0)
		return;
	////////
	int len;
	v.sin_family=AF_INET;
	v.sin_port=htons(atoi(argv[1]));
	v.sin_addr.s_addr=inet_addr("0.0.0.0");
	len=sizeof(v);
	if(bind(sfd,(struct sockaddr*)&v,len)<0)
	{
		perror("Bind");
		return;
	}
	perror("Bind");


	///////////////////////////////////////////////////////////////////////////////////////////////

	bzero(&frame,sizeof(frame));
	recvfrom(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v,&len);
	printf("Established connection with\n");
	printf("\tIP Address %s\n",inet_ntoa((v.sin_addr)));
	printf("\tPort %d \n",ntohs(v.sin_port));

	///////////////////////////////////////////////////////////////////////////////////////////////
	if(frame.RRQ.opcode==1)

	{


		RRQ_frame_read(frame);
		puts("******");
		send_file(sfd,frame.RRQ.fname);
		//ERROR_frame_read(frame);
	}
	else if(frame.WRQ.opcode==2)
	{ //WRQ request is  made by client
			remove("temp");  //remove the file if already present 
		bzero(&frame,sizeof(frame));
		RRQ_frame_read(frame);
		frame.DATA.opcode=3;
		frame.DATA.block=0;
		sendto(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v,len); // send 512 bytes

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

		int fd;


		bzero(&frame,sizeof(frame));
		recvfrom(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v,&len); //recive the frame from server
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

}
