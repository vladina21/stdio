#include "so_stdio.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_SIZE 4096
#define READ 2
#define WRITE 3
#define READ_WRITE 4
#define LAST_WRITE 5
#define LAST_READ 6


typedef struct _so_file {
    int fileDescriptor;
    unsigned char buffer[MAX_SIZE];
    int indexBuf;
    
    int cursor;
    int flagErr;
    int flagTip;

    int fileSize;
    int tipOperatie;
    int modDeschidere;



}SO_FILE;

SO_FILE *so_fopen(const char *pathname, const char *mode){
    SO_FILE *stream=(SO_FILE*)malloc(sizeof(SO_FILE));

    if(stream==NULL){
        stream->flagErr=-1;
        //free(stream->buffer);
        free(stream);
        return NULL;
    }
    //stream->buffer=(char*)malloc(sizeof(char)*MAX_SIZE);
    stream->fileDescriptor=0;
    for(int i=0; i<MAX_SIZE; i++){
        stream->buffer[i]=0;
    }
    stream->flagErr=0;
    stream->flagTip=0;
    stream->indexBuf=0;
    stream->fileSize=0;
    stream->cursor=0;
    stream->tipOperatie=0;
    stream->modDeschidere=0;
   

    stream->cursor=0;
    if(strcmp(mode, "r")==0){
       
            stream->fileDescriptor=open(pathname, O_RDONLY, 0777);
            if(stream->fileDescriptor==-1){
                free(stream);
                return NULL;
            }
            //printf("file descriptor : %d \n", stream->fileDescriptor);
            stream->modDeschidere=READ;
            return stream;

   


    }else if(strcmp(mode, "r+")==0){

            stream->fileDescriptor=open(pathname, O_RDWR);
            if(stream->fileDescriptor==-1){
                free(stream);
                return NULL;
            }
            //printf("file descriptor : %d \n", stream->fileDescriptor);
            stream->modDeschidere=READ_WRITE;
            return stream;
        

    }else if(strcmp(mode, "w")==0){

        stream->fileDescriptor=open(pathname, O_WRONLY | O_CREAT | O_TRUNC , 0777); 
        //printf("file descriptor : %d \n", stream->fileDescriptor);
        stream->modDeschidere=WRITE;
        return stream;

    }else if(strcmp(mode, "w+")==0){
        stream->fileDescriptor=open(pathname, O_RDWR | O_TRUNC | O_CREAT, 0777);
        //printf("file descriptor : %d \n", stream->fileDescriptor);
        stream->modDeschidere=READ_WRITE;
        return stream;

    }else if(strcmp(mode, "a")==0){
        stream->fileDescriptor=open(pathname, O_APPEND, O_CREAT, 0777);
        stream->cursor=lseek(stream->fileDescriptor, 0, SEEK_END);
        stream->modDeschidere=WRITE;
        return stream;

    }else if(strcmp(mode, "a+")==0){
        stream->fileDescriptor=open(pathname, O_APPEND, O_RDONLY, O_CREAT, 0777);
        stream->modDeschidere=READ_WRITE;
        return stream;

    }
    else
        free(stream);
        return NULL;
}
    
int so_fclose(SO_FILE *stream)
{
    if(stream==NULL){
        return SO_EOF; 
    }
    if(stream->tipOperatie == LAST_WRITE)
    {
       if( so_fflush(stream)==SO_EOF)
       {
           free(stream);
           return SO_EOF;
       }

    }
    
    //free(stream->buffer);
   
    int ret=close(stream->fileDescriptor);
    if(ret == -1)
    {
        free(stream);
        return SO_EOF;
    }

    free(stream);
    return 0;
    
}

int so_fileno(SO_FILE *stream)
{
    int fd=stream->fileDescriptor;
    return fd;


}

long so_ftell(SO_FILE *stream){
    
    return stream->cursor;

}

int so_feof(SO_FILE *stream){

    int current=lseek(stream->fileDescriptor, 0, SEEK_CUR);
    if(stream->cursor==lseek(stream->fileDescriptor, 0, SEEK_END)){


        return -1;

    }else
    {
        lseek(stream->fileDescriptor, current, SEEK_SET);
        //printf("eroare la feof\n");
        return 0;
    }
    
}

int so_ferror(SO_FILE *stream){
    if(stream->flagErr==0){
        return -0;
    }else
        return -1;
}

int so_fflush(SO_FILE *stream){

    if(stream->tipOperatie==LAST_WRITE){
        write(stream->fileDescriptor, stream->buffer, stream->indexBuf);
        stream->indexBuf=0;
        return 0;

    }else{
        stream->flagErr=-1;
        stream->indexBuf=0;
        return SO_EOF;

    
    }
}

int so_fseek(SO_FILE *stream, long offset, int whence){
    stream->cursor=lseek(stream->fileDescriptor, offset, whence);


}
int so_fgetc(SO_FILE *stream){

//am ajuns la finalul bufferului
if(stream->tipOperatie==LAST_WRITE){
    
    so_fflush(stream);
}

if(stream->modDeschidere==WRITE){
    stream->flagErr=-1;
    return SO_EOF;

}else{
    if((stream->indexBuf%MAX_SIZE)==0){
        stream->fileSize=read(stream->fileDescriptor, stream->buffer, MAX_SIZE);
        stream->indexBuf=0;
    }

    if(stream->fileSize<MAX_SIZE){
        if(stream->indexBuf>=stream->fileSize){
            return SO_EOF;
        }      
    }

    stream->tipOperatie=LAST_READ;
    return stream->buffer[stream->indexBuf++];
    

}
    
}

int so_fputc(int c, SO_FILE *stream){

 
    if(stream->tipOperatie==READ){
        stream->flagErr=-1;
        return SO_EOF;
    }else
    {
        if(stream->tipOperatie==LAST_READ){
        stream->indexBuf=0;
        
        }
    stream->tipOperatie=LAST_WRITE;

        if(stream->indexBuf==MAX_SIZE){
            so_fflush(stream);
        }
        
    stream->buffer[stream->indexBuf]=(char)c;
    stream->indexBuf++;
    return c;

    }
   
    
}

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream){

    int i;
    unsigned char* buf=(unsigned char*)malloc(sizeof(unsigned char)*size*nmemb);
    for(i=0; i<size*nmemb; i++){
        buf[i]=so_fgetc(stream);
        if(buf[i]==SO_EOF){
            return 0;
        }
  
        // (char*)ptr[i]= c;
        // (char*)ptr++;
       
     
    }


    memcpy(ptr,buf,size*nmemb);
    free(buf);
    return i;
    //so_fflush(stream);
}

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream){
    unsigned char* buf=(unsigned char*)malloc(sizeof(unsigned char)*size*nmemb);
    memcpy(buf,ptr,size*nmemb);
    
    if(ptr==NULL){
        stream->flagErr=-1;
        free(stream);
        return 0;
    }

    if(stream==NULL){
        free(stream);
        return 0;
    }
       

    if (!size || !nmemb )
    {        
        stream->flagErr=1;
        free(stream);
        return 0;
    }
    int i;
    for(i=0; i<size*nmemb; i++){
        
        int err=so_fputc(buf[i], stream);
        if(err==SO_EOF){
            stream->flagErr=-1;
            break;
        }
    }

    
    free(buf);
 

 }


SO_FILE *so_popen(const char *command, const char *type){

}

int so_pclose(SO_FILE *stream){
    
}
