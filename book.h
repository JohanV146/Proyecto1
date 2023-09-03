struct Book {
    char *name;
    char *author;
    int year;
    char *genre;
    char *summary;
    int quantity;
};

// Abre el archivo JSON y lo prepara para escritura
FILE *openJsonFile(const char *filename) {
    FILE *file = fopen(filename, "r+");
    if (file == NULL) {
        perror("Error al abrir el archivo JSON");
        exit(1);
    }
}


// Agrega un libro al archivo JSON
void addjsonBook(FILE *file, const int id, const char *name, const char *author, const int year, const char *genre, const char *summary, const int quantity) {
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);

    if (fileSize > 0) {
        long dataLength = 9; // Longitud de los datos a agregar
        fseek(file, -dataLength, SEEK_END);
        fprintf(file, ",\n");
    } else {
        fprintf(file, "{\n"); // Abrir el objeto JSON
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

// Valida si el nombre ya existe en el archivo JSON
int isNameAlreadyInFile(FILE *file, const char *bookName) {
    fseek(file, 0, SEEK_SET);   // Asegura que el puntero esté al inicio del archivo

    // Leer el contenido del archivo en una cadena de caracteres
    char *fileContent = NULL;
    size_t fileSize = 0;

    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    fileContent = (char *)malloc(fileSize + 1);
    if (fileContent == NULL) {
        fclose(file);
        return 0; // Error en la asignación de memoria
    }

    fread(fileContent, 1, fileSize, file);
    fileContent[fileSize] = '\0';

    cJSON *root = cJSON_Parse(fileContent);
    free(fileContent);

    if (root == NULL) {
        return 0; // Error al parsear el archivo JSON
    }

    cJSON *usersArray = cJSON_GetObjectItem(root, "books");
    if (usersArray == NULL) {
        cJSON_Delete(root);
        return 0; // No se encontró el arreglo "books"
    }

    int userCount = cJSON_GetArraySize(usersArray);
    for (int i = 0; i < userCount; i++) {
        cJSON *user = cJSON_GetArrayItem(usersArray, i);
        cJSON *idObj = cJSON_GetObjectItem(user, "name");
        if (idObj != NULL && strcmp(idObj->valuestring, bookName) == 0) {
            cJSON_Delete(root);
            return 1; // Usuario ya existe en el archivo
        }
    }

    cJSON_Delete(root);
    return 0; // Usuario no encontrado en el archivo
}

int getLastId() {
    FILE *idFile = fopen("last.txt", "r+");
    if (idFile == NULL) {
        // Si el archivo no existe, crea uno y establece el ID inicial en 1
        idFile = fopen("last.txt", "w");
        if (idFile == NULL) {
            perror("Error al crear el archivo last.txt");
            exit(1);
        }
        fprintf(idFile, "1");
    }

    int lastId;
    if (fscanf(idFile, "%d", &lastId) != 1) {
        perror("Error al leer el ultimo ID");
        fclose(idFile);
        exit(1);
    }

    fclose(idFile); // Cierra el archivo después de leerlo o actualizarlo

    return lastId;
}

int addBook() {
    char *direccion;
    direccion = (char *)malloc(100 * sizeof(char));
    printf("Ingrese la direccion donde se encuentra el archivo: ");
    scanf(" %[^\n]", direccion);
    fflush(stdin);
    FILE *file = fopen(direccion, "r");

    if (file == NULL) {
        printf("Error al abrir el archivo.\n");
        return 1;
    }

    FILE *jsonFile = openJsonFile("libros.json");

    struct Book book;
    int maxId = getLastId();
    char line[255];
    
    while (fgets(line, sizeof(line), file) != NULL) {
        char *token = strtok(line, "|");
        if (!isNameAlreadyInFile(jsonFile, token)) {
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

            addjsonBook(jsonFile, maxId, book.name, book.author, book.year, book.genre, book.summary, book.quantity);

            free(book.name);
            free(book.author);
            free(book.genre);
            free(book.summary);

            maxId++;
        }
    }

    FILE *idFile = fopen("last.txt", "r+");
    if (idFile == NULL) {
        perror("Error al abrir el archivo last.txt para actualizacion");
        exit(1);
    }
    fprintf(idFile, "%d", maxId); // Actualiza el ID
    fclose(idFile);

    fclose(file);
    fclose(jsonFile);

    printf("Se agregaron los libros con exito\n");

    return 0;
}
