#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t barbeiros_livres;
sem_t cadeiras_ocupadas;
int nBarbeiros;
int nCadeiras;
int nClientes;
int nClientesAtendidos=0;

void *barbeiro(void *arg){
	long tid = (long) arg;
    while(nClientesAtendidos < nClientes){
	printf("barbeiro %d vai esperar por clientes!!\n",tid);
	sem_wait(&cadeiras_ocupadas); // espera por clientes
	printf("barbeiro %d cortando cabelo\n",tid);
	//corta cabelo
	sleep(10);
	sem_post(&barbeiros_livres);
	printf("barbeiro %d está livre\n",tid);
	//cortou o cabelo e agora está livre
	}
}

void *cliente(void *arg){
	int valor;
	long tid = (long)arg;
	sem_getvalue(&cadeiras_ocupadas,&valor);
	printf("Cliente %d chegou, verificando se há cadeiras livres!!\n",tid);
	if(valor < nCadeiras){
		printf("Cliente %d: há cadeiras livres, ocupando cadeira!!\n",tid);
		sem_post(&cadeiras_ocupadas);
		//espera barbeiro
		printf("Cliente %d: esperando barbeiro ficar livre!!\n",tid);
		sem_wait(&barbeiros_livres);

		//tem cabelo cortado
		printf("Cliente %d: finalmente o barbeiro está livre, indo cortar o cabelo!!\n",tid);
	} else{
		printf("não há cadeiras livres, cliente %d vai embora!!\n",tid);
	}
	nClientesAtendidos++;
}

int main(int argc, const char *argv[])
{
	nBarbeiros = atoi(argv[1]);
	nCadeiras = atoi(argv[2]);
	nClientes = atoi(argv[3]);
	int i,j,aux;
	
	pthread_t barber[nBarbeiros];
	pthread_t clients[nClientes];
	
	sem_init(&barbeiros_livres,0,nBarbeiros);
	sem_init(&cadeiras_ocupadas,0,0);
	
	for (j = 0; j < nBarbeiros; j++) {
		pthread_create(&barber[j],0,barbeiro,(void*)j);
	}
	for (i = 0; i < nClientes; i++) {
		pthread_create(&clients[i],0,cliente,(void*)i);
		sleep(1);
	}

	for (j = 0; j < nBarbeiros; j++) {
		pthread_join(barber[j],0);
	}

	for (i = 0; i < nClientes; i++) {
		pthread_join(clients[i],0);
	}
	
	return 0;
}
