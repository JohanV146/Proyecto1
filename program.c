#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
    int opcion;

    do
    {
        printf( "\n   1. Gestión de catálogo");
        printf( "\n   2. Gestión de usuarios");
        printf( "\n   3. Historial de préstamos");
        printf( "\n   4. Vencimiento de préstamos");
        printf( "\n   5. Estadísticas");
        printf( "\n   6. Volver" );
        printf( "\n\n   Introduzca opcion (1-6): ");

        scanf( "%d", &opcion );
        fflush( stdin );

        switch ( opcion )
        {
            case 1: 
                break;

            case 2: 
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
