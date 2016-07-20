/*Equipe:   Gabriel Saldanha
			Nadny Dantas
			Victoria Aires
			Waldomiro Seabra
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


//Semaforo de controle dos barbeiros livres
sem_t barbeiros_livres;
//semaforo de controle das cadeiras ocupadas
sem_t cadeiras_ocupadas;
//numero de barbeiros
int nBarbeiros;
//numero de cadeiras
int nCadeiras;
//numero total de clientes
int nClientes;
//numero de clientes atendidos
int nClientesAtendidos=0;
//tipo de processamento
int tipo;
//cliente sendo atendido
int cl = 0;
//mutex do numero do cliente sendo atendido
pthread_mutex_t id_cl;

//tipos de processamentos
enum{ClienteRapido=1, BarbeiroRapido=2, Iguais=3};


//função BARBEIRO
void *barbeiro(void *arg){
	//obtem id da thread
	long tid = (long) arg;
	//thread executa indefinidamente
	while(1){
		printf("\033[1m\033[31mbarbeiro: %d  \033[0m\t \033[1m\033[35mvai esperar por clientes!!\n\n",tid);
		//espera clientes ocuparem cadeiras
		sem_wait(&cadeiras_ocupadas); 
		printf("\033[1m\033[31mbarbeiro: %d \033[0m\t \033[1m\033[35mcortando cabelo do cliente %d\n\n",tid,cl);
		//libera acesso ao numero do cliente dendo atendido
		pthread_mutex_unlock(&id_cl);
		//corta cabelo
		if(tipo == ClienteRapido) sleep(10);
		else if(tipo == BarbeiroRapido) sleep(1);
		else sleep(1);
		//apos cortar o cabelo barbeiro está livre
		sem_post(&barbeiros_livres);
		printf("\033[1m\033[31mbarbeiro: %d \033[0m\t \033[1m\033[35mestá livre\n\n",tid);
	}
}

//Função CLIENTE
void *cliente(void *arg){
	//valor do semáforo
	int valor;
	//obtem id da thread
	long tid = (long)arg;
	//obtem vaor do semáforo que representa as cadeiras ocupadas
	sem_getvalue(&cadeiras_ocupadas,&valor);
	printf("\033[1m\033[31mCliente: %d \033[0m\t \033[1m\033[35mchegou, verificando se há cadeiras livres!!\n\n\033[0m",tid);
	//se tem cadeiras livres ocupa uma e espera barbeiro
	if(valor < nCadeiras){
		printf("\033[1m\033[31mCliente: %d \033[0m\t \033[1m\033[35mhá cadeiras livres, ocupando cadeira!!\n\n",tid);
		//diz que ocupou uma cadeira
		sem_post(&cadeiras_ocupadas);
		//espera barbeiro ficar livre
		printf("\033[1m\033[31mCliente: %d \033[0m\t \033[1m\033[35mesperando barbeiro ficar livre!!\n\n",tid);
		sem_wait(&barbeiros_livres);
		//bloqueia acesso do id do cliente sendo atendido para poder ser exibido no barbeiro
		pthread_mutex_lock(&id_cl);
		cl = tid;
		//tem cabelo cortado
		printf("\033[1m\033[31mCliente: %d \033[0m\t \033[1m\033[35mfinalmente o barbeiro está livre, indo cortar o cabelo!!\n\n",tid);
	} else{
		//se não há espaço nas cadeiras o cliente vai embora
		printf("\033[1m\033[31mCliente: %d \033[0m\t \033[1m\033[35mnão há cadeiras livres, cliente vai embora!!\n\n",tid);
	}
	nClientesAtendidos++;
}

int main(int argc, const char *argv[])
{
	if(argc != 5){
		printf("%d\n",argc);
		printf("USO: ./barbeiro <numero de barbeiros> <numero de cadeiras> <numero de clientes> <tipo 1-Cliente mais rapido 2-Barbeiro mais rapido 3-iguais>\n");
		exit(1);
	}
	//atribui valores para as variáveis
	nBarbeiros = atoi(argv[1]);
	nCadeiras = atoi(argv[2]);
	nClientes = atoi(argv[3]);
	tipo = atoi(argv[4]);

	int i,j,aux;

	pthread_t barber[nBarbeiros];
	pthread_t clients[nClientes];
	//inicia semáforos e mutex
	sem_init(&barbeiros_livres,0,nBarbeiros);
	sem_init(&cadeiras_ocupadas,0,0);
	pthread_mutex_init(&id_cl,NULL);
	//cria barbeiros
	for (j = 0; j < nBarbeiros; j++) {
		pthread_create(&barber[j],0,barbeiro,(void*)j);
	}
	//cria clientes com um certo intervalo de tempo
	for (i = 0; i < nClientes; i++) {
		pthread_create(&clients[i],0,cliente,(void*)i);
		if(tipo == ClienteRapido) sleep(1);
		else if(tipo == BarbeiroRapido) sleep(10);
		else sleep(1);
	}

	//espera pelo termino das threads barbeiros
	for (j = 0; j < nBarbeiros; j++) {
		pthread_join(barber[j],0);
	}
	//espera pelo termino das threads clientes
	for (i = 0; i < nClientes; i++) {
		pthread_join(clients[i],0);
	}

	return 0;
}
