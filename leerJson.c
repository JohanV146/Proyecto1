#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include <string.h>

cJSON *read_json_file(const char *file_path) {
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) {
        perror("Error al abrir el archivo");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *file_content = (char *)malloc(file_size + 1);
    fread(file_content, 1, file_size, fp);
    file_content[file_size] = '\0'; // Agrega el terminador de cadena

    fclose(fp);

    cJSON *json = cJSON_Parse(file_content);

    free(file_content);

    return json;
}

int main() {
    char file_path[256];

    printf("Ingrese la ruta del archivo JSON: ");
    fgets(file_path, sizeof(file_path), stdin);

    size_t len = strlen(file_path);
    if (len > 0 && file_path[len - 1] == '\n') {
        file_path[len - 1] = '\0';
    }

    cJSON *json = read_json_file(file_path);
    if (json != NULL) {
        char *json_string = cJSON_Print(json);
        printf("Archivo JSON leído:\n%s\n", json_string);
        free(json_string);
        cJSON_Delete(json);
    }

    return 0;
}
