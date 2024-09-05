
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Funzione per mostrare l'uso di strlen()
void demo_strlen() {
    char saluto[] = "Ciao\0";
    int lunghezza = strlen(saluto);
    printf("Lunghezza di '%s': %d\n", saluto, lunghezza);
}

// Funzione per mostrare l'uso di strcpy()
void demo_strcpy() {
    char saluto[] = "Ciao\0";
    char destinazione[10];
    strcpy_s(destinazione, sizeof(destinazione), saluto);  // Copia di saluto in destinazione
    printf("Copia di '%s' in destinazione: %s\n", saluto, destinazione);
}

// Funzione per mostrare l'uso di strcat()
void demo_strcat() {
    char saluto[20] = "Ciao\0";
    strcat_s(saluto, sizeof(saluto), " Mondo");  // Concatenazione di " Mondo" a saluto
    printf("Concatenazione di 'Ciao' e ' Mondo': %s\n", saluto);
}

// Funzione per mostrare l'uso di strcmp()
void demo_strcmp() {
    char saluto1[] = "Ciao\0";
    char saluto2[] = "Ciao\0";
    int confronto = strcmp(saluto1, saluto2);
    printf("Confronto tra '%s' e '%s': %d\n", saluto1, saluto2, confronto);
}

// Funzione per mostrare l'uso di strdup()
void demo_strdup() {
    char saluto[] = "Ciao\0";
    char* copia_saluto = strdup(saluto);
    printf("Copia della stringa '%s' usando strdup(): %s\n", saluto, copia_saluto);
    free(copia_saluto);  // Liberazione della memoria
}

// Funzione per mostrare l'uso di strchr() e strrchr()
void demo_strchr_strrchr() {
    char saluto[] = "Ciao Mondo!\0";
    char* prima_occorrenza = strchr(saluto, 'M');
    char* ultima_occorrenza = strrchr(saluto, 'M');
    printf("Prima occorrenza di 'o' in '%s': %s\n", saluto, prima_occorrenza);
    printf("Ultima occorrenza di 'o' in '%s': %s\n", saluto, ultima_occorrenza);
}

// Funzione per mostrare l'uso di strstr()
void demo_strstr() {
    char frase[] = "Ciao Mondo!\0";
    char* sottostringa = strstr(frase, "Mon");
    printf("Ricerca di 'Mon' in '%s': %s\n", frase, sottostringa);
}

// Funzione per mostrare l'uso di strtok()
void demo_strtok() {
    char frase[] = "12,34,456,565,343,343,34,3434";
    char* next_token = NULL;
    char* token = strtok_s(frase, ",", &next_token);

    while (token != NULL) {
        printf("Token: %s\n", token);
        token = strtok_s(NULL, ",", &next_token);
    }
}

// Main function
int main() {
    //demo_strlen();
    //demo_strcpy();
    //demo_strcat();
    //demo_strcmp();
    //demo_strdup();
    //demo_strchr_strrchr();
    //demo_strstr();
    demo_strtok();

    return 0;
}
