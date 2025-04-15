#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "banco.h"

#define FIFO_PATH "/tmp/fifo_requisicoes"
#define MAX_REQ 256

pthread_mutex_t mutex_banco;

void* process_input(void *arg) {
    char *requisicao = (char *)arg;
    char resposta[100] = "";
    int id;
    char nome[50];

    pthread_mutex_lock(&mutex_banco); // Protege acesso ao banco

    if (sscanf(requisicao, "INSERT id=%d nome=%s", &id, nome) == 2) {
        if (insertLine(id, nome) == 0) {
            saveChangesInFile();  // <-- só aqui
            sprintf(resposta, "INSERT: OK\n");
        } else
            sprintf(resposta, "INSERT: ERRO (ID duplicado ou limite)\n");
    } else if (sscanf(requisicao, "DELETE id=%d", &id) == 1) {
        if (deleteLine(id) == 0) {
            saveChangesInFile();  // <-- aqui também
            sprintf(resposta, "DELETE: OK\n");
        } else
            sprintf(resposta, "DELETE: ERRO (não encontrado)\n");
    } else if (sscanf(requisicao, "SELECT id=%d", &id) == 1) {
        if (selectLine(id, nome) == 0)
            sprintf(resposta, "SELECT: nome=%s\n", nome);
        else
            sprintf(resposta, "SELECT: ERRO (não encontrado)\n");
    } else if (sscanf(requisicao, "UPDATE id=%d nome=%s", &id, nome) == 2) {
        if (updateLine(id, nome) == 0) {
            saveChangesInFile();  // <-- também aqui
            sprintf(resposta, "UPDATE: OK\n");
        } else
            sprintf(resposta, "UPDATE: ERRO (não encontrado)\n");
    }    

    saveChangesInFile();
    pthread_mutex_unlock(&mutex_banco); // Libera acesso

    printf("Requisição: %sResposta: %s", requisicao, resposta);
    free(requisicao);
    pthread_exit(NULL);
}

int main() {
    initialize_txt();
    pthread_mutex_init(&mutex_banco, NULL); // inicializar o uso do pthread

    mkfifo(FIFO_PATH, 0666); // Cria FIFO

    printf("Servidor pronto. Aguardando requisições...\n");

    while (1) {
        char buffer[MAX_REQ];
        int fd = open(FIFO_PATH, O_RDONLY);
        if (fd < 0) {
            perror("Erro ao abrir FIFO");
            continue;
        }

        ssize_t bytes_lidos;
        while ((bytes_lidos = read(fd, buffer, sizeof(buffer)-1)) > 0) {
            buffer[bytes_lidos] = '\0'; // Garante string terminada
            char *requisicao = strdup(buffer); // Cópia segura
            pthread_t tid;
            pthread_create(&tid, NULL, process_input, requisicao); // anexar thread
            pthread_detach(tid); // quando o process_input terminar, desanexar thread
        }  

        close(fd);
    }

    pthread_mutex_destroy(&mutex_banco); // destroi as threads disponibilizadas
    unlink(FIFO_PATH); // Remove FIFO ao final (opcional)
    return 0;
}
