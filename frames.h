#define DATA_MAX 12
#define mode_size 10
#define fname_size 256
#define ERROR_MAX 20

struct RW
{
        short int opcode;  //2byte 
        char	fname[fname_size]; //fname with 1byte as 0
        char   mode[mode_size]; // mode with 1byte 0
};
struct DATA
{

        short int opcode;  //2byte 
        short int block; // 2byte block
        char    data[DATA_MAX]; // mode with 1byte 0

};
struct ACK
{
        short int opcode;
        short int block;

};
struct ERROR
{
        short int opcode;
        short int error_c;
        char error_msg[ERROR_MAX];

};


union FRAME
{
        struct RW RRQ;
      	struct RW WRQ;
        struct DATA DATA;
        struct ACK ACK;
        struct ERROR ERROR;


};


void RRQ_frame_make(union FRAME *frame,char* fname,char* mode)
{


        bzero(frame,sizeof(*frame));
        frame->RRQ.opcode=01;
        strcpy(frame->RRQ.fname,fname);
        strcpy(frame->RRQ.mode,mode);
        printf("%d,%s,%s\n",frame->RRQ.opcode,frame->RRQ.fname,frame->RRQ.mode);

}

void ERROR_frame_make(union FRAME *frame,int  err_no,char* err_msg)
{

        frame->ERROR.opcode=05;
        frame->ERROR.error_c=err_no;
        strcpy(frame->ERROR.error_msg,err_msg);
        printf("%d,%d,%s\n",frame->ERROR.opcode,frame->ERROR.error_c,frame->ERROR.error_msg);




}

void ACK_frame_make(union FRAME *frame,int block_no)
{

	bzero(frame,sizeof(*frame));
        frame->ACK.opcode=04;
        frame->ACK.block=block_no;
}
void ACK_read(union FRAME frame)
{
        printf("%d,%d\n",frame.ACK.opcode,frame.ACK.block);
	
}





void RRQ_frame_read(union FRAME frame)
{
        printf("%d\n",frame.RRQ.opcode);
        printf("%s\n",frame.RRQ.fname);
        printf("%s\n",frame.RRQ.mode);

}
void ERROR_frame_read(union FRAME frame)
{

printf("%d,%d,%s\n",frame.ERROR.opcode,frame.ERROR.error_c,frame.ERROR.error_msg);
}


