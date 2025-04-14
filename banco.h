#ifndef BANCO_H
#define BANCO_H

#define MAX_REGISTROS 100
#define TAM_NOME 50

typedef struct
{
    int id;
    char nome[TAM_NOME];
} Registro;

extern Registro banco_de_dados[MAX_REGISTROS];
extern int total_registros;

void initialize_txt();
int insert(int id, const char *nome);
int delete(int id);
int select(int id, char *out_nome);
int update(int id, const char *novo_nome);
void saveChangesInFile();

#endif
