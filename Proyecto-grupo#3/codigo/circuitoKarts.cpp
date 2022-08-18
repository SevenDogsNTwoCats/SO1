/**
 * Simulacion de circuito de karts con semaforos binarios. Grupo#3: Alhanis Espinal, Kevin Ponce.
 * UNAH. Sistemas Operativos I - IS412. Jaime Galeas.
 * @file circuitoKarts.cpp
 * @author alhanis.espinal@unah.hn
 * @version 0.1.1
 * @date 2022-04-26
 * 
 */
#include <iostream>
#include<time.h>
#include <stdlib.h> 
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <sys/sem.h>
#include <sys/stat.h>
using namespace std;


#define PERSONAS 2+rand()%10


// Metodos para crear e inicializar los semaforos

int crearSemEspera(){
    return semget(IPC_PRIVATE, 1, IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
}

int crearSemCorriendo(){
    return semget(IPC_PRIVATE,1,IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
}

int crearSemTaquilla(){
    return semget(IPC_PRIVATE,1,IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
}

int crearSemNinos(){
    return semget(IPC_PRIVATE,1,IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR);
}

int inicializarValorSemNinos(int sem_Ninos){
    return semctl(sem_Ninos, 0, SETVAL, 1);
}

int inicializarValorSemTaquilla(int sem_taquilla){
    return semctl(sem_taquilla, 0, SETVAL, 6);
}

int inicializarValorSemEspera(int sem_espera){
    return semctl(sem_espera, 0, SETVAL, 4);
}

int inicializarValorSemCorriendoKart(int sem_corriendoKart){
    return semctl(sem_corriendoKart, 0, SETVAL, 5);
}

//metodo que se ejecutara al inicio del proceso de simulacion para determinar el clima
void determinarClima(){
	int clima = rand()%4;
	switch (clima)
	{
	case 0:
		printf("------------------------------------------------------------------------\n");
		printf("			Hoy el clima esta soleado\n");
		printf("------------------------------------------------------------------------\n");
		break;
	case 1:
		printf("------------------------------------------------------------------------\n");
		printf("			Hoy el clima esta nublado\n");
		printf("------------------------------------------------------------------------\n");
		break;
	case 2:
		printf("------------------------------------------------------------------------\n");
		printf("			Hoy el clima esta lluvioso\n");
		printf("------------------------------------------------------------------------\n");
		break;
	case 3:
		printf("------------------------------------------------------------------------\n");
		printf("			Hoy el clima esta ventoso\n");
		printf("------------------------------------------------------------------------\n");
		break;
	default:
		printf("------------------------------------------------------------------------\n");
		printf("			No se ha determinado el clima\n");
		printf("------------------------------------------------------------------------\n");
		break;
	}
}

//Metodo para el proceso de que cada persona salte el obstaculo
void pasarObstaculos(int identificador){
	int esquivarObstaculo = rand()%3;
	if (esquivarObstaculo ==0 || esquivarObstaculo ==2)
	{
		printf("La persona %i ha esquivado el obstaculo\n", identificador);
	}else if (esquivarObstaculo ==1 || esquivarObstaculo ==3)
	{
		printf("La persona %i ha colisionado con el obstaculo\n", identificador);
	}
}

//Funcion main de la ejecucion de la simulacion
int main() {

    int continuar;
	pid_t pid;
	int sem_espera, sem_corriendoKart, sem_taquilla, sem_ninos;
	int r, identificador;
    int hayNinos, esNino;
 
	struct sembuf arriba = {0, 1, 0};
	struct sembuf abajo = {0, -1, 0};

//------------------------------------CREACION DE SEMAFOROS--------------------------------------

    /**
     *Se crea un semaforo para la Taquilla y si el sistema no pudo asignar recursos para su creacion
     *se imprime en pantalla un mensaje y se finaliza la simulacion
     */
	sem_taquilla= crearSemTaquilla();
	if (sem_espera == -1) {
		perror("Imposible crear semaforo para la entrada a la sala de espera. ");
		return -1;
	}

    /**
     *Se crea un semaforo para la sala de espera y si el sistema no pudo asignar recursos para su creacion
     *se imprime en pantalla un mensaje y se finaliza la simulacion
     */
	sem_espera= crearSemEspera();
	if (sem_espera == -1) {
		perror("Imposible crear semaforo para la entrada a la sala de espera. ");
		return -1;
	}

    /**
     *Se crea un semaforo para la carrera de los karts y si el sistema no pudo asignar recursos para su creacion
     *se imprime en pantalla un mensaje y se finaliza la simulacion
     */
	sem_corriendoKart= crearSemCorriendo();
	if (sem_corriendoKart == -1) {
		perror("Imposible crear semaforo para la carrera. ");
		return -1;
	}

    /**
     *Se crea un semaforo para evitar que entren adultos si hay ninos en la pista y si el sistema no pudo asignar recursos para su creacion
     *se imprime en pantalla un mensaje y se finaliza la simulacion
     */
	sem_ninos= crearSemNinos();
	if (sem_corriendoKart == -1) {
		perror("Imposible crear semaforo para la carrera. ");
		return -1;
	}

//------------------------------------INICIALIZACION DE SEMAFOROS--------------------------------------

    /**
     * Se le define un valor para el semaforo de la carrera de los karts, si no se puede crear se 
     * imprime un mensaje de error y se finaliza la simulacion
     */
	r = inicializarValorSemTaquilla(sem_taquilla);

	if (r == -1) {
		perror("Error inicializando semaforo de la boleteria.");
		return -1;
	}

    /**
     * Se le define un valor para el semaforo de la sala de espera, si no se puede crear se 
     * imprime un mensaje de error y se finaliza la simulacion
     */
	r = inicializarValorSemEspera(sem_espera);

	if (r == -1) {
		perror("Error inicializando semaforo de la sala de espera.");
		return -1;
	}

    /**
     * Se le define un valor para el semaforo de la taquilla, si no se puede crear se 
     * imprime un mensaje de error y se finaliza la simulacion
     */
	r = inicializarValorSemCorriendoKart(sem_corriendoKart);

	if (r == -1) {
		perror("Error inicializando semaforo de la carrera.");
		return -1;
	}

    /**
     * Se le define un valor para el semaforo de ninos en la pista, si no se puede crear se 
     * imprime un mensaje de error y se finaliza la simulacion
     */
	r = inicializarValorSemNinos(sem_ninos);

	if (r == -1) {
		perror("Error inicializando semaforo de ninos en la pista.");
		return -1;
	}


    

//--------------------------------INICIO DE LA SIMULACION PARA CADA GRUPO DE PERSONA----------------------------------
    printf("\n====================== HA INGRESADO UN GRUPO AL CIRCUITO ======================\n");
	//Lo primero que se hace es determinar el clima
	determinarClima();
	for (int i = 0; i < PERSONAS; i++) {
        hayNinos = rand()%1;
        esNino = rand()%1;
        //creacion de proceso
		pid = fork();

        //En caso de error en la creacion de los subproceso
		if (pid == -1) {
			perror("Error creando procesos hijos.");
			return -1;
		}
        //En caso de exito en la creacion de los procesos
		if (pid == 0) {

			identificador = i;
            //Persona i llega a la cola
			printf("Persona %i llegando a la cola.\n", identificador);

            //Persona i compra boleto
            r = semop(sem_taquilla, &abajo, 1);
			if (r==-1) {
				perror("Error bajando semaforo");
			}
            //Persona i entra a la sala de espera
			printf("Persona %i a comprado un boleto.\n", identificador);

            r= semop(sem_espera, &arriba, 1);
			if (r == -1) {
				perror("Error subiendo semaforo de la sala de espera.");
			}
			printf("Persona %i ha salido de la boleteria.\n", identificador);

            //Entrar a la sala de espera
			r = semop(sem_espera, &abajo, 1);
			if (r==-1) {
				perror("Error bajando semaforo");
			}

            //Persona i entra a la sala de espera
			printf("Persona %i entra en la sala de espera.\n", identificador);
			sleep(1); //espera un rato

            //Entrar a correr un kart
			r = semop(sem_corriendoKart, &abajo, 1);
			if (r==-1) {
				perror("Error bajando semaforo");
			}
            //si entro un nino, se determina de manera aleatoria gracias a la variable num
            if(hayNinos ==1 && esNino ==0){
                r = semop(sem_ninos, &abajo, 1);
                if (r==-1) {
                    perror("Error bajando semaforo");
                }
                printf("Hay un nino en la pista persona %i debe esperar\n", identificador);
                sleep(2); 
				printf("El nino salio de la pista la persona %i puede igresar\n", identificador);

            }
			//Si entro a correr en el kart salir de la sala de espera
			r= semop(sem_ninos, &arriba, 1);

			if (r == -1) {
				perror("Error subiendo semaforo de que hay ninos en la pista.");
			}
            
            //Si entro a correr en el kart salir de la sala de espera
			r= semop(sem_espera, &arriba, 1);

			if (r == -1) {
				perror("Error subiendo semaforo de la sala de espera.");
			}

			printf("Persona %i corriendo kart.\n", identificador);
            //Corre unos seg el kart 
            sleep(2);
			//Salta o no el obstaculo
			pasarObstaculos(identificador);
			//Corre otros seg el kart 
            sleep(2);
            //una vez termina de correr 
			r= semop(sem_corriendoKart, &arriba, 1);

			if (r == -1) {
				perror("Error subiendo semaforo de la sala de espera.");
			}

			printf("Persona %i termina la carrera.\n", identificador);

            if(hayNinos == 2){
                r = semop(sem_ninos, &arriba, 1);
			    if (r==-1) {
				    perror("Error bajando semaforo");
			    }
            }
            

			exit(0);

		}

	}

//Gestion de los procesos
	pid = wait(NULL);

	while ( (pid != -1) ||
        ( (pid == -1) && (errno ==EINTR)))

			pid = wait(NULL);

            r = semctl(sem_taquilla, 0, IPC_RMID);

			if ( r==-1) {
				perror("Error al eliminar el semaforo de la taquilla.");
			}

			r = semctl(sem_espera, 0, IPC_RMID);

			if ( r==-1) {
				perror("Error al eliminar el semaforo de la sala de espera.");
			}

			r = semctl(sem_corriendoKart, 0, IPC_RMID);

			if ( r==-1) {
				perror("Error al eliminar el semaforo de la carrera.");
			}

            r = semctl(sem_ninos, 0, IPC_RMID);

			if ( r==-1) {
				perror("Error al eliminar el semaforo de ninos en la pista.");
			}

    printf("\nEL GRUPO HA SALIDO DEL CIRCUITO. Desea recibir otro grupo, ingrese \"1\" para continuar:\n");
    cin >> continuar;
    if(continuar==1){
        main();
        exit(0);
    }

	printf("Simulacion finalizada\n");

	return 0;

}



