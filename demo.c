#include "so_stdio.h"
#include <stdio.h>


int main(){
    //fopen test
    SO_FILE *fis=so_fopen("fisier.txt", "w");
    //SO_FILE *fis2=so_fopen("fisier2", "w");
    
    //fclose test
    // int ret = so_fclose(fis2);
    // if(ret==0){
    //     printf("fisierul s-a inchis cu succes\n");
    
    // }{
    //     printf("error\n");
    // }
   // SO_FILE *fis3=so_fopen("fisier2", "w");

    //fileno test
    // int fd=so_fileno(fis);
    // printf("%d\n", fd);

    // //ftell test
    // int ret2 = so_ftell(fis);
    // printf("%d\n", ret2);

    // //ferrror test
    // int ret3=so_ferror(fis);
    // printf("%d\n", ret3);

    //feof test
    // printf("%d\n", so_feof(fis));

    // //fgetc test
    // int c;
    // while(c!=SO_EOF){
    //     c=so_fgetc(fis);
    //     printf("%c", c);
    // }


    //fputc test
    // so_fputc('c', fis);
    // so_fputc('d', fis);
    // so_fputc('d', fis);
    // so_fclose(fis);
//     //fread test

//     char * buf =(char * )malloc(3);
//     so_fread(buf,1,3,fis);
    
//     printf("%s\n", buf);
    

//     //fwrite test
   // char str[] = "Buna\n";
    
    const char *str= "Buna";
    so_fwrite(str, 1, 2, fis);

    so_fclose(fis);

}
