#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>


sem_t semA;
sem_t semB;
sem_t semP;
sem_t mutex;


int aNum = 0;
int bNum = 0;
int inTheCar = 0;
int carId = 0;


void* fanThread(void* arg) {
    
    sem_wait(&mutex);
    printf("Thread id: %ld, Team: %s, I am looking for a car\n", pthread_self(),(char*) arg);
    if(*(char*) arg == 'A'){
        aNum += 1;
    }
    else{
        bNum += 1;
    }
    if(aNum == 4){
        sem_post(&semA);
        sem_post(&semA);
        sem_post(&semA);
        sem_post(&semA);
        aNum-=4;
    }

    else if(bNum == 4){
        sem_post(&semB);
        sem_post(&semB);
        sem_post(&semB);
        sem_post(&semB);
        bNum-=4;
    }

    else if(aNum >= 2 && bNum >=2){
        sem_post(&semA);
        sem_post(&semA);
        sem_post(&semB);
        sem_post(&semB);
        aNum-=2;
        bNum-=2;
    }
    sem_post(&mutex);
    

    if(*(char*) arg == 'A'){
        sem_wait(&semA);
    }

    else{
        sem_wait(&semB);
    }
    sem_wait(&semP);
    
    
    sem_wait(&mutex);
    printf("Thread id: %ld, Team: %s, I have found a spot in a car\n", pthread_self(),(char*) arg);
    inTheCar += 1;
    if(inTheCar == 4){
        printf("Thread id: %ld, Team: %s, I am the captain and driving the car with ID %d\n", pthread_self(),(char*) arg,carId);
        carId++;
        inTheCar = 0;
        sem_post(&semP);
        sem_post(&semP);
        sem_post(&semP);
        sem_post(&semP);
    }
    sem_post(&mutex);
    return NULL;
}

int main(int argc, const char * argv[]) {
    
    int aThreadNum = atoi(argv[1]);
    int bThreadNum = atoi(argv[2]);
    
    if(aThreadNum%2 != 0){
        printf("The main terminates\n");
        return 0;
    }
    
    if(bThreadNum%2 != 0){
        printf("The main terminates\n");
        return 0;
    }
    
    int total = aThreadNum+bThreadNum;
    
    if(total%4 != 0){
        printf("The main terminates\n");
        return 0;
    }
    
    pthread_t aThreads[aThreadNum];
    pthread_t bThreads[bThreadNum];
    
    sem_init(&semA, 0, 0);
    sem_init(&semB, 0, 0);
    sem_init(&semP, 0, 4);
    sem_init(&mutex, 0, 1);
    
    

   
    for(int i = 0;i<aThreadNum;i++){
        pthread_create(&aThreads[i],NULL,fanThread,(void*) "A");
    }
    for(int i = 0; i<bThreadNum;i++){
        pthread_create(&bThreads[i],NULL,fanThread,(void*) "B");
    }



    for(int i = 0;i<aThreadNum;i++){
        pthread_join(aThreads[i],NULL);
    }
    
    
    for(int i = 0;i<bThreadNum;i++){
        pthread_join(bThreads[i],NULL);
    }

    printf("The main terminates\n");
    
    return 0;
}
