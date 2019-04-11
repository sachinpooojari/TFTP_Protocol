

void send_file(int sfd,char  *fname)
{
	char *s;
	int len=sizeof(v);
	int fd;
	fd=open(fname,0,0664); // to open file requested
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
	static int block_no=1;
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
	//      printf("%s\n%d",frame.DATA.data,strlen(frame.DATA.data));
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
	struct sockaddr_in v1_recive;

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

	//      if(frame.ACK.block!=block_no)  //recived block is not the same as sent 
	//                      goto retransmit;                
	if(flag==0)
	{
		block_no++;
		goto repeat;
	}


}

