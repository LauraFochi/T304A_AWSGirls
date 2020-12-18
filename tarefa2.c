/*
Trabalho final da disciplina de Sistemas Operacionais
Tarefa 2
Grupo AWS Girls
Laura Nunes Fochi / RA: 256469 / Email: l256469@dac.unicamp.br
Myrelle Silva Lopes / RA: 242265 / Email: smyrellelopes@gmail.com

Para compilar o programa utilize:
$ gcc -o tarefa2 tarefa2.c -lpthread

Para executar o programa utilize:
$ ./tarefa2
*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 20
int contPostIt = 0; //adiciona/conta quantidade post it na mochila
sem_t acordar; //controla saída do pombo
sem_t mutex; //controla acesso a mochila do pombo

void dorme_aleatorio();
void leva_mochila_ate_B_e_volta();
void ColaPostIt();
int elaboraMensagem();

//---------------------------------
// Prototipos das funcoes para o usuário e o pombo
void *usuario(void *);
void *pombo(void *);

int VEZES;
int val=0;
int buffer[N]; //buffer para salvar as mensagens da mochila
int proxPosicaoVazia = 0;

int main(void)
{
    printf("Informe a quantidade de usuários: ");
    scanf("%d", &VEZES); //numero de instancias da thread usuarios
    printf("\n");

    //Define a semente da funcao geradora de numeros aleatorios
    srand(time(NULL));

	//iniciando os semaforos
    sem_init(&mutex, 0 , 1);
    sem_init(&acordar, 0, 0);

    pthread_t thdPmb, thdUsu[VEZES];
	for (int i=0; i<VEZES; i++) //instanciando a thread usuario pelo numero de vezes definido pelo usuario
	{
		pthread_create(&thdUsu[i], 0, (void *) usuario, NULL);
	}
    pthread_create(&thdPmb, 0, (void *) pombo, NULL);

    /* Bloqueiam a thread principal ate que as threads indicadas por
    * thdUsu e thdPmb terminem.
    */
    pthread_join(thdPmb,0);
	for (int i=0; i<VEZES; i++)
	{
		pthread_join(thdUsu[i],0);
	}
	
    printf("\n");
    exit(0);
}

void *usuario(void *p_arg)
{
    printf("usuario \n");
	contPostIt=0; //quantidade de post its colados na mochila

	while(1)
	{
		dorme_aleatorio();
        int mensagem = elaboraMensagem();
		
		sem_wait(&mutex); //apenas um usuário por vez na mochila
        ColaPostIt(mensagem);
		contPostIt++;

		sem_post(&mutex);

		if (contPostIt == N)
		{
			printf("mochila cheia\n");
			sem_post(&acordar);
		}
	}
}

void *pombo(void *p_arg) 
{
    printf("pombo \n");
	while(1)
	{
		sem_wait (&acordar);
		sem_wait (&mutex);
		printf("pombo acorda\n");

		leva_mochila_ate_B_e_volta();
		contPostIt = 0;

		sem_post (&mutex);

		for (int i=0; i<N; i++)
		{
			printf("Adic. msg %d\n", buffer[i]);
		}

        printf("volta para A \n");
	}
	pthread_exit(NULL);
}

void leva_mochila_ate_B_e_volta()
{
    printf("leva mochila ate B \n");
	sleep((rand() % 3)+1); //pausando por um tempo aleatório entre 1 e 3 segundos
}

void dorme_aleatorio()
{
    printf("dorme aleatorio \n");
	sleep((rand() % 3)+1);
}

void ColaPostIt(int mensagem)
{
	buffer[proxPosicaoVazia] = mensagem; //salvando a mensagem na proxima posicao vazia do buffer
	proxPosicaoVazia = (proxPosicaoVazia + 1) %N;
	printf("cola mensagem %d \n", mensagem);   
}

int elaboraMensagem() 
{
	val++;
    printf("\nElaborando mensagem: %d \n", val);
	return val;
}

