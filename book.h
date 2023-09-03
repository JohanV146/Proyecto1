#include <stdbool.h>

struct Book {
    char *name;
    char *author;
    int year;
    char *genre;
    char *summary;
    int quantity;
};

// Abre el archivo
FILE *openJsonFile(const char *filename) {
    FILE *file = fopen(filename, "r+");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        exit(1);
    }
}


// Agrega un libro al archivo JSON
void addjsonBook(FILE *file, const int id, const char *name, const char *author, const int year, const char *genre, const char *summary, const int quantity) {
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);

    if (fileSize > 0) {
        fseek(file, -9, SEEK_END);
        fprintf(file, ",\n");
    } else {
        fprintf(file, "{\n");
        fprintf(file, "\t\"books\": [\n"); // Iniciar el arreglo "books"
    }

    fprintf(file, "\t\t{\n");
    fprintf(file, "\t\t\t\"id\": %d,\n", id); 
    fprintf(file, "\t\t\t\"name\": \"%s\",\n", name);
    fprintf(file, "\t\t\t\"author\": \"%s\",\n", author);
    fprintf(file, "\t\t\t\"year\": %d,\n", year); 
    fprintf(file, "\t\t\t\"genre\": \"%s\",\n", genre);
    fprintf(file, "\t\t\t\"summary\": \"%s\",\n", summary);
    fprintf(file, "\t\t\t\"quantity\": %d\n", quantity);
    fprintf(file, "\t\t}\n");

    fprintf(file, "\t]\n");
    fprintf(file, "}\n"); 
}

// Valida si el nombre ya existe en el archivo
bool existName(FILE *file, const char *bookName) {
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
        return false; // Error al parsear el archivo JSON
    }

    cJSON *usersArray = cJSON_GetObjectItem(root, "books");
    if (usersArray == NULL) {
        cJSON_Delete(root);
        return false;
    }

    int userCount = cJSON_GetArraySize(usersArray);
    for (int i = 0; i < userCount; i++) {
        cJSON *user = cJSON_GetArrayItem(usersArray, i);
        cJSON *idObj = cJSON_GetObjectItem(user, "name");
        if (idObj != NULL && strcmp(idObj->valuestring, bookName) == 0) {
            cJSON_Delete(root);
            return true; // Usuario ya existe en el archivo
        }
    }

    cJSON_Delete(root);
    return false;
}

int getLastId() {
    FILE *fileId = fopen("last.txt", "r+");
    if (fileId == NULL) {
        // Si el archivo no existe, crea uno
        fileId = fopen("last.txt", "w");
        if (fileId == NULL) {
            perror("Error al crear el archivo");
            exit(1);
        }
        fprintf(fileId, "1");
    }

    int lastId;
    if (fscanf(fileId, "%d", &lastId) != 1) {
        perror("Error al leer el ultimo ID");
        fclose(fileId);
        exit(1);
    }

    fclose(fileId);

    return lastId;
}

int addBook() {
    char *adress;
    adress = (char *)malloc(100 * sizeof(char));
    printf("Ingrese la direccion donde se encuentra el archivo: ");
    scanf(" %[^\n]", adress);
    fflush(stdin);
    FILE *file = fopen(adress, "r");

    if (file == NULL) {
        printf("Error al abrir el archivo.\n");
        return 1;
    }

    FILE *jsonFile = openJsonFile("libros.json");

    struct Book book;
    int lastId = getLastId();
    char line[255];
    
    while (fgets(line, sizeof(line), file) != NULL) {
        char *token = strtok(line, "|");
        if (!existName(jsonFile, token)) {
            book.name = strdup(token);

            token = strtok(NULL, "|");
            book.author = strdup(token);

            token = strtok(NULL, "|");
            book.year = atoi(token);

            token = strtok(NULL, "|");
            book.genre = strdup(token);

            token = strtok(NULL, "|");
            book.summary = strdup(token);

            token = strtok(NULL, "|");
            book.quantity = atoi(token);

            addjsonBook(jsonFile, lastId, book.name, book.author, book.year, book.genre, book.summary, book.quantity);

            free(book.name);
            free(book.author);
            free(book.genre);
            free(book.summary);

            lastId++;
        }
    }

    FILE *fileId = fopen("last.txt", "r+");
    if (fileId == NULL) {
        perror("Error al abrir el archivo");
        exit(1);
    }
    fprintf(fileId, "%d", lastId);
    fclose(fileId);

    fclose(file);
    fclose(jsonFile);

    printf("Se agregaron los libros con exito\n");

    return 0;
}
