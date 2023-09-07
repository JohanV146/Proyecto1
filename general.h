struct LoanT {
    int id;
    char idUser[10];
    char estado[11];
    char *name;
    int bookId;
    char tardia[3];
    char startDate[20];
    char endDate[20];
};

//modifica el estado y si es tardio
bool ModifyDev(const char *fileName, const int loanId, const char *newEstado, const char *newTardio) {
    FILE *fileRead = fopen(fileName, "r"); // Cambiado el nombre de la variable
    if (fileRead == NULL) {
        printf("El archivo no existe\n");
        return false; 
    }

    fseek(fileRead, 0, SEEK_END);  
    long fileSize = ftell(fileRead);  
    fseek(fileRead, 0, SEEK_SET);   

    char *fileContent = (char *)malloc(fileSize + 1);

    fread(fileContent, 1, fileSize, fileRead);
    fileContent[fileSize] = '\0';

    fclose(fileRead);

    cJSON *root = cJSON_Parse(fileContent);
    free(fileContent);
    if (root == NULL) {
        printf("Error al analizar el archivo JSON\n");
        return false;
    }

    cJSON *loansArray = cJSON_GetObjectItem(root, "loans");
    if (loansArray == NULL || !cJSON_IsArray(loansArray)) {
        cJSON_Delete(root);
        printf("No se encontró un array de préstamos en el JSON\n");
        return false;
    }

    int numLoans = cJSON_GetArraySize(loansArray);
    cJSON *loanToUpdate = NULL;

    for (int i = 0; i < numLoans; i++) {
        cJSON *loanObj = cJSON_GetArrayItem(loansArray, i);
        cJSON *idObj = cJSON_GetObjectItem(loanObj, "id");

        if (idObj && cJSON_IsNumber(idObj) && idObj->valueint == loanId) {
            loanToUpdate = loanObj;
            break;
        }
    }

    if (loanToUpdate == NULL) {
        cJSON_Delete(root);
        printf("El préstamo con identificador %d no se encontró en el JSON\n", loanId);
        return false;
    }

    cJSON *estadoObj = cJSON_GetObjectItem(loanToUpdate, "estado");
    cJSON *tardioObj = cJSON_GetObjectItem(loanToUpdate, "tardio");

    if (estadoObj && cJSON_IsString(estadoObj)) {
        cJSON_DeleteItemFromObject(loanToUpdate, "estado");
        cJSON_AddStringToObject(loanToUpdate, "estado", newEstado);
    }

    if (tardioObj && cJSON_IsString(tardioObj)) {
        cJSON_DeleteItemFromObject(loanToUpdate, "tardio");
        cJSON_AddStringToObject(loanToUpdate, "tardio", newTardio);
    }

    char *new_json_str = cJSON_Print(root);

    FILE *fileWrite = fopen(fileName, "w"); // Cambiado el nombre de la variable
    if (fileWrite == NULL) {
        cJSON_Delete(root);
        printf("Error al abrir el archivo para escritura\n");
        return false;
    }
    
    fprintf(fileWrite, "%s", new_json_str);
    fclose(fileWrite);
    free(new_json_str);

    cJSON_Delete(root);
    return true;
}

struct LoanT *loadLoans(const char *fileName, int *numLoans) {
    cJSON *root = readJsonFile(fileName);
    if (root == NULL) {
        return NULL; // Error al analizar el archivo JSON
    }
    cJSON *loansArray = cJSON_GetObjectItem(root, "loans");
    if (loansArray == NULL || !cJSON_IsArray(loansArray)) {
        cJSON_Delete(root);
        return NULL;
    }
    int num = cJSON_GetArraySize(loansArray);
    struct LoanT *loans = (struct LoanT *)malloc(num * sizeof(struct LoanT));

    for (int i = 0; i < num; i++) {
        cJSON *loanObj = cJSON_GetArrayItem(loansArray, i);
        cJSON *idObj = cJSON_GetObjectItem(loanObj, "id");
        cJSON *idUserObj = cJSON_GetObjectItem(loanObj, "userIdT");
        cJSON *estadobj = cJSON_GetObjectItem(loanObj, "estado");
        cJSON *nameobj = cJSON_GetObjectItem(loanObj, "name");
        cJSON *tardiaobj = cJSON_GetObjectItem(loanObj, "tardia");
        cJSON *bookIdObj = cJSON_GetObjectItem(loanObj, "bookIdT");
        cJSON *startDateObj = cJSON_GetObjectItem(loanObj, "startDate");
        cJSON *endDateObj = cJSON_GetObjectItem(loanObj, "endDate");

        if (idObj && cJSON_IsNumber(idObj) &&
            idUserObj && cJSON_IsString(idUserObj) &&
            bookIdObj && cJSON_IsNumber(bookIdObj) &&
            startDateObj && cJSON_IsString(startDateObj) &&
            endDateObj && cJSON_IsString(endDateObj) &&
            estadobj && cJSON_IsString(estadobj) &&
            nameobj && cJSON_IsString(nameobj) &&
            tardiaobj && cJSON_IsString(tardiaobj)) {
            loans[i].id = idObj->valueint;
            strcpy(loans[i].idUser, idUserObj->valuestring);
            loans[i].bookId = bookIdObj->valueint;
            strcpy(loans[i].startDate, startDateObj->valuestring);
            strcpy(loans[i].endDate, endDateObj->valuestring);
            strcpy(loans[i].estado, estadobj->valuestring);
            loans[i].name = (char *)malloc((strlen(nameobj->valuestring) + 1) * sizeof(char));
            strcpy(loans[i].name, nameobj->valuestring);
            strcpy(loans[i].tardia, tardiaobj->valuestring);
        }
    }

    for (int i = 0; i < num; i++) {
        free(loans[i].name);
    }

    cJSON_Delete(root);
    *numLoans = num;
    return loans;
}

bool Modify2(const char *fileName, const int idBook) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("El archivo no existe\n");
        return false; 
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

    if (root == NULL) {
        return false;
    }

    cJSON *usersArray = cJSON_GetObjectItem(root, "books");
    if (usersArray == NULL) {
        cJSON_Delete(root);
        return false;
    }

    int userCount = cJSON_GetArraySize(usersArray);
    for (int i = 0; i < userCount; i++) {
        cJSON *book = cJSON_GetArrayItem(usersArray, i);
        cJSON *quantityB = cJSON_GetObjectItem(book, "quantity");
        cJSON *idBooks = cJSON_GetObjectItem(book, "id");
        if (idBooks != NULL && idBooks->type == cJSON_Number && idBooks->valueint == idBook) {
            int actual = quantityB->valueint;
            actual += 1;
            cJSON_SetNumberValue(quantityB, actual);
        }
    }

    // Convierte la estructura cJSON modificada de nuevo a una cadena JSON
    char *new_json_str = cJSON_Print(root);

    // Reabre el archivo en modo escritura y sobrescribe el contenido
    file = fopen(fileName, "w");
    fprintf(file, "%s", new_json_str);
    fclose(file);
    free(new_json_str);

    cJSON_Delete(root);
    return true;
}

const char *startD(const struct Loan *loans, int numLoans, int idLoan) {
    for (int i = 0; i < numLoans; i++) {
        if (loans[i].id == idLoan) {
            return loans[i].startDate;
        }
    }
    return NULL;
}

const int returnBookId(const struct Loan *loans, int numLoans, int idLoan) {
    for (int i = 0; i < numLoans; i++) {
        if (loans[i].id == idLoan) {
            return loans[i].bookId;
        }
    }
    return -1;
}

const char *fechaFinal(const struct Loan *loans, int numLoans, int idLoan){
    for (int i = 0; i < numLoans; i++) {
        if (loans[i].id == idLoan) {
            return loans[i].endDate;
        }
    }
    return NULL;
}

// Función para convertir una fecha de formato "MM-DD-AAAA" a "AAAA-MM-DD"
void convertirFormatoFecha(const char *fecha, char *nuevaFecha) {
    int mes, dia, anio;
    sscanf(fecha, "%2d-%2d-%4d", &mes, &dia, &anio);
    sprintf(nuevaFecha, "%04d-%02d-%02d", anio, mes, dia);
}

int calculateLoan(const char *startDate, const char *returnDate, const char *actual, const int loanId, const int bookId) {
    struct tm tm1 = {0}, tm2 = {0}, tm3 = {0};

    char nuevaFecha1[11];
    char nuevaFecha2[11];
    char nuevaFecha3[11];

    convertirFormatoFecha(startDate, nuevaFecha1);
    convertirFormatoFecha(returnDate, nuevaFecha2);
    convertirFormatoFecha(actual, nuevaFecha3);

    if (sscanf(nuevaFecha1, "%4d-%2d-%2d", &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday) != 3 ||
        sscanf(nuevaFecha2, "%4d-%2d-%2d", &tm2.tm_year, &tm2.tm_mon, &tm2.tm_mday) != 3 ||
        sscanf(nuevaFecha3, "%4d-%2d-%2d", &tm3.tm_year, &tm3.tm_mon, &tm3.tm_mday) != 3) {
        return -1;
    }

    tm1.tm_year -= 1900;
    tm2.tm_year -= 1900;
    tm3.tm_year -= 1900;
    tm1.tm_mon -= 1;
    tm2.tm_mon -= 1;
    tm3.tm_mon -= 1;

    time_t tiempo1 = mktime(&tm1);
    time_t tiempo2 = mktime(&tm2);
    time_t tiempo3 = mktime(&tm3);

    if (tiempo1 == -1 || tiempo2 == -1 || tiempo3 == -1) {
        return -1;
    }

    int daysDifference = (int)difftime(tiempo2, tiempo1) / 86400;
    int tarde = (int)(difftime(tiempo3, tiempo2) / 86400);

    if (daysDifference == 0) {
        daysDifference = 1;
    }

    if (tarde <= 0) {
        // Modificar el estado del préstamo a "finalizado" y "No" para entrega tardía
        ModifyDev("prestamos.json", loanId, "finalizado", "No");
    } else {
        // Modificar el estado del préstamo a "finalizado" y "Si" para entrega tardía
        ModifyDev("prestamos.json", loanId, "finalizado", "Si");
    }

    Modify2("libros.json", bookId);

    // Calcula la tarifa según la duración del préstamo
    if (daysDifference >= 1 && daysDifference <= 7) {
        if (tarde <= 0) {
            return 150 * daysDifference;
        } else {
            return (150 * daysDifference) + (75 * tarde);
        }
    } else if (daysDifference >= 8 && daysDifference <= 15) {
        if (tarde <= 0) {
            return 125 * daysDifference;
        } else {
            return (125 * daysDifference) + (50 * tarde);
        }
    } else if (daysDifference >= 16) {
        if (tarde <= 0) {
            return 100 * daysDifference;
        } else {
            return (100 * daysDifference) + (25 * tarde);
        }
    } else {
        return -1; // Duración de préstamo inválida
    }
}

bool validarEstado(const char *fileName, const int loanId) {
    FILE *fileRead = fopen(fileName, "r");
    if (fileRead == NULL) {
        printf("El archivo no existe\n");
        return false; 
    }

    fseek(fileRead, 0, SEEK_END);  
    long fileSize = ftell(fileRead);  
    fseek(fileRead, 0, SEEK_SET);   

    char *fileContent = (char *)malloc(fileSize + 1);

    fread(fileContent, 1, fileSize, fileRead);
    fileContent[fileSize] = '\0';

    fclose(fileRead);

    cJSON *root = cJSON_Parse(fileContent);
    free(fileContent);
    if (root == NULL) {
        printf("Error al analizar el archivo JSON\n");
        return false;
    }

    cJSON *loansArray = cJSON_GetObjectItem(root, "loans");
    if (loansArray == NULL || !cJSON_IsArray(loansArray)) {
        cJSON_Delete(root);
        printf("No se encontró un array de préstamos en el JSON\n");
        return false;
    }

    int numLoans = cJSON_GetArraySize(loansArray);
    cJSON *loanToUpdate = NULL;

    for (int i = 0; i < numLoans; i++) {
        cJSON *loanObj = cJSON_GetArrayItem(loansArray, i);
        cJSON *idObj = cJSON_GetObjectItem(loanObj, "id");

        if (idObj && cJSON_IsNumber(idObj) && idObj->valueint == loanId) {
            loanToUpdate = loanObj;
            break;
        }
    }

    if (loanToUpdate == NULL) {
        cJSON_Delete(root);
        printf("El préstamo con identificador %d no se encontró en el JSON\n", loanId);
        return false;
    }

    cJSON *estadoObj = cJSON_GetObjectItem(loanToUpdate, "estado");

    if (estadoObj && cJSON_IsString(estadoObj)) {
        const char *estado = estadoObj->valuestring;
        cJSON_Delete(root);

        // Comprobar si el estado es "activo" y retornar true, de lo contrario, retornar false
        if (strcmp(estado, "activo") == 0) {
            return true;
        } else {
            return false;
        }
    }

    cJSON_Delete(root);
    return false; // No se encontró el estado o no es una cadena
}

bool existIdLoan(const struct Loan *loans, int numLoans, int idLoan) {
    for (int i = 0; i < numLoans; i++) {
        if (loans[i].id == idLoan) {
            return true;
        }
    }
    return false;
}

// Función para procesar la devolución de un ejemplar
void returnLoan() {
    int numLoans;
    struct Loan *loans = (struct Loan *)malloc(sizeof(struct Loan));
    struct Loan *loansT = loadLoansFromFile("prestamos.json", &numLoans);

    if (numLoans == 0) {
        printf("No hay prestamos\n");
    } else{
        while (1) {
            printf("Ingrese el identificador del prestamo: ");
            scanf("%d", &loans->id);
            fflush(stdin);

            if (existIdLoan(loansT, numLoans, loans->id) && validarEstado("prestamos.json", loans->id)) {
                break;
            } else {
                printf("El prestamo no existe.\n");
            }
        }

        while (1) {
            printf("Ingrese la fecha de devolucion (MM-DD-AAAA): ");
            scanf("%19s", loans->endDate);
            fflush(stdin);

            if (getValidDate(loans->endDate)) {
                break;
            } else {
                printf("Fecha invalida.\n");
            }
        }

        const char *startD = fechaFinal(loansT, numLoans, loans->id);
        const char *returnDate = fechaFinal(loansT, numLoans, loans->id);
        const int bookidnew = returnBookId(loansT, numLoans, loans->id);
        int loanFee = calculateLoan(startD, returnDate, loans->endDate, loans->id, bookidnew);

        if (loanFee == -1) {
            printf("Error al calcular la tarifa del prestamo.\n");
        } else {

            printf("Monto a cancelar: %d\n", loanFee);
        }
    }

    free(loansT);
    free(loans);
}

bool comprueba(const char *startDate, const char *returnDate, const char *actual){
    struct tm tm1 = {0}, tm2 = {0}, tm3 = {0};

    char nuevaFecha1[11];
    char nuevaFecha2[11];
    char nuevaFecha3[11];

    convertirFormatoFecha(startDate, nuevaFecha1);
    convertirFormatoFecha(returnDate, nuevaFecha2);
    convertirFormatoFecha(actual, nuevaFecha3);

    if (sscanf(nuevaFecha1, "%4d-%2d-%2d", &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday) != 3 ||
        sscanf(nuevaFecha2, "%4d-%2d-%2d", &tm2.tm_year, &tm2.tm_mon, &tm2.tm_mday) != 3 ||
        sscanf(nuevaFecha3, "%4d-%2d-%2d", &tm3.tm_year, &tm3.tm_mon, &tm3.tm_mday) != 3) {
        return false;
    }

    tm1.tm_year -= 1900;
    tm2.tm_year -= 1900;
    tm3.tm_year -= 1900;
    tm1.tm_mon -= 1;
    tm2.tm_mon -= 1;
    tm3.tm_mon -= 1;

    time_t tiempo1 = mktime(&tm1);
    time_t tiempo2 = mktime(&tm2);
    time_t tiempo3 = mktime(&tm3);

    if (tiempo1 == -1 || tiempo2 == -1 || tiempo3 == -1) {
        return false;
    }

    int daysDifference = (int)difftime(tiempo3, tiempo1) / 86400;
    int tarde = (int)(difftime(tiempo2, tiempo3) / 86400);

    if (daysDifference >= 0 && tarde >= 0){
        return true;
    } else {
        return false;
    }
}

void historial(){
    int numLoans;
    struct LoanT *loans = (struct LoanT *)malloc(sizeof(struct LoanT));
    struct LoanT *loansT = loadLoans("prestamos.json", &numLoans);

    while (1) {
        printf("Ingrese la fecha de inicio (MM-DD-AAAA): ");
        scanf("%19s", loans->startDate);
        fflush(stdin);

        if (getValidDate(loans->startDate)) {
            break;
        } else {
            printf("Fecha invalida.\n");
        }
    }

    while (1) {
        printf("Ingrese la fecha de finalizacion (MM-DD-AAAA): ");
        scanf("%19s", loans->endDate);
        fflush(stdin);

        if (getValidDate(loans->endDate) && isEndDateValid(loans->startDate, loans->endDate)) {
            break;
        } else {
            printf("Fecha invalida.\n");
        }
    }

    for (int i = 0; i < numLoans; i++) {
        if (comprueba(loans->startDate, loans->endDate, loans[i].endDate)) {
            printf("Id de prestamo: %d\n", loans[i].id);
            printf("Usuario: %s\n", loans[i].idUser);
            printf("estado: %s\n", loans[i].estado);
            printf("Nombre del libro: %s\n", loans[i].name);
            printf("Id de libro: %d\n", loans[i].bookId);
            printf("Fecha inicial de prestamo: %s\n", loans[i].tardia);          
        }
    }
}