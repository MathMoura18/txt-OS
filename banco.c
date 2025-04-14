#include "banco.h"
#include <stdio.h>
#include <string.h>

Registro banco[MAX_REGISTROS];
int total_registros = 0;

void initialize_txt() {
    FILE *fp = fopen("banco.txt", "r");
    if (fp) {
        while (fscanf(fp, "%d %s", &banco[total_registros].id, banco[total_registros].nome) == 2) {
            total_registros++;
        }
        fclose(fp);
    }
}

int insert(int id, const char *nome) {
    for (int i = 0; i < total_registros; i++) {
        if (banco[i].id == id)
            return -1; // ID duplicado
    }

    if (total_registros < MAX_REGISTROS) {
        banco[total_registros].id = id;
        strncpy(banco[total_registros].nome, nome, 49);
        total_registros++;
        return 0;
    }

    return -1; // Limite atingido
}

int delete(int id) {
    for (int i = 0; i < total_registros; i++) {
        if (banco[i].id == id) {
            banco[i] = banco[total_registros - 1]; // Substitui com o último
            total_registros--;
            return 0;
        }
    }
    return -1;
}

int select(int id, char *nome_out) {
    for (int i = 0; i < total_registros; i++) {
        if (banco[i].id == id) {
            strcpy(nome_out, banco[i].nome);
            return 0;
        }
    }
    return -1;
}

int update(int id, const char *novo_nome) {
    for (int i = 0; i < total_registros; i++) {
        if (banco[i].id == id) {
            strncpy(banco[i].nome, novo_nome, 49);
            return 0;
        }
    }
    return -1;
}

void saveChangesInFile() {
    FILE *fp = fopen("banco.txt", "w");
    if (fp) {
        for (int i = 0; i < total_registros; i++) {
            fprintf(fp, "%d %s\n", banco[i].id, banco[i].nome);
        }
        fclose(fp);
    }
}
