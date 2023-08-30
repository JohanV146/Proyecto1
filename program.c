#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "cJSON.h"
#include "users.h"

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
        printf( "\n   6. Volver" );
        printf( "\n\n   Introduzca opcion (1-6): ");

        scanf( "%d", &opcion );
        fflush( stdin );

        switch ( opcion )
        {
            case 1: 
                break;

            case 2:
                addUser();
                break;

            case 3: 
                break;

            case 4: 
                break;

            case 5: 
                break;
                
            case 6: 
                break;
        }

    } while ( opcion != 6 );

    return 0;
}
