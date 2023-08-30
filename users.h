struct User {
    char *id;
    char *name;
    char *address;
};

//valida que el id sea calido, que contenga 9 diguitos y que los digitos sean validos
int isValidId(const char *input) {
    // Verifica si la cadena tiene exactamente 9 caracteres y si todos son dígitos
    if (strlen(input) != 9) {
        return 0;
    }
    for (size_t i = 0; i < strlen(input); i++) {
        if (!isdigit(input[i])) {
            return 0;
        }
    }
    return 1;
}

//valida que el usuario no exista en el archivo json
int isUserAlreadyInFile(const char *fileName, const char *userId) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        return 0; // El archivo no existe
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *fileContent = (char *)malloc(fileSize + 1);
    if (fileContent == NULL) {
        fclose(file);
        return 0; // Error en la asignación de memoria
    }

    fread(fileContent, 1, fileSize, file);
    fileContent[fileSize] = '\0';

    fclose(file);

    cJSON *root = cJSON_Parse(fileContent);
    free(fileContent);

    if (root == NULL) {
        return 0; // Error al parsear el archivo JSON
    }

    cJSON *usersArray = cJSON_GetObjectItem(root, "users");
    if (usersArray == NULL) {
        cJSON_Delete(root);
        return 0; // No se encontró el arreglo "users"
    }

    int userCount = cJSON_GetArraySize(usersArray);
    for (int i = 0; i < userCount; i++) {
        cJSON *user = cJSON_GetArrayItem(usersArray, i);
        cJSON *idObj = cJSON_GetObjectItem(user, "id");
        if (idObj != NULL && strcmp(idObj->valuestring, userId) == 0) {
            cJSON_Delete(root);
            return 1; // Usuario ya existe en el archivo
        }
    }

    cJSON_Delete(root);
    return 0; // Usuario no encontrado en el archivo
}

//agrega los datos al archivo json
int addjson(const char *id, const char *name, const char *address) {
    FILE *file = fopen("usuarios.json", "r+");
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        
        if (fileSize > 0) {
            fseek(file, -9, SEEK_END); // Mover el puntero tres pasos antes del cierre del último elemento
            fprintf(file, ",\n"); // Agregar una coma y un salto de línea antes del nuevo usuario
        } else {
            fprintf(file, "{\n"); // Abrir el objeto JSON
            fprintf(file, "\t\"users\": [\n"); // Iniciar el arreglo "users"
        }
        
        fprintf(file, "\t\t{\n");
        fprintf(file, "\t\t\t\"id\": \"%s\",\n", id);
        fprintf(file, "\t\t\t\"name\": \"%s\",\n", name);
        fprintf(file, "\t\t\t\"address\": \"%s\"\n", address);
        fprintf(file, "\t\t}\n");

        if (fileSize > 0) {
            fprintf(file, "\t]\n"); // Cerrar el arreglo "users"
            fprintf(file, "}\n"); // Cerrar el objeto JSON
        } else {
            fprintf(file, "\t]\n"); // Cerrar el arreglo "users"
            fprintf(file, "}\n"); // Cerrar el objeto JSON
        }

        fclose(file);

        return 0;
    } else {
        return 1; // Error al abrir el archivo
    }
}


//Funcion para agregar usuario
void addUser() {
    struct User *temp = (struct User *)malloc(sizeof(struct User));
    char idT[10];

    if (temp == NULL) {
        printf("Error al asignar memoria.\n");
        return;
    }

    while (1) {
        printf("Ingrese su numero de cedula (9 digitos): ");
        scanf("%9s", idT);
        fflush(stdin);
        
        if (isValidId(idT) && !isUserAlreadyInFile("usuarios.json", idT)) {
            break; // Salir del bucle si la cédula es válida y el usuario no existe
        } else {
            if (!isValidId(idT)) {
                printf("Cedula no valida. Debe ser un valor numerico de 9 digitos.\n");
            } else {
                printf("El usuario ya existe.\n");
            }
        }
    }

    temp->id = strdup(idT);

    temp->name = (char *)malloc(100 * sizeof(char));
    printf("Ingrese su numero de nombre: ");
    scanf(" %[^\n]", temp->name);
    fflush(stdin);

    temp->address = (char *)malloc(100 * sizeof(char));
    printf("Ingrese su direccion: ");
    scanf(" %[^\n]", temp->address);
    fflush(stdin);

    if (addjson(temp->id, temp->name, temp->address) == 0) {
        printf("Se agrego exitosamente.\n");
    }

    free(temp->name);
    free(temp->address);
    free(temp);
}

//libera la memoria de las variables
void freeUser(struct User *user) {
    free(user->id);
    free(user->name);
    free(user->address);
    free(user);
}