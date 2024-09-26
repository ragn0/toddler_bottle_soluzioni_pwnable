#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

int main(){
	char* arg[101];
	for (int i = 0; i < 100; i++){
	arg[i] = "A";
}

	arg[65] = "";
	arg[66] = "\x20\x0a\x0d";
	arg[67] = "6000"; // Ho messo un po di valori a caso per cercare il primo che messo nella funzione htons avrebbe dato un numero di porta valido per il server socket
	printf("%d\n", htons(atoi(arg['C'])));
	arg[100] = NULL;
	// Cosí ho creato l'array che contiene tutti gli argomenti da passare sulla linea al programma, in bash non é possibile poiché non si puó passare un null byte \x00 da bash
	int pipaIN[2];
	int pipaERR[2];
	// Creo i due array che conterranno i descriptor delle pipe di lettura e scrittura di stdin e stderr
	if (pipe(pipaIN) || pipe(pipaERR)){
	  perror("Pipa non gita bene");
	  return -1;
	} 
        char* const env_variables[] = {"\xde\xad\xbe\xef=\xca\xfe\xba\xbe", NULL};	
	switch(fork()){ // Con fork vado a creare un sottoprocesso figlio al processo parente dove potró eseguire il programma input e duplicare le pipe da me create e alle quali scriveró gli input e portare l'input a stdin e stderr
		case -1: // forca fallita dio salame
		   perror("Forca fallita");
		   return -1;
		case 0: // fioletto
		   close(pipaIN[1]);
		   close(pipaERR[1]);
		   FILE* piezz = fopen("\x0a", "wb");
		   fwrite("\x00\x00\x00\x00", 1, 4, piezz);
		   fclose(piezz);
		   dup2(pipaIN[0], STDIN_FILENO);
		   dup2(pipaERR[0], STDERR_FILENO); // dup2 copia il vecchio file descriptor (il primo argomento), copiandolo nel nuovo (il secondo argomento) chiudendo se necessario quello nuovo e poi riaprendolo.
		   close(pipaIN[0]);
		   close(pipaERR[0]);
	           execve("/home/input2/input", arg, env_variables); // execve esegue il programma in un processo che ha lo stesso process id di quello chiamante la funzione, il primo argomento é il luogo dove sta il programma, il secondo sono gli argomenti da passare al programma e env_variables alla fine mi passa le variabili di environment e quindi mi fa passare lo stage 3
		default: // Siamo nel processo padre dove scriviamo nelle nostre pipe gli input che vogliamo.
		   close(pipaIN[0]);
		   close(pipaERR[0]);
		   write(pipaIN[1], "\x00\x0a\x00\xff", 4);   		   
		   write(pipaERR[1], "\x00\x0a\x02\xff", 4);   		   
		   wait(NULL);
}
	printf("Amm fernut wagliu");
	return 0;
}

// Flag ottenuta: Mommy! I learned how to pass various input in Linux :)

