#include <stdlib.h>
#include <stdio.h>
#include <openssl/md5.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#define _GNU_SOURCE
#include <sched.h>
#include <sys/wait.h>

#define PMODE 0644
//rajouter -lcrypto pour la compilation


//  ={0} => all the array is reset to zero (only works for zero!)

char hash[1 + 2*MD5_DIGEST_LENGTH]={0};

char* md5hash (char *str){
    unsigned char md5 [MD5_DIGEST_LENGTH]= {0};
    MD5(str, strlen(str), md5);
    for (int i=0; i<MD5_DIGEST_LENGTH;i++){
        sprintf(hash+2*i,"%02x",md5[i]);
    }
    return hash;
}

int zeros (char *s, int n ){
    size_t m=n;
    if (strlen(s)<m){
        return 0;
    }else{
        for (int i=0; i<n;i++){
            if (s[i]!='0'){
                return 0;
            }
        }
        return 1;
    }
}

void bruteforce (int first, int step, int zero){
    int f=first;
    int pid;
    int fd_cible;
    char *buff = malloc(sizeof(int)*sizeof("found."));
    char nb[sizeof(int)];
    char p[sizeof(int)];
    int* buffer;
    char* g=malloc(sizeof(char)*sizeof(char));
    do{
        pid=getpid();
        f+=step;
        sprintf(nb,"%d",f);
        g=md5hash(nb);
    }while(zeros(md5hash(nb),zero));
    sprintf(buff,"found.%d",pid);
    if (( fd_cible = open(buff, O_WRONLY|O_CREAT|O_TRUNC, PMODE)) == -1){
        perror("Ne peux creer ");
    }
    sprintf(p,"%d",pid);
    if(write(fd_cible,g,sizeof(g))==-1){
        perror("write");
    }
    if(read(fd_cible,buffer,strlen(g))){
        printf("%ls",buffer);
    }
    free(buff);
    exit(0);
}

int main(void){
    int pid[10];
    int f;
    for (int i=0; i<10;i++){
        pid[i]=fork();
        if(pid[i]==-1){
            perror("fork");
            exit(EXIT_FAILURE);
        }else if (pid[i]==0){
            bruteforce(5,6,6);
        }
    }
    f=0;
    while(!f){
        f=1;
        if(waitpid(-1,NULL,WNOHANG)==0){
            kill(-1,SIGCHLD);
            exit(0);
        }
    }
    exit(0);
}
