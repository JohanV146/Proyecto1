#include <time.h>

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

struct UserT {
    char id[10];
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


struct Loan *loadLoansFromFile(const char *fileName, int *numLoans) {
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
    struct Loan *loans = (struct Loan *)malloc(num * sizeof(struct Loan));

    for (int i = 0; i < num; i++) {
        cJSON *loanObj = cJSON_GetArrayItem(loansArray, i);
        cJSON *idObj = cJSON_GetObjectItem(loanObj, "id");
        cJSON *idUserObj = cJSON_GetObjectItem(loanObj, "userIdT");
        cJSON *bookIdObj = cJSON_GetObjectItem(loanObj, "bookIdT");
        cJSON *startDateObj = cJSON_GetObjectItem(loanObj, "startDate");
        cJSON *endDateObj = cJSON_GetObjectItem(loanObj, "endDate");

        if (idObj && cJSON_IsNumber(idObj) &&
            idUserObj && cJSON_IsString(idUserObj) &&
            bookIdObj && cJSON_IsNumber(bookIdObj) &&
            startDateObj && cJSON_IsString(startDateObj) &&
            endDateObj && cJSON_IsString(endDateObj)) {
            loans[i].id = idObj->valueint;
            strcpy(loans[i].idUser, idUserObj->valuestring);
            loans[i].bookId = bookIdObj->valueint;
            strcpy(loans[i].startDate, startDateObj->valuestring);
            strcpy(loans[i].endDate, endDateObj->valuestring);
        }
    }

    cJSON_Delete(root);
    *numLoans = num;
    return loans;
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

bool isAvailable2(const struct Loan *loans, int numLoans, int idBook, const char *startDateN) {
    for (int i = 0; i < numLoans; i++) {
        if (loans[i].bookId == idBook) {
            struct tm tm1 = {0}, tm2 = {0};
            const char *endDateStr = loans[i].endDate;

            if (sscanf(endDateStr, "%2d-%2d-%4d", &tm1.tm_mon, &tm1.tm_mday, &tm1.tm_year) != 3 ||
                sscanf(startDateN, "%2d-%2d-%4d", &tm2.tm_mon, &tm2.tm_mday, &tm2.tm_year) != 3) {
                return false;
            }

            // Ajusta los valores de mes y año para que sean compatibles con struct tm
            tm1.tm_mon -= 1;
            tm2.tm_mon -= 1;
            tm1.tm_year -= 1900;
            tm2.tm_year -= 1900;

            // Comparar las fechas
            time_t tiempo1 = mktime(&tm1);
            time_t tiempo2 = mktime(&tm2);

            if (tiempo1 == -1 || tiempo2 == -1) {
                return false;
            }

            if (tiempo2 >= tiempo1) {
                return true;
            }
        }
    }

    return false;
}

// Obtiene la fecha y hora actual del sistema
void getCurrentDate(char *currentDate) {
    time_t t;
    struct tm *temp;

    time(&t);
    temp = localtime(&t);

    strftime(currentDate, 11, "%m-%d-%Y", temp); // Formato MM-DD-AAAA
}

// valida si un año es bisiesto
bool isLeapYear(int year) {
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

// valida si una fecha es válida
bool isValidDate(const char *date) {
    int month, day, year;
    if (sscanf(date, "%2d-%2d-%4d", &month, &day, &year) != 3) {
        return false; // No se pudo leer mes, día y año correctamente.
    }

    if (year < 0 || month < 1 || month > 12 || day < 1) {
        return false; // Año negativo o mes/día fuera de rango.
    }

    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    if (isLeapYear(year)) {
        daysInMonth[2] = 29; // Febrero tiene 29 días en años bisiestos.
    }

    return (day >= 1 && day <= daysInMonth[month]);
}

// Función para validar el formato de una fecha (DD-MM-AAAA)
bool isValidDateFormat(const char *date) {
    if (strlen(date) != 10) {
        return false;
    }

    // Verifica que los caracteres en las posiciones específicas sean dígitos y que los separadores sean guiones (-).
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) {
            if (date[i] != '-') {
                return false;
            }
        } else {
            if (!isdigit(date[i])) {
                return false;
            }
        }
    }

    return true;
}

// valida que la fecha sea valida
bool getValidDate(char *date) {
    if (isValidDateFormat(date) && isValidDate(date)) {
        char currentDate[11];
        getCurrentDate(currentDate);

        if (strcmp(date, currentDate) >= 0) {
            return true;
        }
    }
    return false; 
}

// Valida que la fecha de finalización sea mayor o igual a la de inicio
bool isEndDateValid(const char *startDate, const char *endDate) {
    struct tm startTm, endTm;

    memset(&startTm, 0, sizeof(struct tm));
    memset(&endTm, 0, sizeof(struct tm));

    if (sscanf(startDate, "%2d-%2d-%4d", &startTm.tm_mon, &startTm.tm_mday, &startTm.tm_year) != 3 ||
        sscanf(endDate, "%2d-%2d-%4d", &endTm.tm_mon, &endTm.tm_mday, &endTm.tm_year) != 3) {
        return false;
    }

    // Ajusta los valores de mes y año para que sean compatibles con struct tm
    startTm.tm_mon -= 1;
    endTm.tm_mon -= 1;
    startTm.tm_year -= 1900;
    endTm.tm_year -= 1900;

    time_t startTime = mktime(&startTm);
    time_t endTime = mktime(&endTm);

    if (startTime == -1 || endTime == -1) {
        return false;
    }
    
    return difftime(endTime, startTime) >= 0;
}

bool existIdBook(const struct BookT *books, int numBooks, int idBook) {
    for (int i = 0; i < numBooks; i++) {
        if (books[i].id == idBook) {
            return true;
        }
    }
    return false;
}

bool existIdUser(const struct UserT *users, int numUsers, const char *idT) {
    for (int i = 0; i < numUsers; i++) {
        if (strcmp(users[i].id, idT) == 0) {
            return true;
        }
    }
    return false;
}

bool isAvailable(const struct BookT *books, int numBooks, int idBook) {
    for (int i = 0; i < numBooks; i++) {
        if (books[i].id == idBook && books[i].quantity > 0) {
            return true;
        }
    }
    return false;
}

char *returnName(const struct BookT *books, int numBooks, int idBook) {
    for (int i = 0; i < numBooks; i++) {
        if (books[i].id == idBook) {
            return books[i].name;
        }
    }
    return NULL;
}

//modifica la cantidad de libros
bool Modify(const char *fileName, const int idBook) {
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
            actual -= 1;
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

int getLastIdLoan() {
    FILE *fileId = fopen("lastLoan.txt", "r+");
    if (fileId == NULL) {
        // Si el archivo no existe, crea uno
        fileId = fopen("lastLoan.txt", "w");
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

//agrega los datos al archivo json
void addjsonLoan(const int id, const char *userIdT, const int bookIdT, const char *startDateT, const char *endDateT, const char *estado, const char *name, const char *tardia) {
    FILE *file = fopen("prestamos.json", "r+");
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        
        if (fileSize > 0) {
            fseek(file, -9, SEEK_END);
            fprintf(file, ",\n");
        } else {
            fprintf(file, "{\n");
            fprintf(file, "\t\"loans\": [\n");
        }
        
        fprintf(file, "\t\t{\n");
        fprintf(file, "\t\t\t\"id\": %d,\n", id); 
        fprintf(file, "\t\t\t\"userIdT\": \"%s\",\n", userIdT);
        fprintf(file, "\t\t\t\"bookIdT\": %d,\n", bookIdT);
        fprintf(file, "\t\t\t\"name\": \"%s\",\n", name);
        fprintf(file, "\t\t\t\"startDate\": \"%s\",\n", startDateT); 
        fprintf(file, "\t\t\t\"endDate\": \"%s\",\n", endDateT);
        fprintf(file, "\t\t\t\"estado\": \"%s\",\n", estado);
        fprintf(file, "\t\t\t\"tardia\": \"%s\"\n", tardia);
        fprintf(file, "\t\t}\n");

        fprintf(file, "\t]\n");
        fprintf(file, "}\n");

        fclose(file);

        printf("Se agrego exitosamente.\n");
    } else {
        printf("No se pudo abrir el archivo\n");
    }
}

void comprobante(const int id, const char *userIdT, const int bookIdT, const char *startDateT, const char *endDateT, const char *name){
    printf("Id de prestamo: %d\n", id);
    printf("Usuario: %s\n", userIdT);
    printf("Id de libro: %d\n", bookIdT);
    printf("Fecha inicial de prestamo: %s\n", startDateT);
    printf("Fecha final de prestamo: %s\n", endDateT);
    printf("Nombre del libro: %s\n", name);
}

void addLoan() {
    int numBooks;
    int numLoans;
    int numUser;

    struct Loan *loans = (struct Loan *)malloc(sizeof(struct Loan));
    struct BookT *books = loadBooks("libros.json", &numBooks);
    struct Loan *loansT = loadLoansFromFile("prestamos.json", &numLoans);
    struct UserT *users = loadUser("usuarios.json", &numUser);

    while (1) {
        printf("Ingrese su numero de cedula (9 digitos): ");
        scanf("%9s", loans->idUser);
        fflush(stdin);

        if (existIdUser(users, numUser, loans->idUser)) {
            break;
        } else {
            printf("El usuario no existe.\n");
        }
    }

    while (1) {
        printf("Ingrese el ID del libro: ");
        scanf("%d", &loans->bookId);
        fflush(stdin);

        if (existIdBook(books, numBooks, loans->bookId)) {
            break;
        } else {
            printf("El libro no existe.\n");
        }
    }

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

    int lastId = getLastIdLoan();
    const char *estado = "No";
    char *nameT = returnName(books, numBooks, loans->bookId);

    if (isAvailable(books, numBooks, loans->bookId) || isAvailable2(loansT, numLoans, loans->bookId, loans->startDate)) {
        Modify("libros.json", loans->bookId);
        addjsonLoan(lastId, loans->idUser, loans->bookId, loans->startDate, loans->endDate, "activo", nameT, estado);
        comprobante(lastId, loans->idUser, loans->bookId, loans->startDate, loans->endDate, nameT);
    } else {
        printf("No se puede realizar el préstamo para estas fechas.\n");
    }

    free(books);
    free(loans);
    free(users);
}