#include <stdbool.h>

bool searchBooks(){
    char *search;
    search = (char *)malloc(100 * sizeof(char));
    printf("Ingrese el texto a buscar: ");
    scanf(" %[^\n]", search);
    fflush(stdin);

    FILE *file = fopen("libros.json", "r"); // abre el archivo

    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        free(search);
        return false;
    }

    fseek(file, 0, SEEK_SET);   // pocisiona el puntero al inicio del archivo

    char *fileContent = NULL;
    size_t fileSize = 0;

    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    fileContent = (char *)malloc(fileSize + 1);

    fread(fileContent, 1, fileSize, file);
    fileContent[fileSize] = '\0';

    cJSON *root = cJSON_Parse(fileContent);
    free(fileContent);

    if (root == NULL) {
        return false;
    }

    cJSON *booksArray = cJSON_GetObjectItem(root, "books");
    if (booksArray == NULL) {
        cJSON_Delete(root);
        return false;
    }

    int bookCount = cJSON_GetArraySize(booksArray);
    int match = 0;

    printf("Resultados de la busqueda:\n");

    for (int i = 0; i < bookCount; i++) {
        cJSON *book = cJSON_GetArrayItem(booksArray, i);
        cJSON *nameB = cJSON_GetObjectItem(book, "name");
        cJSON *authorB = cJSON_GetObjectItem(book, "author");
        cJSON *summaryB = cJSON_GetObjectItem(book, "summary");
        cJSON *quantityB = cJSON_GetObjectItem(book, "quantity");

        if (nameB != NULL && strstr(nameB->valuestring, search) != NULL ||
            authorB != NULL && strstr(authorB->valuestring, search) != NULL ||
            summaryB != NULL && strstr(summaryB->valuestring, search)){ // verifica si existe coincidencia por nombre, autor o resumen
            printf("Id: %d\n", cJSON_GetObjectItem(book, "id")->valueint);
            printf("Nombre: %s\n", nameB->valuestring);
            printf("Resumen: %s\n", summaryB->valuestring);
            if (quantityB != NULL && quantityB->valueint > 0) {
                printf("Estado: Disponible\n\n");
            } else {
                printf("Estado: No Disponible\n\n");
            }
            match++;
        }
    }
    cJSON_Delete(root); // Liberar la memoria utilizada por cJSON
    free(search);

    if (match == 0) {
        printf("No hay resultados.\n");
    }

    return true;
}

bool advancedSearchBooks(){
    char *searchName, *searchAuthor, *searchGenre, *searchSummary;
    char method[2];
    searchName = (char *)malloc(100 * sizeof(char));
    searchAuthor = (char *)malloc(100 * sizeof(char));
    searchGenre = (char *)malloc(100 * sizeof(char));
    searchSummary = (char *)malloc(100 * sizeof(char));

    printf("Se le van a solicitar datos, si no quiere hacer algun modo de busqueda escriba (No)\n\n");

    printf("Ingrese el nombre del libro: ");
    scanf(" %[^\n]", searchName);
    fflush(stdin);

    printf("Ingrese el nombre del autor: ");
    scanf(" %[^\n]", searchAuthor);
    fflush(stdin);

    printf("Ingrese el genero del libro: ");
    scanf(" %[^\n]", searchGenre);
    fflush(stdin);

    printf("Ingrese el resumen del libro: ");
    scanf(" %[^\n]", searchSummary);
    fflush(stdin);

    while (1) {
        printf("Ingrese el metodo de busqueda (1:contiene o 2:exacta): ");
        scanf(" %1s", method);
        fflush(stdin);
        
        if (strcmp(method, "1") == 0 || strcmp(method, "2") == 0){
            break;
        } else {
            printf("Solo debe ingresar 1 o 2.\n");
        }
    }

    FILE *file = fopen("libros.json", "r"); // abre el archivo

    if (file == NULL) {
        printf("Error al abrir el archivo.\n");
        return false;
    }

    fseek(file, 0, SEEK_SET);

    char *fileContent = NULL;
    size_t fileSize = 0;

    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    fileContent = (char *)malloc(fileSize + 1);

    fread(fileContent, 1, fileSize, file);
    fileContent[fileSize] = '\0';

    cJSON *root = cJSON_Parse(fileContent);
    free(fileContent);

    if (root == NULL) {
        fclose(file);
        return false;
    }

    cJSON *booksArray = cJSON_GetObjectItem(root, "books");
    if (booksArray == NULL) {
        cJSON_Delete(root);
        printf("No hay contenido en este archivo.\n");
        fclose(file);
        return false;
    }

    int bookCount = cJSON_GetArraySize(booksArray);
    int match = 0;

    printf("Resultados de la busqueda:\n");

    for (int i = 0; i < bookCount; i++) {
        cJSON *book = cJSON_GetArrayItem(booksArray, i);
        cJSON *nameB = cJSON_GetObjectItem(book, "name");
        cJSON *authorB = cJSON_GetObjectItem(book, "author");
        cJSON *genreB = cJSON_GetObjectItem(book, "genre");
        cJSON *summaryB = cJSON_GetObjectItem(book, "summary");

        bool nameMatch = true, authorMatch = true, genreMatch = true, summaryMatch = true;

        if (strcmp(method, "1") == 0){
            if (strcmp(searchName, "No") != 0 && (nameB == NULL || strstr(nameB->valuestring, searchName) == NULL)) {
                nameMatch = false;
            }

            if (strcmp(searchAuthor, "No") != 0 && (authorB == NULL || strstr(authorB->valuestring, searchAuthor) == NULL)) {
                authorMatch = false;
            }

            if (strcmp(searchGenre, "No") != 0 && (genreB == NULL || strstr(genreB->valuestring, searchGenre) == NULL)) {
                genreMatch = false;
            }

            if (strcmp(searchSummary, "No") != 0 && (summaryB == NULL || strstr(summaryB->valuestring, searchSummary) != 0)) {
                summaryMatch = false;
            }      
        } else if (strcmp(method, "2") == 0){
            if (strcmp(searchName, "no") != 0 && (nameB == NULL || strcmp(nameB->valuestring, searchName) != 0)) {
                nameMatch = false;
            }

            if (strcmp(searchAuthor, "no") != 0 && (authorB == NULL || strcmp(authorB->valuestring, searchAuthor) != 0)) {
                authorMatch = false;
            }

            if (strcmp(searchGenre, "no") != 0 && (genreB == NULL || strcmp(genreB->valuestring, searchGenre) != 0)) {
                genreMatch = false;
            }

            if (strcmp(searchSummary, "no") != 0 && (summaryB == NULL || strcmp(summaryB->valuestring, searchSummary) != 0)) {
                summaryMatch = false;
            }
        }

        if (nameMatch == true && authorMatch == true && genreMatch == true && summaryMatch == true) {
            printf("Id: %d\n", cJSON_GetObjectItem(book, "id")->valueint);
            printf("Nombre: %s\n", nameB->valuestring);
            printf("Autor: %s\n", authorB->valuestring);
            printf("Genero: %s\n", genreB->valuestring);
            printf("Resumen: %s\n", summaryB->valuestring);
            printf("\n");
            match++;
        } 
    
    }
    
    cJSON_Delete(root);
    fclose(file);

    if (match == 0) {
        printf("No hay resultados.\n");
    }
    return true;
}