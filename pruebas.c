#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "cJSON.h"

struct UserT {
    char id[10];
};

struct Loan {
    int id;
    char idUser[10];
    int bookId;
    char startDate[20];
    char endDate[20];
};

struct BookT {
    int id;
    char *name;
    int quantity;
};

cJSON *readJsonFile(const char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *fileContent = (char *)malloc(fileSize + 1);

    fread(fileContent, 1, fileSize, file);
    fileContent[fileSize] = '\0';

    fclose(file);

    cJSON *root = cJSON_Parse(fileContent);
    free(fileContent);

    return root;
}

struct BookT *loadBooks(const char *fileName, int *numBooks) {
    cJSON *root = readJsonFile(fileName);
    if (root == NULL) {
        return NULL; // Error al analizar el archivo JSON
    }

    cJSON *booksArray = cJSON_GetObjectItem(root, "books");
    if (booksArray == NULL || !cJSON_IsArray(booksArray)) {
        cJSON_Delete(root);
        return NULL; // No se encontró un array de libros en el JSON
    }

    int num = cJSON_GetArraySize(booksArray);
    struct BookT *books = (struct BookT *)malloc(num * sizeof(struct BookT));

    for (int i = 0; i < num; i++) {
        cJSON *bookObj = cJSON_GetArrayItem(booksArray, i);
        cJSON *idObj = cJSON_GetObjectItem(bookObj, "id");
        cJSON *nameObj = cJSON_GetObjectItem(bookObj, "name");
        cJSON *quantityObj = cJSON_GetObjectItem(bookObj, "quantity");

        if (idObj && cJSON_IsNumber(idObj) &&
            nameObj && cJSON_IsString(nameObj) &&
            quantityObj && cJSON_IsNumber(quantityObj)) {
            books[i].id = idObj->valueint;
            books[i].name = (char *)malloc(100 * sizeof(char));
            strcpy(books[i].name, nameObj->valuestring);
            books[i].quantity = quantityObj->valueint;
        }
    }

    cJSON_Delete(root);
    *numBooks = num;
    return books;
}

char *returnName(const struct BookT *books, int numBooks, int idBook) {
    for (int i = 0; i < numBooks; i++) {
        if (books[i].id == idBook) {
            return books[i].name;
        }
    }
    return NULL;
}

struct UserT *loadUser(const char *fileName, int *numUsers) {
    cJSON *root = readJsonFile(fileName);
    if (root == NULL) {
        return NULL; // Error al analizar el archivo JSON
    }

    cJSON *usersArray = cJSON_GetObjectItem(root, "users");
    if (usersArray == NULL || !cJSON_IsArray(usersArray)) {
        cJSON_Delete(root);
        return NULL;
    }

    int num = cJSON_GetArraySize(usersArray);
    struct UserT *users = (struct UserT *)malloc(num * sizeof(struct UserT));

    for (int i = 0; i < num; i++) {
        cJSON *userObj = cJSON_GetArrayItem(usersArray, i);
        cJSON *idUserObj = cJSON_GetObjectItem(userObj, "id");

        if (idUserObj && cJSON_IsString(idUserObj)) {
            strcpy(users[i].id, idUserObj->valuestring);
        }
    }

    cJSON_Delete(root);
    *numUsers = num;
    return users;
}

bool existIdUser(const struct UserT *users, int numUsers, const char *idT) {
    for (int i = 0; i < numUsers; i++) {
        if (strcmp(users[i].id, idT) == 0) {
            return true;
        }
    }
    return false;
}

void addLoan() {
    int numUser;
    struct Loan *loans = (struct Loan *)malloc(sizeof(struct Loan));
    struct UserT *users = loadUser("usuarios.json", &numUser);

    while (1) {
        printf("Ingrese su número de cédula (9 dígitos): ");
        scanf("%9s", loans->idUser);
        fflush(stdin);

        if (existIdUser(users, numUser, loans->idUser)) {
            break;
        } else {
            printf("El usuario no existe.\n");
        }
    }
    printf("%s\n", loans->idUser);

    free(users);
}

int main()
{
    int numBooks;
    struct BookT *books = loadBooks("libros.json", &numBooks);
    char *nameT = returnName(books, numBooks, 5);
    printf("%s", nameT);
    return 0;
}