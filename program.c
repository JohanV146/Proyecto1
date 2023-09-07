#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "cJSON.h"
#include "users.h"
#include "book.h"
#include "loan2.h"
#include "search.h"
#include "general.h"

void menuGeneral(){
    int opcion;
    do
    {
        printf( "\n   1. Busqueda simple");
        printf( "\n   2. Busqueda avanzada");
        printf( "\n   3. Prestamo de ejemplar");
        printf( "\n   4. Devolucion de ejemplar");
        printf( "\n   5. Volver" );
        printf( "\n\n   Introduzca opcion (1-5): ");

        scanf( "%d", &opcion );
        fflush( stdin );

        switch ( opcion )
        {
            case 1: 
                searchBooks();
                break;

            case 2:
                advancedSearchBooks();
                break;

            case 3: 
                addLoan();
                break;

            case 4: 
                returnLoan();
                break;

            case 5: 
                break;
        }
    } while ( opcion != 5 );
}

int main()
{
    int opcion;

    do
    {
        printf( "\n   1. Gestion de catalogo");
        printf( "\n   2. Gestion de usuarios");
        printf( "\n   3. Historial de prestamos");
        printf( "\n   4. Vencimiento de prestamos");
        printf( "\n   5. Estadisticas");
        printf( "\n   6. Opciones generales");
        printf( "\n   7. Volver" );
        printf( "\n\n   Introduzca opcion (1-6): ");

        scanf( "%d", &opcion );
        fflush( stdin );

        switch ( opcion )
        {
            case 1: 
                addBook();
                break;

            case 2:
                addUser();
                break;

            case 3: 
                historial();
                break;

            case 4: 
                printf("No se implemento");
                break;

            case 5: 
                printf("No se implemento");
                break;
                
            case 6: 
                menuGeneral();
                break;
            case 7: 
                break;
        }

    } while ( opcion != 7 );

    return 0;
}
