#include <stdbool.h>

struct User {
    char *id;
    char *name;
    char *address;
};

//valida que el id sea valido, que contenga 9 digitos y que los digitos sean validos
bool validateId(const char *id) {
    // Verifica si la cadena tiene exactamente 9 caracteres y si todos son dígitos
    if (strlen(id) != 9) {
        return false;
    }
    for (int i = 0; i < strlen(id); i++) {
        if (!isdigit(id[i])) {
            return false;
        }
    }
    return true;
}

//valida que el usuario no exista en el archivo json
bool existId(const char *fileName, const char *userId) {
    FILE *file = fopen(fileName, "r"); // abre el archivo
    if (file == NULL) { // verifica de que el archivo exista
        printf("El archivo no existe\n");
        return false; 
    }

    fseek(file, 0, SEEK_END);   // Mueve el puntero al final del archivo
    long fileSize = ftell(file);    // Obtiene la cantidad de bytes del archivo
    fseek(file, 0, SEEK_SET);   // El puntero regresa al inicio luego de obtener el tamaño del archivo

    char *fileContent = (char *)malloc(fileSize + 1);

    fread(fileContent, 1, fileSize, file);
    fileContent[fileSize] = '\0';

    fclose(file);

    cJSON *root = cJSON_Parse(fileContent);
    free(fileContent);

    if (root == NULL) {
        return false;
    }

    cJSON *usersArray = cJSON_GetObjectItem(root, "users");
    if (usersArray == NULL) {
        cJSON_Delete(root);
        return false;
    }

    int userCount = cJSON_GetArraySize(usersArray);
    for (int i = 0; i < userCount; i++) {
        cJSON *user = cJSON_GetArrayItem(usersArray, i);
        cJSON *idUser = cJSON_GetObjectItem(user, "id");
        if (idUser != NULL && strcmp(idUser->valuestring, userId) == 0) {
            cJSON_Delete(root);
            return true;
        }
    }

    cJSON_Delete(root);
    return false;
}

//agrega los datos al archivo json
void addjson(const char *id, const char *name, const char *address) {
    FILE *file = fopen("usuarios.json", "r+");
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        
        if (fileSize > 0) {
            fseek(file, -9, SEEK_END); // Mover el puntero 9 pasos antes del cierre del último elemento
            fprintf(file, ",\n"); // Agregar una coma y un salto de línea antes del nuevo usuario
        } else {
            fprintf(file, "{\n");
            fprintf(file, "\t\"users\": [\n"); // Iniciar el arreglo "users"
        }
        
        fprintf(file, "\t\t{\n");
        fprintf(file, "\t\t\t\"id\": \"%s\",\n", id);
        fprintf(file, "\t\t\t\"name\": \"%s\",\n", name);
        fprintf(file, "\t\t\t\"address\": \"%s\"\n", address);
        fprintf(file, "\t\t}\n");

        fprintf(file, "\t]\n"); // Cerrar el arreglo "users"
        fprintf(file, "}\n");

        fclose(file);

        printf("Se agrego exitosamente.\n");
    } else {
        printf("No se pudo abrir el archivo\n");
    }
}


//Funcion para agregar usuario
void addUser() {
    struct User *temp = (struct User *)malloc(sizeof(struct User));
    char idT[10];

    while (1) {
        printf("Ingrese su numero de cedula (9 digitos): ");
        scanf("%9s", idT);
        fflush(stdin);
        
        if (validateId(idT) && !existId("usuarios.json", idT)) {
            break; // Salir del bucle si la cédula es válida y el usuario no existe
        } else {
            if (!validateId(idT)) {
                printf("La cedula no es valida, debe ser un valor numerico de 9 digitos.\n");
            } else {
                printf("El usuario ya existe.\n");
            }
        }
    }

    temp->id = strdup(idT);

    temp->name = (char *)malloc(100 * sizeof(char));
    printf("Ingrese su nombre: ");
    scanf(" %[^\n]", temp->name);
    fflush(stdin);

    temp->address = (char *)malloc(100 * sizeof(char));
    printf("Ingrese su direccion: ");
    scanf(" %[^\n]", temp->address);
    fflush(stdin);

    addjson(temp->id, temp->name, temp->address);

    free(temp->name);
    free(temp->address);
    free(temp);
}