/*
 *
 * Value Meaning
 0 Not defined, see error message (if any).
int r;
                fd_set readfds;
                struct timeval t;
                t.tv_sec=WAIT_TIME;
                t.tv_usec=0;
                FD_ZERO(&readfds);
                FD_SET(sfd,&readfds);   // to check timout  
                r=select(sfd+1,&readfds,0,0,&t);
int r;
                fd_set readfds;
                struct timeval t;
                t.tv_sec=WAIT_TIME;
                t.tv_usec=0;
                FD_ZERO(&readfds);
                FD_SET(sfd,&readfds);   // to check timout  
                r=select(sfd+1,&readfds,0,0,&t);
                if(r<0)
                {
                        perror("\"Select\" of Ack DaTA");
                        return;
                }
                else if(r==0)
                {
                        if(++rt_count<WAIT_COUNT)
                        {
                                printf("Timout retransmitted %d\n",rt_count);
                                //return;
                                goto retransmit;
                        }
                        else
                        {
                                printf("disconnected ...");
                                return;
                        }

                }
                if(r<0)
                {
                        perror("\"Select\" of Ack DaTA");
                        return;
                }
                else if(r==0)
                {
                        if(++rt_count<WAIT_COUNT)
                        {
                                printf("Timout retransmitted %d\n",rt_count);
                                //return;
                                goto retransmit;
                        }
                        else
                        {
                                printf("disconnected ...");
                                return;
                        }

                }
 1 File not found.
 2 Access violation.
 3 Disk full or allocation exceeded.
 4 Illegal TFTP operation.
 5 Unknown transfer ID.
 6 File already exists.
 7 No such user
 */



#include"header.h"
#include"frames.h"
#define WAIT_TIME 5
#define WAIT_COUNT 3
union FRAME frame;





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
	struct sockaddr_in v,v1_recive,v2_recive;
#define WAIT_TIME 5
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


	////////

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
		//ERROR_frame_read(frame);
		char *s;
		int fd;
		fd=open(frame.RRQ.fname,0,0664); // to open file requested
		if(fd<0)
		{       
			perror("read");
			ERROR_frame_make(&frame,3,"File not Found");	
			sendto(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v,len); // send Error Frame
			return;
		}
		//      printf("fd=%d\n\n",fd);
		int i;
		int flag=0;
		static int block_no;
		char buffer[DATA_MAX];
repeat:	
		bzero(&frame,sizeof(frame));
		bzero(&buffer,sizeof(buffer));
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
		strcpy(buffer,frame.DATA.data);
		int rt_count=0;   //retransmitt count 
retransmit:
		strcpy(frame.DATA.data,buffer);
		frame.DATA.opcode=3;
		frame.DATA.block=block_no;
		printf(" %d\n",frame.DATA.block);  
		//	printf("%s\n%d",frame.DATA.data,strlen(frame.DATA.data));
		sendto(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v,len); // send 512 bytes
		bzero(&frame,sizeof(frame));



		////////select to monitor ack
		int r;
		fd_set readfds;
		struct timeval t;
		t.tv_sec=WAIT_TIME;
		t.tv_usec=0;
		FD_ZERO(&readfds);
		FD_SET(sfd,&readfds);   // to check timout  
		r=select(sfd+1,&readfds,0,0,&t);
		if(r<0)
		{
			perror("\"Select\" of Ack DaTA");
			return;
		}
		else if(r==0)
		{
			if(++rt_count<WAIT_COUNT)
			{
				printf("Timout retransmitted %d\n",rt_count);
				//return;
				goto retransmit;
			}
			else
			{
                                printf("disconnected ...");
                                return;
                        }

                }

		////////
		rt_count=0;
		recvfrom(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v1_recive,&len); //ack read
		if(v1_recive.sin_port!=v.sin_port)
		{
			ERROR_frame_make(&frame,2,"ACCESS VIOLATION");
		
			sendto(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v1_recive,len); // send 512 bytes
			puts("Error");

		}
		if(frame.ACK.opcode==05)
			ERROR_frame_read(frame);

		//	if(frame.ACK.block!=block_no)  //recived block is not the same as sent 
		//			goto retransmit;		
		if(flag==0)
		{
			block_no++;
			goto repeat;
		}

	}
	else if(frame.RRQ.opcode==2)
	{
		RRQ_frame_read(frame);
			
	
	}
///////////////////////////////////////////////////////////////////////	


	/*
repeat:
	bzero(&frame,sizeof(frame));
	recvfrom(sfd,&frame,sizeof(frame),0,(struct sockaddr*)&v,&len);
//	printf("%d\n",frame.DATA.opcode);
//	printf("%d\n",frame.DATA.block);
//	printf("%s",frame.DATA.data);
//	printf("***************************");
//	int size=strlen(frame.DATA.data);
//	printf("SIZE= %d",size);
//	if(strlen(frame.DATA.data)==DATA_MAX)
//			goto repeat;
 printf("%d,%d,%s\n",frame.ERROR.opcode,frame.ERROR.error_c,frame.ERROR.error_msg);
*/

}
